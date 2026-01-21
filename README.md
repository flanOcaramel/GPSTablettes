# Arduino GPS Tracker

Une application de suivi GPS en temps réel avec Arduino UNO R4 WiFi et une interface web OpenStreetMap.

## Structure du projet

```
arduino-tracker/
├── server.js                 # Serveur principal Node.js
├── package.json              # Dépendances et scripts npm
├── config/                   # Fichiers de configuration
├── src/                      # Code source serveur
│   ├── controllers/          # Contrôleurs API
│   ├── middleware/          # Middleware Express
│   ├── models/              # Modèles de données
│   ├── routes/              # Routes API
│   ├── services/            # Services métier
│   └── utils/               # Utilitaires
├── public/                   # Fichiers statiques web
│   ├── index.html           # Page principale
│   ├── css/                 # Styles
│   │   └── style.css
│   ├── js/                  # JavaScript client
│   │   └── main.js
│   ├── components/          # Composants UI
│   ├── utils/               # Utilitaires client
│   └── assets/              # Images et autres ressources
├── Arduino/                  # Code Arduino
│   ├── sketches/            # Sketchs Arduino
│   │   └── uno_r4_tracker.ino
│   └── libraries/           # Bibliothèques personnalisées
├── docs/                     # Documentation
│   └── README.md            # Documentation détaillée
├── scripts/                  # Scripts de déploiement
└── data.db                  # Base de données SQLite (générée automatiquement)
```

## Installation rapide

Voir la documentation complète dans [docs/README.md](docs/README.md)

### Démarrage rapide

```bash
# Installer les dépendances
npm install

# Lancer le serveur
npm start
```

Le serveur sera disponible sur http://localhost:3000
