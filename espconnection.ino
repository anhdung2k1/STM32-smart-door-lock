#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "...";        
const char* password = "..."; 
const char* serverUrl = "http://yourserver.com/api.php";

#define TX_PIN 10 // Pin truyền dữ liệu với uart3 của STM32 chân PB10
#define RX_PIN 11 // Pin nhận dữ liệu với uart3 của STM32 chân PB11

void setup() {
    Serial.begin(115200); 
    Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN); // Khởi động UART

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); 
}

void loop() {
    // Gửi dữ liệu từ STM32 đến ESP32
    if (Serial2.available()) {
        String fingerprintData = Serial2.readStringUntil('\n'); // Đọc dữ liệu từ STM32
        Serial.print("Received from STM32: ");
        Serial.println(fingerprintData);

        // Gửi dữ liệu từ ESP32 lên MySQL
        if (sendDataToServer(fingerprintData)) {
            Serial.println("Data sent to MySQL server successfully");
        } else {
            Serial.println("Failed to send data to MySQL server");
        }

        // Lấy dữ liệu từ MySQL
        String mysqlData = getDataFromServer();
        if (mysqlData.length() > 0) {
            Serial.print("Data received from MySQL: ");
            Serial.println(mysqlData);

            // Gửi dữ liệu xuống STM32
            Serial2.println(mysqlData);
        }
    }

    delay(1000); // Đợi 1 giây trước khi kiểm tra lại
}

bool sendDataToServer(const String& data) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    // Chuyển đổi dữ liệu thành JSON
    String jsonData = "{\"fingerprint_id\":\"" + data + "\"}";

    int httpResponseCode = http.POST(jsonData);
    http.end();

    // Kiểm tra mã phản hồi
    if (httpResponseCode > 0) {
        return httpResponseCode == 200;
    } else {
        Serial.printf("Error on sending POST: %s\n", http.errorToString(httpResponseCode).c_str());
        return false;
    }
}

String getDataFromServer() {
    HTTPClient http;
    http.begin(serverUrl);

    int httpResponseCode = http.GET();
    String payload;

    if (httpResponseCode > 0) {
        payload = http.getString();
    } else {
        Serial.printf("Error on sending GET: %s\n", http.errorToString(httpResponseCode).c_str());
    }

    http.end();
    return payload;
}
