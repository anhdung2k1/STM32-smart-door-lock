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

const char* ca_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDeTCCAmGgAwIBAgIUGxif8wd3NxzQXuJzzPHJRVfDZwAwDQYJKoZIhvcNAQEL\n" \
"-----END CERTIFICATE-----\n";  

const char* client_cert = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDwDCCAqigAwIBAgIUYGrf4fmwR/UgYDkHFnN2fNEWNKAwDQYJKoZIhvcNAQEL\n" \
"-----END CERTIFICATE-----\n";  

const char* client_key = \
"-----BEGIN PRIVATE KEY-----\n" \
"MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDDQrfnu8PXB+0p\n" \
"-----END PRIVATE KEY-----\n";  

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
