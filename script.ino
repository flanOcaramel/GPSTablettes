#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <TinyGPSPlus.h>

const char* ssid = "ssid";
const char* password = "pwd";

const char* server = "localhost"; // ou votre IP serveur
const int port = 80;
const char* endpoint = "/position.php";

TinyGPSPlus gps; // déclaration globale
WiFiClient wifi;
HttpClient client(wifi, server, port);

String macStr;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
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
  Serial.println("Connecté!");
}

void loop() {
  // Alimenter les données GPS au parser
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }

  // Vérifier si une position GPS valide est disponible
  if (gps.location.isValid() && gps.location.isUpdated()) {
    // Préparer les données POST
    String postData = "mac=" + macStr + "&lat=" + String(gps.location.lat(), 6) + "&lng=" + String(gps.location.lng(), 6);

    // Envoyer la requête POST au serveur
    client.beginRequest();
    client.post(endpoint);
    client.sendHeader("Content-Type", "application/x-www-form-urlencoded");
    client.sendHeader("Content-Length", postData.length());
    client.beginBody();
    client.print(postData);
    client.endRequest();

    // Lire la réponse pour déboguer
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    Serial.println("POST Status: " + String(statusCode));
    Serial.println("Response: " + response);

    // Délai pour éviter des mises à jour trop fréquentes (10 secondes)
    delay(1000);
  }

  // Petit délai pour éviter une boucle trop serrée
  delay(100);
}
