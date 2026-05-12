#include <WiFi.h>
#include <ESP32Servo.h>

/* ===== WIFI CONFIG ===== */
const char* ssid = "Anujs25";
const char* password = "1234567890";

/* ===== WEB SERVER ===== */
WiFiServer server(80);

/* ===== SERVO CONFIG ===== */
Servo myServo;
const int servoPin = 13;
bool movementDone = true;

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);

  // Servo setup
  myServo.attach(servoPin, 500, 2500);   // MG996R full PWM range
  myServo.write(60);                     // Default position
  Serial.println("Servo ready at 60°");

  // WiFi setup
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("🌐 HTTP server started");
}

/* ===== SERVO ACTIONS ===== */
void moveServo180() {
  movementDone = false;
  Serial.println("🔵 Servo: 60° → 180° → 60°");

  myServo.write(165);
  delay(500);

  myServo.write(60);
  delay(500);

  movementDone = true;
  Serial.println("✅ Movement 1 complete");
}

void moveServo0() {
  movementDone = false;
  Serial.println("🟢 Servo: 60° → 0° → 60°");

  myServo.write(0);
  delay(500);

  myServo.write(60);
  delay(500);

  movementDone = true;
  Serial.println("✅ Movement 2 complete");
}

/* ===== LOOP ===== */
void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("⚡ Client connected");

  String request = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;
      Serial.write(c);

      if (c == '\n') {

        if (request.indexOf("GET /?cmd=1") >= 0 && movementDone) {
          moveServo180();
        }

        if (request.indexOf("GET /?cmd=2") >= 0 && movementDone) {
          moveServo0();
        }

        client.println("HTTP/1.1 200 OK");
        client.println("Connection: close");
        client.println();
        break;
      }
    }
  }

  client.stop();
  Serial.println("🔌 Client disconnected");
}

