#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "nguyen";        
const char* password = "12345678"; 
const char* loginUrl = "http://ck-application-authentication.cluster-intern.site/api/accounts/signin";
const char* apiRecordsUrl = "http://ck-application-authentication.cluster-intern.site/api/records"; 
const char* apiNotificationsUrl = "http://ck-application-authentication.cluster-intern.site/api/notifications"; 

String token = "";

void setup() {
    Serial.begin(115200); 

    // Kết nối WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); 

    // Đăng nhập và lấy token
    if (login()) {
        Serial.println("Logged in successfully");
    } else {
        Serial.println("Login failed");
    }
}

void loop() {
    // Gửi dữ liệu nếu có
    String fingerprintData = "Fingerprint Detected"; 
    String status = "success"; 

    if (sendDataToRecords(fingerprintData, status)) {
        Serial.println("Data sent to records successfully");
    } else {
        Serial.println("Failed to send data to records");
    }

    if (sendDataToNotifications(fingerprintData, status)) {
        Serial.println("Data sent to notifications successfully");
    } else {
        Serial.println("Failed to send data to notifications");
    }

    delay(10000); 
}

bool login() {
    HTTPClient http;
    http.begin(loginUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"userName\": \"admin\", \"password\": \"Admin@123\"}";
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode == 200) {
        String response = http.getString();
        Serial.println("Response: " + response);
        int startIndex = response.indexOf("token\":\"") + 8;
        int endIndex = response.indexOf("\"", startIndex);
        token = response.substring(startIndex, endIndex);
        Serial.println("Token: " + token);
        http.end();
        return true;
    } else {
        Serial.printf("Login failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
        http.end();
        return false;
    }
}

bool sendDataToRecords(const String& data, const String& status) {
    HTTPClient http;
    http.begin(apiRecordsUrl);
    http.addHeader("Authorization", "Bearer " + token);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"recordData\": \"" + data + "\", \"status\": \"" + status + "\"}";
    int httpResponseCode = http.POST(jsonData);
    http.end();

    if (httpResponseCode > 0) {
        return httpResponseCode == 200;
    } else {
        Serial.printf("Error on sending to records: %s\n", http.errorToString(httpResponseCode).c_str());
        return false;
    }
}

bool sendDataToNotifications(const String& data, const String& status) {
    HTTPClient http;
    http.begin(apiNotificationsUrl);
    http.addHeader("Authorization", "Bearer " + token);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"message\": \"" + data + "\", \"status\": \"" + status + "\"}";
    int httpResponseCode = http.POST(jsonData);
    http.end();

    if (httpResponseCode > 0) {
        return httpResponseCode == 200;
    } else {
        Serial.printf("Error on sending to notifications: %s\n", http.errorToString(httpResponseCode).c_str());
        return false;
    }
}
