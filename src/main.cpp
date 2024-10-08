#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Max6675.h"
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>


#define RXD 26  // RXD
#define TXD 22  // TXD

#define RXD2 16  // RXD
#define TXD2 17  // TXD

//Switch
const int buttonPin = 32; // กำหนดขาพินของสวิตซ์
int switchState = 0;
int lastSwitchState = 0;
int ledState = 0;
int state = 1;

// Switch reset
const int buttonPin2 = 34;
int switch_rst = 0;

//LED1
const int LED_TTL = 18;

//LED2
const int LED_RS = 19;

//LED3
const int LED_wifi = 5;

//LED4
const int LED_power = 12;

int previousState = 0; // ตัวแปรสำหรับเก็บค่า state ก่อนหน้า

char Res_Cnt=0;

// กำหนด SSID และรหัสผ่านสำหรับ Access Point
const char* apSSID = "ESP32";
const char* apPassword = "12345678";

// MQTT broker settings
const char* mqtt_server = "76a05382ac0c4728aa05e3ec258beda1.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "testmqtt"; 
const char* mqtt_password = "ESP32mqtt"; 
const char* mqtt_topic_sub = "esp32";
const char* mqtt_topic_pub = "mqtt";

// HiveMQ Cloud Let's Encrypt CA certificate
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFBTCCAu2gAwIBAgIQS6hSk/eaL6JzBkuoBI110DANBgkqhkiG9w0BAQsFADBP
MQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJuZXQgU2VjdXJpdHkgUmVzZWFy
Y2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBYMTAeFw0yNDAzMTMwMDAwMDBa
Fw0yNzAzMTIyMzU5NTlaMDMxCzAJBgNVBAYTAlVTMRYwFAYDVQQKEw1MZXQncyBF
bmNyeXB0MQwwCgYDVQQDEwNSMTAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEK
AoIBAQDPV+XmxFQS7bRH/sknWHZGUCiMHT6I3wWd1bUYKb3dtVq/+vbOo76vACFL
YlpaPAEvxVgD9on/jhFD68G14BQHlo9vH9fnuoE5CXVlt8KvGFs3Jijno/QHK20a
/6tYvJWuQP/py1fEtVt/eA0YYbwX51TGu0mRzW4Y0YCF7qZlNrx06rxQTOr8IfM4
FpOUurDTazgGzRYSespSdcitdrLCnF2YRVxvYXvGLe48E1KGAdlX5jgc3421H5KR
mudKHMxFqHJV8LDmowfs/acbZp4/SItxhHFYyTr6717yW0QrPHTnj7JHwQdqzZq3
DZb3EoEmUVQK7GH29/Xi8orIlQ2NAgMBAAGjgfgwgfUwDgYDVR0PAQH/BAQDAgGG
MB0GA1UdJQQWMBQGCCsGAQUFBwMCBggrBgEFBQcDATASBgNVHRMBAf8ECDAGAQH/
AgEAMB0GA1UdDgQWBBS7vMNHpeS8qcbDpHIMEI2iNeHI6DAfBgNVHSMEGDAWgBR5
tFnme7bl5AFzgAiIyBpY9umbbjAyBggrBgEFBQcBAQQmMCQwIgYIKwYBBQUHMAKG
Fmh0dHA6Ly94MS5pLmxlbmNyLm9yZy8wEwYDVR0gBAwwCjAIBgZngQwBAgEwJwYD
VR0fBCAwHjAcoBqgGIYWaHR0cDovL3gxLmMubGVuY3Iub3JnLzANBgkqhkiG9w0B
AQsFAAOCAgEAkrHnQTfreZ2B5s3iJeE6IOmQRJWjgVzPw139vaBw1bGWKCIL0vIo
zwzn1OZDjCQiHcFCktEJr59L9MhwTyAWsVrdAfYf+B9haxQnsHKNY67u4s5Lzzfd
u6PUzeetUK29v+PsPmI2cJkxp+iN3epi4hKu9ZzUPSwMqtCceb7qPVxEbpYxY1p9
1n5PJKBLBX9eb9LU6l8zSxPWV7bK3lG4XaMJgnT9x3ies7msFtpKK5bDtotij/l0
GaKeA97pb5uwD9KgWvaFXMIEt8jVTjLEvwRdvCn294GPDF08U8lAkIv7tghluaQh
1QnlE4SEN4LOECj8dsIGJXpGUk3aU3KkJz9icKy+aUgA+2cP21uh6NcDIS3XyfaZ
QjmDQ993ChII8SXWupQZVBiIpcWO4RqZk3lr7Bz5MUCwzDIA359e57SSq5CCkY0N
4B6Vulk7LktfwrdGNVI5BsC9qqxSwSKgRJeZ9wygIaehbHFHFhcBaMDKpiZlBHyz
rsnnlFXCb5s8HKn5LsUgGvB24L7sGNZP2CX7dhHov+YhD+jozLW2p9W4959Bz2Ei
RmqDtmiXLnzqTpXbI+suyCsohKRg6Un0RC47+cpiVwHiXZAW+cn8eiNIjqbVgXLx
KPpdzvvtTnOPlC7SQZSYmdunr3Bf9b77AiC/ZidstK36dRILKz7OA54=
-----END CERTIFICATE-----
)EOF";

// สร้างเว็บเซิร์ฟเวอร์
AsyncWebServer server(80);

// HTML ฟอร์มสำหรับรับข้อมูล SSID และรหัสผ่าน
const char* htmlForm = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 WiFi Configuration</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
      background-color: #f0f0f0;
    }
    .container {
      background-color: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      max-width: 400px;
      width: 100%;
      text-align: center;
    }
    h2 {
      margin-bottom: 20px;
      color: #333;
    }
    input[type="text"], input[type="password"] {
      width: calc(100% - 22px);
      padding: 10px;
      margin: 10px 0;
      border: 1px solid #ccc;
      border-radius: 5px;
    }
    input[type="submit"] {
      background-color: #4CAF50;
      color: white;
      border: none;
      border-radius: 5px;
      padding: 10px 20px;
      cursor: pointer;
      margin-top: 10px;
    }
    input[type="submit"]:hover {
      background-color: #45a049;
    }
    .form-group {
      margin-bottom: 15px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>WiFi Configuration</h2>
    <form action="/get" method="get">
      <div class="form-group">
        <label for="ssid">SSID:</label>
        <input type="text" id="ssid" name="ssid" required>
      </div>
      <div class="form-group">
        <label for="password">Password:</label>
        <input type="password" id="password" name="password" required>
      </div>
      <input type="submit" value="Save">
    </form>
    <form action="/reset" method="get">
      <input type="submit" value="Reset WiFi Configuration" style="background-color: #f44336;">
    </form>
  </div>
</body>
</html>
)rawliteral";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// ฟังก์ชันบันทึกข้อมูล Wi-Fi ไปยัง SPIFFS
void saveWiFiConfig(String ssid, String password) {
  File file = SPIFFS.open("/wifi_config.txt", FILE_WRITE);
  if (file) {
    file.println(ssid);
    file.println(password);
    file.close();
    Serial.println("WiFi configuration saved.");
  } else {
    Serial.println("Failed to save WiFi configuration.");
  }
}

// ฟังก์ชันโหลดข้อมูล Wi-Fi จาก SPIFFS
void loadWiFiConfig(String &ssid, String &password) {
  File file = SPIFFS.open("/wifi_config.txt", FILE_READ);
  if (file) {
    ssid = file.readStringUntil('\n');
    password = file.readStringUntil('\n');
    ssid.trim();
    password.trim();
    file.close();
    Serial.println("WiFi configuration loaded.");
  } else {
    Serial.println("Failed to load WiFi configuration.");
  }
}

// ฟังก์ชันลบข้อมูล Wi-Fi จาก SPIFFS
void resetWiFiConfig() {
  SPIFFS.remove("/wifi_config.txt");
  Serial.println("WiFi configuration reset.");
  ESP.restart(); // restart ESP32
}

void callback(char* mqtt_topic_sub, byte* message, unsigned int length) {
  String messageTemp;
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }
  
  Serial.println(messageTemp);

  if (messageTemp.length() > 0) {
    if (state == 1) {
      Serial1.println(messageTemp);
      
      // ใช้ timeout ในการรอข้อมูลจาก Serial1
      unsigned long startMillis = millis();
      while (millis() - startMillis < 2000) {  // Timeout 1 วินาที
        if (Serial1.available() > 0) {
          String received1 = Serial1.readString();
          Serial.print("Received from TTL: ");
          Serial.println(received1);
          client.publish(mqtt_topic_pub, received1.c_str());
          return;  // ออกจาก callback หลังจากส่งข้อมูล
        }
        delay(10);  // Delay เล็กน้อยเพื่อหลีกเลี่ยง busy waiting
      }
      Serial.println("Serial not connected or no data available.");
       for (int i = 0; i < 5; i++) {  // วนลูปเพื่อกระพริบ LED 5 ครั้ง
        digitalWrite(LED_TTL, HIGH);  // เปิด LED
        delay(100);  // รอ 500 มิลลิวินาที
        digitalWrite(LED_TTL, LOW);  // ปิด LED
        delay(100);  // รอ 500 มิลลิวินาที
        }

    } else if (state == 2) {
      Serial2.println(messageTemp);

      // ใช้ timeout ในการรอข้อมูลจาก Serial2
      unsigned long startMillis = millis();
      while (millis() - startMillis < 2000) {  // Timeout 1 วินาที
        if (Serial2.available() > 0) {// && Serial2.readString() != "" && Serial2.readString() != NULL && Serial2.readString() != "\r" && Serial2.readString() != "\n") {
          String received2 = Serial2.readString();
          Serial.print("Received from RS-232: ");
          Serial.println(received2);
          client.publish(mqtt_topic_pub, received2.c_str());
          return;  // ออกจาก callback หลังจากส่งข้อมูล
        }
        delay(10);  // Delay เล็กน้อยเพื่อหลีกเลี่ยง busy waiting
      }
      Serial.println("Serial not connected or no data available.");
      for (int i = 0; i < 5; i++) {  // วนลูปเพื่อกระพริบ LED 5 ครั้ง
        digitalWrite(LED_RS, HIGH);  // เปิด LED
        delay(100);  // รอ 500 มิลลิวินาที
        digitalWrite(LED_RS, LOW);  // ปิด LED
        delay(100);  // รอ 500 มิลลิวินาที
        }
      delay(10);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  //while (!client.connected()) {
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_sub); // Subscribe to the topic
      Res_Cnt=0;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    //delay(5000);
    // if(++Res_Cnt>5) {
    // ESP.restart();
    // }
  }
 
  return;
}

//LED status แสดงไฟว่ามีการเชื่อมต่อ wifi หรือไม่
void wifi_blink(void* pvParameter){
  while(true){
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_wifi, 1);
      delay(300);
      digitalWrite(LED_wifi, 0);
      delay(300);
      digitalWrite(LED_TTL, 0);
      digitalWrite(LED_RS, 0);
    }
    else{
      digitalWrite(LED_wifi, 1);
    }
    vTaskDelay(100/ portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
  

}

//SW_reset กดเพื่อรีเซ็ตบอร์ด
void switch_reset(void* pvParameter){
  while(true){
    int switch_rst = digitalRead(buttonPin2);

    if(switch_rst == HIGH) {
      //Serial.println(switch_rst);
      Serial.println("Switch pressed: Restarting ESP32...");
      delay(1000);
      //resetWiFiConfig();
      ESP.restart(); 
    }
    vTaskDelay(100/ portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void wifi_setup(){
  // ถ้าไม่มีข้อมูล Wi-Fi ที่บันทึกไว้ หรือเชื่อมต่อไม่สำเร็จ
  if (WiFi.status() != WL_CONNECTED) {
    // ตั้งค่า ESP32 เป็น Access Point
    bool result = WiFi.softAP(apSSID, apPassword);
    if(result) {
      Serial.println("Access Point Started");
    } else {
      Serial.println("Access Point Failed");
    }

    // ตรวจสอบที่อยู่ IP ของ Access Point
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    String apIpAddress = "AP IP address: " + IP.toString();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(LED_TTL, OUTPUT);
  pinMode(LED_RS, OUTPUT);
  pinMode(LED_wifi, OUTPUT);
  pinMode(LED_power, OUTPUT);
  digitalWrite(LED_power, HIGH);

  // Start Serial for communication with other devices
  Serial1.begin(9600, SERIAL_8N1, RXD, TXD);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  espClient.setCACert(root_ca);

  Serial.println("ESP32 Initialized");
  // Serial1.println("Serial1 Initialized");

    // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
 // โหลดข้อมูล Wi-Fi ที่บันทึกไว้
  String ssid, password;
  loadWiFiConfig(ssid, password);

  // พยายามเชื่อมต่อ Wi-Fi หากมีข้อมูลบันทึกไว้
  if (ssid.length() > 0 && password.length() > 0) {
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to WiFi");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to WiFi");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      String ipAddress = "Connected to WiFi\nIP:" + WiFi.localIP().toString();
    } else {
      Serial.println("Failed to connect to WiFi, starting AP mode...");
    }
  }
  wifi_setup();
  // // ถ้าไม่มีข้อมูล Wi-Fi ที่บันทึกไว้ หรือเชื่อมต่อไม่สำเร็จ
  // if (WiFi.status() != WL_CONNECTED) {
  //   // ตั้งค่า ESP32 เป็น Access Point
  //   bool result = WiFi.softAP(apSSID, apPassword);
  //   if(result) {
  //     Serial.println("Access Point Started");
  //   } else {
  //     Serial.println("Access Point Failed");
  //   }

  //   // ตรวจสอบที่อยู่ IP ของ Access Point
  //   IPAddress IP = WiFi.softAPIP();
  //   Serial.print("AP IP address: ");
  //   Serial.println(IP);
  //   String apIpAddress = "AP IP address: " + IP.toString();
  // }

  // กำหนดรูท URL และแสดงฟอร์ม HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", htmlForm);
  });

  // รับข้อมูล SSID และรหัสผ่านจากฟอร์ม
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    String ssid = request->getParam("ssid")->value();
    String password = request->getParam("password")->value();
    saveWiFiConfig(ssid, password);
    request->send(200, "text/html", "WiFi configuration saved. Please restart the ESP32.");
    ESP.restart();
  });

  // รีเซ็ตข้อมูล Wi-Fi (Reset in the webpage)
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request){
    resetWiFiConfig();
    request->send(200, "text/html", "WiFi configuration reset. ESP32 is restarting...");
  });

  // เริ่มต้นเว็บเซิร์ฟเวอร์
  server.begin();

  // setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.subscribe(mqtt_topic_sub);
  //reconnect(); // Start the MQTT connection

  xTaskCreatePinnedToCore(
    wifi_blink,          // Function that implements the task
    "wifi_blink",        // Task name
    2048,                // Stack size (in words) allocated for the task
    NULL,                // Parameter passed to the task
    1,                   // Task priority
    NULL,                // Task handle
    0                    // Core to which the task is pinned (0 or 1)
  );

  xTaskCreatePinnedToCore(
    switch_reset,        // Function that implements the task
    "switch_port",       // Task names
    2048,                // Stack size (in words) allocated for the task
    NULL,                // Parameter passed to the task
    1,                   // Task priority
    NULL,                // Task handle
    0                    // Core to which the task is pinned (0 or 1)
  );
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  switchState = digitalRead(buttonPin);
  // ตรวจสอบว่ามีการเปลี่ยนแปลงสถานะของสวิทช์หรือไม่
  if (switchState == HIGH && lastSwitchState == LOW) {
    // ถ้ามีการกดสวิทช์
    ledState++;
    if (ledState > 1) {
      ledState = 0;
    }
  }

  // Update status switch
  lastSwitchState = switchState;

  // Case status LED
  switch (ledState) {
    case 0:
      digitalWrite(LED_TTL, HIGH);
      digitalWrite(LED_RS, LOW);
      state = 1;
      break;
    case 1:
      digitalWrite(LED_TTL, LOW);
      digitalWrite(LED_RS, HIGH);
      state = 2;
      break;
  }

    // แสดงข้อความเมื่อ state เปลี่ยน
  if (state != previousState) {
    if (state == 1) {
      Serial.println("Switching to TTL");
    } else if (state == 2) {
      Serial.println("Switching to RS-232");
    }
    previousState = state; // อัพเดต previousState
  }

  // Check if reset button is pressed to restart ESP32
  //switch_rst = digitalRead(buttonPin2);

  // if(switch_rst == HIGH) {
  //   Serial.println("Switch pressed: Restarting ESP32...");
  //   delay(1000); // Optional: Delay to debounce or allow time to notice the action
  //   resetWiFiConfig();
  //   ESP.restart(); // Restart ESP32
  // }

  delay(10);
  }
