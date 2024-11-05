#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

struct Config {
    const char* ssid;
    const char* password;
    const char* apiBaseUrl;
    const char* userName;
};

const Config CONFIG = {
    "Dung", 
    "dung1109", 
    "https://ck-application-authentication.cluster-intern.site/api",  
    "admin"
};

String loginUrl = String(CONFIG.apiBaseUrl) + "/accounts/signin";
String apiRecordsUrl = String(CONFIG.apiBaseUrl) + "/records";
String apiNotificationsUrl = String(CONFIG.apiBaseUrl) + "/notifications";

String token = "";
unsigned long lastTokenRefresh = 0;
const unsigned long TOKEN_REFRESH_INTERVAL = 3600000; 
 
const char* ca_cert = R"(
-----BEGIN CERTIFICATE-----
MIIDeTCCAmGgAwIBAgIUGxif8wd3NxzQXuJzzPHJRVfDZwAwDQYJKoZIhvcNAQEL
BQAwTDELMAkGA1UEBhMCVk4xCzAJBgNVBAgMAlZOMRIwEAYDVQQHDAlIb0NoaU1p
bmgxDTALBgNVBAoMBEFnaXMxDTALBgNVBAMMBEFnaXMwHhcNMjQxMTA0MTY0MTE2
WhcNMzQxMTAyMTY0MTE2WjBMMQswCQYDVQQGEwJWTjELMAkGA1UECAwCVk4xEjAQ
BgNVBAcMCUhvQ2hpTWluaDENMAsGA1UECgwEQWdpczENMAsGA1UEAwwEQWdpczCC
ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMdEAnOLtfsFDHipIDvrR0xs
FhfRee0LgpW1odNPLSKVt2GYF/LSM+AkBgLUs3z8cTCSaTBle+HDyDJjH6PG52JT
OUnjYAiitFpOMn5T8GUuH94TkkC/i053AZeLXDonGRKbJKBZI3wzZZNLTPsAios5
R+nXQaC97V3IKql/EgUu7gTx/mM9SHLj/+5ekkRqzGJhUF9p+f22VKU1JEdOP23x
8piKKej6oD4cmaml+WHVy4gJdAYtWRVT2hyskmjF215Z8ZPMD1urd66W0zTeEEsh
wTRKg3hFPRulOV5Bd2YiabcKGwFqPn5N+kmUzaqPOqc5SsmlS2ArLmvvD4+d1JEC
AwEAAaNTMFEwHQYDVR0OBBYEFH+kzV3v61yCES4rKICMCrO09P8JMB8GA1UdIwQY
MBaAFH+kzV3v61yCES4rKICMCrO09P8JMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI
hvcNAQELBQADggEBABGOYiVDdSsVdUOoShlLFmT19VAYWvxtcHmxqpFAg1ukaIbJ
pYbKFHnyzchyJ4wJoJsCLtF5ZVO3b6ctAx6aIpnf2Qlwek+b+UOWZO0ul2i5cB0e
7vRn5ReQAdi6riQqDjmRCytzpwB+AhLS/NAOwZghqna6pKLKolXwWI46fFRHXTy8
950OG9A/T3voIGeFIFx47SEjOdvW1InA3Jcuojj9YQbRZ9Uf4Nvln+uBhjWmPPfb
DeW4+4qTRjOypO8Lpu8hPqgYJ4bvVGgOZfcuQZ5ta/P2xODoKuiePh3lnQpCxDJc
2T51HhO4X3Zx/T6uRMheCINLpW8M6oFCW3mjc8A=
-----END CERTIFICATE-----
)";  


const char* client_cert = R"(
-----BEGIN CERTIFICATE-----
MIIDwDCCAqigAwIBAgIUYGrf4fmwR/UgYDkHFnN2fNEWNKAwDQYJKoZIhvcNAQEL
BQAwTDELMAkGA1UEBhMCVk4xCzAJBgNVBAgMAlZOMRIwEAYDVQQHDAlIb0NoaU1p
bmgxDTALBgNVBAoMBEFnaXMxDTALBgNVBAMMBEFnaXMwHhcNMjQxMTA0MTY0MTE3
WhcNMzQxMTAyMTY0MTE3WjBlMQswCQYDVQQGEwJVUzETMBEGA1UECAwKQ2FsaWZv
cm5pYTEWMBQGA1UEBwwNU2FuIEZyYW5jaXNjbzESMBAGA1UECgwJTXlDb21wYW55
MRUwEwYDVQQDDAwzNC4xMzkuMTMuMzcwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAw
ggEKAoIBAQDDQrfnu8PXB+0p752DKDFp8x1kVBlbWWMTWw+9jezHNlpcj8ZmzOJ5
BfQn/Q9yvFiTOV7r1ZE6olUuef2cQtLE/7RbO7Fr9LJj9a5xx9HTfANnMA2TM9nh
xRhWiMbck3kVLbrqQ5qrErm2+uRtaFXCuLng98sF5bHPio7mZlLRr6frESohThrt
IQeyo1iv3VxXxBGSE+wOrOpafNA6xbvY+fiKcHLvqcOb2BkIT54+bAzhcejd98VU
HWTteyrXFxc+9BFA3kd2W34Wv1lGtCY2d2NLUqcCUs4KwifGaChtIecuRIfc7wN1
Z56nYafpaeIqqcHBCKCMKZpaBaUrzsN1AgMBAAGjgYAwfjAfBgNVHSMEGDAWgBR/
pM1d7+tcghEuKyiAjAqztPT/CTAJBgNVHRMEAjAAMAsGA1UdDwQEAwIE8DATBgNV
HSUEDDAKBggrBgEFBQcDATAPBgNVHREECDAGhwQiiw0lMB0GA1UdDgQWBBQmxLG/
yJ425YVYq/9Q8dhKIKLHFTANBgkqhkiG9w0BAQsFAAOCAQEAGrVPS/TopVDtQw7J
i00f2abIFj/aCr2P/K84+siXPYPci5bZ+G0lhgWggZVydVJ/LHs7n1uYkbh/ThjX
8eHfcjJTwe85xAD1pHDOje7MErMbtgQ+o+8ODFsTSN3MEL352FJeqyZkUuoTz9c4
XBd86Nb1nodApLEnb/WEpC97HC09glQ1umguQBMf/B6thcvxRXLTxsOqk8jrycyT
UzFulO95k8bUgSHtUZc7iHbWRvOW1O3n0IZycpVRuj6XtFgqZhSbu50Ap1h0vFyR
MGF6PaUpm5W8EJ5ehYBwJDLLBIiqPseYAHiSr25NWNzz4+F0sToD1TcM+ruD7cNs
bywAiA==
-----END CERTIFICATE-----
)";

const char* client_key = R"(
-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDDQrfnu8PXB+0p
752DKDFp8x1kVBlbWWMTWw+9jezHNlpcj8ZmzOJ5BfQn/Q9yvFiTOV7r1ZE6olUu
ef2cQtLE/7RbO7Fr9LJj9a5xx9HTfANnMA2TM9nhxRhWiMbck3kVLbrqQ5qrErm2
+uRtaFXCuLng98sF5bHPio7mZlLRr6frESohThrtIQeyo1iv3VxXxBGSE+wOrOpa
fNA6xbvY+fiKcHLvqcOb2BkIT54+bAzhcejd98VUHWTteyrXFxc+9BFA3kd2W34W
v1lGtCY2d2NLUqcCUs4KwifGaChtIecuRIfc7wN1Z56nYafpaeIqqcHBCKCMKZpa
BaUrzsN1AgMBAAECggEAUqkL8a4rWfyhCf/GTp/VS2UALFjt4x4UMz15jGVot2zr
FARJmXti5JkAo20d/RR+6tR+upfOw2O1a9d45Y3kbEFTuMuscGdGncqD7ucIjN8T
EbvmA0sQEeO+SePdS8OinrSdTE8SSxnCRRQxX5+rrqNFDVfZOsXiYFnn5OGsbdeR
SSbx1vDuAZanlNIUFul7YcksB0Jo2Vy9RMeWjT9HYQU8HpKnlLCTs9zU9J6xOUVD
BmHYcRuQGmDssG8S0Q0bBt24XxAYgIw5pmyPoXAdHtS/oylxGu9ljZZmx53dpdAq
PkQXQQjj+4c+MgxacCmYfWKfoGAHxrcKkg5I/KyIgQKBgQDsMp3iNCx5auByZ4Vt
El6Rlm6aftT4QyWAWx1uF62EUttugsCTP+a4kIJ3aRFEi3901wHyUUljlP1ZBvOS
vsvLYVO6DN1VNjSz+7J0ODp/3UCSEtF9JOB/Gv/6yUBxmQl85CFlmHseGCS6Rl+c
dfpuI8xE01660dWvJj9CaIODPwKBgQDToX2pYp+qfce7kqGHb3SVgKChvOS++wQa
2dQO+UKLrGNdHRsAGT76w25aTPoWvx47zwr9waU3nZo5Nkq9AKxUPorK+qSasmC3
VN2+PzKAtg9D3EvT5phk0H9BrRgEJ9Mscd3XhDrcwrsgFelsCWVVjKsGM6VkodTK
7wrvNfSwSwKBgC80po1Q9BqIG7eBrQ7xfukF8bD67EF52gLfMvCP1uP9U1Gw3CAR
w2DbsNeArizO3HhTiFYPnOVEH6YtV3zamQxatUsXyxx+quXVxgKoQjjP1VncXPXv
6tY8ZTCQDEsn1NebGHmfkeuf06dO7Ujt41Ej4m1W+TXJPjW1JSt2i+ItAoGAfppv
eHOdnPreD+Jqa4FLt1xZvCgql+3GyOFjMQoAu1oHn16YFnkD6jQbwj2mpxDyX0k/
BAticWEBMBTApUzPyx4XHdLPwhPQNBy7AuL/rWw8uZYCbuPWawkn9/w7D9FbTY1Y
72MVa5e7Jji1iYIdcw5SCcW/vfHqQPS8D9qxZP0CgYBzsT2MtydVG8s46b2ZGtTz
0rjeYePMJyMegmomRUMqfeiB0lusN7lvWjNPnYg6VCwog/Dd6surhSwjt19WqCgz
G2BQld/qOhkmahet6l0zBYbeUGrzkeeS+kuuC+jYKnuq2t7wtbrIW9ywtSxHgAe5
+S0zzAPayfrfC4u+1KARGA==
-----END PRIVATE KEY-----
)";

WiFiClientSecure client;
HTTPClient https;

void setup() {
    Serial.begin(115200);

    unsigned long startAttemptTime = millis();
    WiFi.begin(CONFIG.ssid, CONFIG.password);

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) { 
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\nFailed to connect to WiFi. Restarting...");
        ESP.restart();
        return;
    }

    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());


    client.setCACert(ca_cert);  
    client.setCertificate(client_cert); 
    client.setPrivateKey(client_key);  

    int loginAttempts = 0;
    const int maxAttempts = 3;

    while (loginAttempts < maxAttempts) {
        if (login()) {
            Serial.println("Logged in successfully");
            break;
        } else {
            loginAttempts++;
            Serial.printf("Login attempt %d failed\n", loginAttempts);
            if (loginAttempts == maxAttempts) {
                Serial.println("Login failed after multiple attempts. Restarting...");
                ESP.restart();
            }
        }
    }
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection lost. Reconnecting...");
        WiFi.reconnect();
        return;
    }
    if (millis() - lastTokenRefresh >= TOKEN_REFRESH_INTERVAL) {
        if (login()) {
            lastTokenRefresh = millis();
        } else {
            Serial.println("Token refresh failed");
            return;
        }
    }

    if (Serial.available()) {
        char receivedChar = Serial.read();
        static const char TRIGGER_CHAR = '1';
        static const String FINGERPRINT_MESSAGE = "Fingerprint Detected";
        static const String SUCCESS_STATUS = "success";

        if (receivedChar == TRIGGER_CHAR) {
            if (sendDataToRecords(FINGERPRINT_MESSAGE, SUCCESS_STATUS)) {
                Serial.println("Data sent to records successfully");
            } else {
                Serial.println("Failed to send data to records");
            }

            if (sendDataToNotifications(FINGERPRINT_MESSAGE, SUCCESS_STATUS)) {
                Serial.println("Data sent to notifications successfully");
            } else {
                Serial.println("Failed to send data to notifications");
            }
        } else {
            Serial.println("Received character is not '1', data not sent.");
        }
    }

    delay(100);
}

bool sendHttpRequest(const String& url, const String& data, const String& status, const char* dataField) {
    const int MAX_RETRIES = 3;
    int retryCount = 0;
    int httpResponseCode;

    while (retryCount < MAX_RETRIES) {
        https.begin(client, url);  
        https.addHeader("Authorization", "Bearer " + token);
        https.addHeader("Content-Type", "application/json");

        
        String sanitizedData = data;
        sanitizedData.replace("\"", "\\\"");
        String sanitizedStatus = status;
        sanitizedStatus.replace("\"", "\\\"");

        StaticJsonDocument<200> doc;
        doc[dataField] = sanitizedData;
        doc["status"] = sanitizedStatus;

        String jsonData;
        serializeJson(doc, jsonData);

        httpResponseCode = https.POST(jsonData);

        if (httpResponseCode == 200) {
            https.end();
            return true;
        }

        retryCount++;
        if (retryCount < MAX_RETRIES) {
            delay(1000 * retryCount);  
        }
    }

    Serial.printf("Error after %d retries: %s\n", 
                  MAX_RETRIES, 
                  https.errorToString(httpResponseCode).c_str());
    https.end();
    return false;
}

bool sendDataToRecords(const String& data, const String& status) {
    return sendHttpRequest(apiRecordsUrl, data, status, "recordData");
}

bool sendDataToNotifications(const String& data, const String& status) {
    return sendHttpRequest(apiNotificationsUrl, data, status, "message");
}

bool login() {
    https.begin(client, loginUrl);  
    https.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> requestDoc;
    requestDoc["userName"] = CONFIG.userName;
    requestDoc["password"] = CONFIG.password;

    String jsonData;
    serializeJson(requestDoc, jsonData);

    int httpResponseCode = https.POST(jsonData);

    if (httpResponseCode == 200) {
        String response = https.getString();
        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);

        if (error) {
            Serial.println("Failed to parse response");
            https.end();
            return false;
        }

        if (!responseDoc.containsKey("token")) {
            Serial.println("Invalid response format");
            https.end();
            return false;
        }

        token = responseDoc["token"].as<String>();
        Serial.println("Token: " + token);
        https.end();
        return true;
    } else {
        Serial.printf("Login failed, error: %s\n", https.errorToString(httpResponseCode).c_str());
        https.end();
        return false;
    }
}
