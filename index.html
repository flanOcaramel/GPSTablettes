<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Suivi Arduino GPS</title>

  <!-- Leaflet -->
  <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css" />
  <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>

  <style>
    #map { height: 100vh; }
  </style>
</head>
<body>

<div id="map"></div>

<script>
  // Carte
  const map = L.map('map').setView([48.6921, 6.1844], 13);
  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '© OpenStreetMap'
  }).addTo(map);

  // Marqueur
  const marker = L.marker([48.6921, 6.1844]).addTo(map);

  // Rafraîchissement GPS
  setInterval(() => {
    fetch('position.json?' + Date.now())
      .then(r => r.json())
      .then(p => {
        if (p.lat && p.lng) {
          marker.setLatLng([p.lat, p.lng]);
          map.panTo([p.lat, p.lng]);
        }
      });
  }, 3000);
</script>

</body>
</html>
