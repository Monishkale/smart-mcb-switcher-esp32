#include <WiFi.h>
#include <ESP_Mail_Client.h>
#include <ESP32Servo.h>

/* ===== WIFI CONFIG ===== */
const char* ssid = "internet";
const char* password = "internet pass";

/* ===== WEB SERVER ===== */
WiFiServer server(80);

/* ===== HALL SENSOR ===== */
const int hallPin = 12;   // GPIO4
bool mailSent = false;

/* ===== SERVO CONFIG ===== */
Servo myServo;
const int servoPin = 13;
bool movementDone = true;

/* ===== EMAIL CONFIG ===== */
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "esp32mailg@gmail.com"
#define AUTHOR_PASSWORD "mail pass"
#define RECIPIENT_EMAIL "personalmail@gmail.com"

/* ===== SEND EMAIL ===== */
void sendEmail() {
  SMTPSession smtp;
  Session_Config config;

  config.server.host_name = SMTP_HOST;
  config.server.port = SMTP_PORT;
  config.login.email = AUTHOR_EMAIL;
  config.login.password = AUTHOR_PASSWORD;
  config.time.ntp_server = F("pool.ntp.org");
  config.time.gmt_offset = 5.5 * 3600;

  SMTP_Message message;
  message.sender.name = "MCB SENSOR ALERT";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "MAGNET DETECTED";
  message.addRecipient("User", RECIPIENT_EMAIL);
  message.text.content = "✅ MCB HAS BEEN TRIPPED";

  if (smtp.connect(&config)) {
    if (MailClient.sendMail(&smtp, &message)) {
      Serial.println("✅ Email sent");
    } else {
      Serial.println("❌ Email failed");
    }
  }
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
  Serial.println("✅ Servo cycle 1 complete");
}

void moveServo0() {
  movementDone = false;
  Serial.println("🟢 Servo: 60° → 0° → 60°");

  myServo.write(0);
  delay(500);

  myServo.write(60);
  delay(500);

  movementDone = true;
  Serial.println("✅ Servo cycle 2 complete");
}

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);

  pinMode(hallPin, INPUT_PULLUP);

  myServo.attach(servoPin, 500, 2500);
  myServo.write(60);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected");
  Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("🌐 HTTP server started");
  Serial.println("🧲 Hall sensor + 🌍 Servo ready");
}

/* ===== LOOP ===== */
void loop() {

  /* ---- HALL SENSOR LOGIC ---- */
  int hallState = digitalRead(hallPin);

  if (hallState == HIGH) {
    Serial.println("🟢 HIGH - Magnet detected");
    mailSent = false;
  }

  if (hallState == LOW && !mailSent) {
    Serial.println("🔴 LOW - Magnet not detected");
    sendEmail();
    mailSent = true;
  }

  /* ---- WEB SERVO CONTROL ---- */
  WiFiClient client = server.available();
  if (client) {
    Serial.println("⚡ Web client connected");

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
    Serial.println("🔌 Web client disconnected");
  }

  delay(200);
}
