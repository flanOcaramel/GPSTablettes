let map;
const markers = {}; // key: mac -> L.Marker
const deviceList = document.getElementById("devices");

function fmtRow(row) {
  return `
    <strong>${row.mac}</strong><br/>
    lat: ${row.lat.toFixed(6)} lon: ${row.lon.toFixed(6)}<br/>
    envoyé: ${row.sent_at || "—"}<br/>
    reçu: ${row.received_at}<br/>
    ssid: ${row.ssid || "—"}
  `;
}

function addOrUpdate(row) {
  const pos = [row.lat, row.lon];
  if (markers[row.mac]) {
    markers[row.mac].setLatLng(pos);
    markers[row.mac].setPopupContent(fmtRow(row));
  } else {
    const marker = L.marker(pos).addTo(map);
    marker.bindPopup(fmtRow(row));
    markers[row.mac] = marker;
  }

  // Update sidebar
  let li = document.getElementById("dev-" + row.mac.replace(/[:.]/g, "_"));
  if (!li) {
    li = document.createElement("li");
    li.id = "dev-" + row.mac.replace(/[:.]/g, "_");
    deviceList.appendChild(li);
    li.addEventListener("click", () => {
      map.setView(pos, 24);
      markers[row.mac].openPopup();
    });
  }
  li.innerHTML = fmtRow(row);
}

async function loadExisting() {
  const resp = await fetch("/api/locations");
  const rows = await resp.json();
  if (!rows || !rows.length) return;
  rows.forEach((r) => addOrUpdate(r));
  // center map to first device
  map.setView([rows[0].lat, rows[0].lon], 18);
}

function initMap() {
  map = L.map("map").setView([48.6924, 6.1844], 19);

  // Add OpenStreetMap tiles
  L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    attribution: "© OpenStreetMap contributors",
    maxZoom: 19,
  }).addTo(map);

  loadExisting();

  const socket = io();
  socket.on("new-location", (row) => {
    addOrUpdate(row);
  });
}

// Initialize map when DOM is loaded
document.addEventListener("DOMContentLoaded", initMap);
