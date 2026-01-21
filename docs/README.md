# Arduino Tracker (UNO R4 WiFi - GPS → OpenStreetMap)

Ce projet contient :
- un serveur Node.js (Express + SQLite + Socket.IO) pour recevoir et stocker des rapports depuis les UNO R4 WiFi,
- une page web affichant les positions sur OpenStreetMap avec Leaflet.js (un marqueur par adresse MAC),
- un sketch Arduino pour UNO R4 WiFi (TinyGPSPlus + WiFiNINA + ArduinoHttpClient) qui envoie { mac, lat, lon, sent_at, ssid } au serveur.

Arborescence recommandée :
```
arduino-tracker/
├─ server.js
├─ package.json
├─ data.db              (généré automatiquement)
├─ create_zip.sh
├─ create_zip.ps1
├─ public/
│  ├─ index.html
│  ├─ main.js
│  └─ style.css
└─ Arduino/
   └─ uno_r4_tracker.ino
```

IMPORTANT — Ce README suppose que ta machine **n'a que l'IDE** (ou rien) : voici toutes les étapes pour rendre le projet fonctionnel.

1) Installer Node.js et npm
- Windows / macOS / Linux : télécharge et installe Node.js (version LTS recommandée, >=18) depuis https://nodejs.org/
- Vérifie :
  - node -v
  - npm -v

2) Installer les dépendances du serveur
- Ouvre un terminal dans le dossier `arduino-tracker` puis :
  - npm install
- Cela installera express, sqlite3, socket.io, cors.

3) Lancer le serveur en local
- npm start
- Par défaut le serveur écoute sur http://localhost:3000
- Endpoint utiles :
  - POST /api/report — recevoir les rapports JSON depuis les UNO
  - GET /api/locations — renvoie la dernière position par MAC

4) OpenStreetMap avec Leaflet.js
- Ce projet utilise OpenStreetMap avec Leaflet.js, **aucune clé API n'est requise**.
- Les cartes sont gratuites et open source.
- Leaflet.js est chargé depuis le CDN officiel dans `public/index.html`.

5) Arduino IDE et dépendances (UNO R4 WiFi)
- Installe l’IDE Arduino :
  - Arduino IDE 2.x recommandé : https://www.arduino.cc/en/software
- Ouvre l'IDE et installe via le Library Manager (Sketch > Include Library > Manage Libraries...):
  - TinyGPSPlus (TinyGPS++)
  - WiFiNINA
  - ArduinoHttpClient
- Board support :
  - Dans l'IDE, sélectionne la carte `Arduino UNO R4 WiFi` dans Tools > Board. Si elle n'apparait pas, mets à jour l'IDE ou installe le gestionnaire de cartes correspondant (consulte la documentation Arduino pour UNO R4).
- Wiring GPS → UNO R4 WiFi :
  - GPS TX → RX1 (Serial1 RX)
  - GPS RX → TX1 (Serial1 TX)
  - Alimentation 3.3V/5V selon module

6) Configuration du sketch UNO R4
- Ouvre `Arduino/uno_r4_tracker.ino` dans l'IDE Arduino.
- Modifie :
  - WIFI_SSID et WIFI_PASS
  - SERVER_HOST (IP ou nom du serveur, ex: 192.168.1.100 si serveur local sur le même réseau)
  - SERVER_PORT (par défaut 3000)
  - SERVER_PATH (par défaut /api/report)
- Flashe la carte.

7) Test sans Arduino
- Tu peux simuler un appareil avec curl :
  curl -X POST http://localhost:3000/api/report -H "Content-Type: application/json" -d '{"mac":"AA:BB:CC:DD:EE:FF","lat":48.8566,"lon":2.3522,"sent_at":"2026-01-21T12:00:00Z","ssid":"Test"}'

8) Création du .zip (plusieurs méthodes)
- Linux / macOS (si `zip` installé) :
  - ./create_zip.sh
- Windows PowerShell (script fourni) :
  - powershell -ExecutionPolicy Bypass -File create_zip.ps1
- Manuel :
  - Sélectionne le dossier `arduino-tracker` et compresse en .zip via l'explorateur / Finder.
- Le .zip généré contient tout le projet (sauf data.db ignoré si vide).

9) Sécurité & production (bons points à suivre)
- Ajouter une authentification (API key / HMAC) côté serveur pour éviter injections de fausses positions.
- Passer en HTTPS pour le serveur en production.
- Ajouter rate limiting / validation stricte des données.
- Remplacer SQLite par une base adaptée si tu stockes beaucoup de données.

10) Commandes utiles résumé
- Installer dépendances Node : npm install
- Lancer serveur : npm start
- Créer zip (Linux/mac) : bash create_zip.sh
- Créer zip (Windows PS) : powershell -ExecutionPolicy Bypass -File create_zip.ps1

---

Si tu veux que je génère le .zip pour toi et fournisse un lien téléchargeable, je peux :
- créer un dépôt GitHub et te donner le lien zip, ou
- te donner un fichier .zip ici si tu autorises l'upload (dis‑le moi).

Si tu veux que j'ajoute :
- authentification API simple (clé dans header) + adaptation du sketch,
- envoi via HTTPS (WiFiClientSecure) pour l'UNO R4 WiFi,
- ou Dockerfile pour exécuter le serveur facilement,
dis‑le moi et je l'ajoute.
