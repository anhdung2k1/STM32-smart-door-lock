#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

struct Config {
    const char* ssid;
    const char* password;
    const char* apiBaseUrl;
    const char* userName;
};

const Config CONFIG = {
    "Dung", 
    "dung1109", 
    "http://ck-application-authentication.cluster-intern.site/api",
    "admin"
};

String loginUrl = String(CONFIG.apiBaseUrl) + "/accounts/signin";
String apiRecordsUrl = String(CONFIG.apiBaseUrl) + "/records";
String apiNotificationsUrl = String(CONFIG.apiBaseUrl) + "/notifications";

String token = "";
unsigned long lastTokenRefresh = 0;
const unsigned long TOKEN_REFRESH_INTERVAL = 3600000; 

void setup() {
    Serial.begin(115200);
    // Connect to WiFi with timeout
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
    HTTPClient http;
    const int MAX_RETRIES = 3;
    int retryCount = 0;
    int httpResponseCode;

    while (retryCount < MAX_RETRIES) {
        http.begin(url);
        http.addHeader("Authorization", "Bearer " + token);
        http.addHeader("Content-Type", "application/json");

        // Sanitize input
        String sanitizedData = data;
        sanitizedData.replace("\"", "\\\"");
        String sanitizedStatus = status;
        sanitizedStatus.replace("\"", "\\\"");

        StaticJsonDocument<200> doc;
        doc[dataField] = sanitizedData;
        doc["status"] = sanitizedStatus;

        String jsonData;
        serializeJson(doc, jsonData);

        httpResponseCode = http.POST(jsonData);

        if (httpResponseCode == 200) {
            http.end();
            return true;
        }

        retryCount++;
        if (retryCount < MAX_RETRIES) {
            delay(1000 * retryCount);  
        }
    }

    Serial.printf("Error after %d retries: %s\n", 
                  MAX_RETRIES, 
                  http.errorToString(httpResponseCode).c_str());
    http.end();
    return false;
}

bool sendDataToRecords(const String& data, const String& status) {
    return sendHttpRequest(apiRecordsUrl, data, status, "recordData");
}

bool sendDataToNotifications(const String& data, const String& status) {
    return sendHttpRequest(apiNotificationsUrl, data, status, "message");
}

bool login() {
    HTTPClient http;
    http.begin(loginUrl);
    http.addHeader("Content-Type", "application/json");

    StaticJsonDocument<200> requestDoc;
    requestDoc["userName"] = CONFIG.userName;
    requestDoc["password"] = CONFIG.password;

    String jsonData;
    serializeJson(requestDoc, jsonData);

    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode == 200) {
        String response = http.getString();
        StaticJsonDocument<512> responseDoc;
        DeserializationError error = deserializeJson(responseDoc, response);

        if (error) {
            Serial.println("Failed to parse response");
            http.end();
            return false;
        }

        if (!responseDoc.containsKey("token")) {
            Serial.println("Invalid response format");
            http.end();
            return false;
        }

        token = responseDoc["token"].as<String>();
        Serial.println("Token: " + token);
        http.end();
        return true;
    } else {
        Serial.printf("Login failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
        http.end();
        return false;
    }
}