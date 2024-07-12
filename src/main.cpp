#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "Max6675.h"
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>


#define RXD 26  // RXD
#define TXD 22  // TXD

int thermoDO = 19;
int thermoCS = 5;
int thermoCLK = 18;

int state = 1;

long last;
long last2;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

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
  ESP.restart(); // รีสตาร์ท ESP32
}

// void setup_wifi() {
//   delay(10);
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

void callback(char* mqtt_topic_sub, byte* message, unsigned int length) {
  // Serial.print("Message arrived on topic: ");
  // Serial.print(topic);
  // Serial.print(". Message: ");
  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  if(messageTemp == ":i"){
    
    // state = 1;
    Serial1.print(":i");
    String receivediTSD = Serial1.readString();
    Serial.print("Received iTSD: ");
    Serial.println(receivediTSD);


  }
  else if(messageTemp == ":dA"){

    // state = 2;
    Serial1.println(":dA");
    String receivediTSD = Serial1.readString();
    Serial.print("Received iTSD: ");
    Serial.println(receivediTSD);
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic_sub); // Subscribe to the topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Start Serial1 for communication with other devices
  Serial1.begin(9600, SERIAL_8N1, RXD, TXD);

  espClient.setCACert(root_ca);

  Serial.println("ESP32 Initialized");
  Serial1.println("Serial1 Initialized");

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
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  
    
  //   if(state == 1){
  //     if(millis()-last > 5000){
  //       last = millis();
  //       Serial1.print(":i");
  //     }
  //   }else if(state == 2){
  //     if(millis() - last > 5000){
  //       last = millis();
  //       Serial1.println(":dA");
  //     }
  //   }

  //   if(millis() - last2 > 100){
  //     last2 = millis();
  //     if(Serial1.available()>0){
  //       // Serial.print("State:");
  //       // Serial.println(state);
  //       String receivediTSD = Serial1.readString();
  //       Serial.print("Received iTSD: ");
  //       Serial.println(receivediTSD);
  //       receivediTSD  = "";
  //     }
      
  // }
    

  // Serial.println("Hello");

   //if (Serial1.available()>0) {
    // Read data from Serial1
     //Serial.println("Hello");
    // String receivedData = Serial1.readString();

    // Serial.print("Received data: ");
    // Serial.println(receivedData);

  //}

  // double celsius = thermocouple.readCelsius();
  // double fahrenheit = thermocouple.readFahrenheit();
  // String celsiusStr = String(celsius, 2);
  // String fahrenheitStr = String(fahrenheit, 2);
  // String payload = "C = " + String(celsius, 2) + " , F = " + String(fahrenheit, 2);

  // Serial.print("C = ");
  // Serial.print(celsius);
  // Serial.print("\tF = ");
  // Serial.println(fahrenheit);
  // client.publish(mqtt_topic_pub, payload.c_str());
    
  delay(1000); 
}
