#include <WiFi.h>
#include <ESP_Mail_Client.h>

const char* ssid = "Mak";
const char* password = "monish123";

WiFiServer server(80);

/* ===== EMAIL CONFIG ===== */
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "esp32.email.sending@gmail.com"
#define AUTHOR_PASSWORD "hzpiudanuiuxfxdk"
#define RECIPIENT_EMAIL "monishanandkale@gmail.com"

bool mailSent = false;

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
  message.sender.name = "ESP32 LOCAL SENDER";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "ESP32 OFF Button Pressed";
  message.addRecipient("User", RECIPIENT_EMAIL);
  message.text.content = "ALERT! POWER SUPPLY HAS BEEN DISCONNECTED";

  if (smtp.connect(&config)) {
    MailClient.sendMail(&smtp, &message);
    Serial.println("✅ Email sent");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (!client) return;

  String header = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      header += c;

      if (c == '\n') {
        if (header.indexOf("GET /?pin=1") >= 0) {
        Serial.println("🔔 ON detected");
        mailSent = false;   // prepare for OFF
        }

        if (header.indexOf("GET /?pin=0") >= 0 && !mailSent) {
        Serial.println("🔕 OFF detected");
        sendEmail();        // 📧 EMAIL SENT ONLY HERE
        mailSent = true;   // prevent spam
        }

        client.println("HTTP/1.1 200 OK");
        client.println("Connection: close");
        client.println();
        break;
      }
    }
  }
  client.stop();
}
