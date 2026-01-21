/*
  UNO R4 WiFi - GPS -> serveur POST JSON (avec WiFiS3)
  Librairies requises :
    - TinyGPSPlus
    - WiFiS3 (fourni par le paquet de cartes UNO R4)
    - ArduinoHttpClient

  Configure :
    - WIFI_SSID, WIFI_PASS
    - SERVER_HOST (IP ou nom), SERVER_PORT (ex: 3000), SERVER_PATH (ex: "/api/report")
    - Vitesse du GPS (GPS_BAUD)
*/

#include <TinyGPSPlus.h>
#include <WiFiS3.h>           // <-- utiliser WiFiS3 pour UNO R4 WiFi
#include <ArduinoHttpClient.h>

const char* WIFI_SSID = "YOUR_SSID";
const char* WIFI_PASS = "YOUR_PASSWORD";

const char* SERVER_HOST = "ip"; // adresse du serveur (IP ou nom)
const uint16_t SERVER_PORT = 3000;
const char* SERVER_PATH = "/api/report";

const unsigned long SEND_INTERVAL_MS = 60UL * 1000UL; // envoi périodique (ms)
const uint32_t GPS_BAUD = 9600;

// GPS sur Serial1 (UNO R4 WiFi) : connecte le TX du module GPS au RX1, RX au TX1
TinyGPSPlus gps;
HardwareSerial GPSSerial(1); // Serial1

WiFiClient wifiClient;
HttpClient httpClient(wifiClient, SERVER_HOST, SERVER_PORT);

double lastLat = 0.0;
double lastLon = 0.0;
unsigned long lastSendMs = 0;

String macToString(const byte mac[6]) {
  char buf[18];
  sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

String isoTimestampNow() {
  // Pas d'horloge RTC par défaut — renvoie millis() si pas d'heure NTP
  unsigned long ms = millis();
  char buf[32];
  snprintf(buf, sizeof(buf), "ms:%lu", ms);
  return String(buf);
}

void connectWiFi() {
  Serial.print("Connexion au Wi‑Fi ");
  Serial.print(WIFI_SSID);
  Serial.print(" ... ");
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Pas de module WiFi détecté !");
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("déjà connecté.");
    return;
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi‑Fi connecté !");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    byte mac[6];
    if (WiFi.macAddress(mac)) {
      Serial.print("MAC: ");
      Serial.println(macToString(mac));
    } else {
      Serial.println("Impossible de lire MAC");
    }

    // Envoi immédiat au moment de la connexion
    sendLocationOnce();
  } else {
    Serial.println("\nEchec connexion Wi‑Fi.");
  }
}

void sendLocationOnce() {
  if (lastLat == 0.0 && lastLon == 0.0) {
    Serial.println("Pas de fix GPS valide — envoi ignoré.");
    return;
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Non connecté au Wi‑Fi — envoi impossible.");
    return;
  }

  byte mac[6];
  String macStr = "unknown";
  if (WiFi.macAddress(mac)) {
    macStr = macToString(mac);
  }

  String json = "{";
  json += "\"mac\":\"" + macStr + "\",";
  json += "\"lat\":" + String(lastLat, 6) + ",";
  json += "\"lon\":" + String(lastLon, 6) + ",";
  json += "\"sent_at\":\"" + isoTimestampNow() + "\",";
  json += "\"ssid\":\"" + String(WIFI_SSID) + "\"";
  json += "}";

  Serial.print("Envoi POST -> ");
  Serial.print(SERVER_HOST);
  Serial.print(":");
  Serial.print(SERVER_PORT);
  Serial.print(SERVER_PATH);
  Serial.print("  body=");
  Serial.println(json);

  int statusCode = httpClient.post(SERVER_PATH, "application/json", json);
  if (statusCode > 0) {
    String response = httpClient.getString();
    Serial.print("HTTP ");
    Serial.print(statusCode);
    Serial.print(" response: ");
    Serial.println(response);
  } else {
    Serial.print("Erreur HTTPClient.post(): ");
    Serial.println(statusCode);
  }

  lastSendMs = millis();
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println("UNO R4 WiFi - GPS tracker (WiFiS3)");

  GPSSerial.begin(GPS_BAUD);
  delay(200);

  connectWiFi();
}

void loop() {
  while (GPSSerial.available()) {
    char c = (char)GPSSerial.read();
    gps.encode(c);
    if (gps.location.isUpdated() && gps.location.isValid()) {
      lastLat = gps.location.lat();
      lastLon = gps.location.lng();
      Serial.print("Fix GPS: ");
      Serial.print(lastLat, 6);
      Serial.print(", ");
      Serial.println(lastLon, 6);
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  } else {
    if (millis() - lastSendMs >= SEND_INTERVAL_MS) {
      sendLocationOnce();
    }
  }

  delay(100);
}
