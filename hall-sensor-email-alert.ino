#include <WiFi.h>
#include <ESP_Mail_Client.h>

/* ===== WIFI CONFIG ===== */
const char* ssid = "Anujs25";
const char* password = "1234567890";

/* ===== HALL SENSOR ===== */
const int hallPin = 12;   // GPIO4

/* ===== EMAIL CONFIG ===== */
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "esp32.email.sending@gmail.com"
#define AUTHOR_PASSWORD "hzpiudanuiuxfxdk"
#define RECIPIENT_EMAIL "monishanandkale@gmail.com"

/* ===== FLAG ===== */
bool mailSent = false;

/* ===== SEND EMAIL FUNCTION ===== */
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
  message.sender.name = "ESP32 SENSOR ALERT";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "MAGNET DETECTED";
  message.addRecipient("User", RECIPIENT_EMAIL);
  message.text.content = "✅ Magnet detected by Hall sensor (HIGH)";

  if (smtp.connect(&config)) {
    if (MailClient.sendMail(&smtp, &message)) {
      Serial.println("✅ Email sent");
    } else {
      Serial.println("❌ Email failed");
    }
  }
}

/* ===== SETUP ===== */
void setup() {
  Serial.begin(115200);
  pinMode(hallPin, INPUT_PULLUP);

  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
  Serial.println(WiFi.localIP());
  Serial.println("ESP32 Hall Sensor Monitor Started");
}

/* ===== LOOP ===== */
void loop() {
  int state = digitalRead(hallPin);

  if (state == HIGH) {
    Serial.println("🔴 HIGH - Magnet not detected");
    mailSent = false;
  }

  if (state == LOW && !mailSent) {
    Serial.println("🟢 LOW - Magnet detected");
    sendEmail();
    mailSent = true;  
  }

  delay(500);
}

