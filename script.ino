#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <TinyGPSPlus.h>

const char* ssid = "TON_WIFI";
const char* password = "MOT_DE_PASSE";

const char* server = "ton-serveur.com"; // sans http://
const int port = 80;
const char* endpoint = "/position.php";

TinyGPSPlus gps;
WiFiClient wifi;
HttpClient client(wifi, server, port);

String macStr;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600); // GPS
  while (!Serial);

  // Récupération MAC
  byte mac[6];
  WiFi.macAddress(mac);
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 16) macStr += "0";
    macStr += String(mac[i], HEX);
    if (i < 5) macStr += ":";
  }

  // Connexion WiFi
  Serial.print("Connexion WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
