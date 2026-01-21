const express = require("express");
const http = require("http");
const sqlite3 = require("sqlite3").verbose();
const path = require("path");
const { Server } = require("socket.io");
const cors = require("cors");

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));
app.use("/css", express.static(path.join(__dirname, "public/css")));
app.use("/js", express.static(path.join(__dirname, "public/js")));
app.use("/assets", express.static(path.join(__dirname, "public/assets")));

// Database
const db = new sqlite3.Database(path.join(__dirname, "data.db"));
db.serialize(() => {
  db.run(`CREATE TABLE IF NOT EXISTS locations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    mac TEXT,
    lat REAL,
    lon REAL,
    sent_at TEXT,
    received_at TEXT,
    ssid TEXT
  )`);
});

// API: receive a report from device
app.post("/api/report", (req, res) => {
  const { mac, lat, lon, sent_at, ssid } = req.body || {};
  if (!mac || typeof lat !== "number" || typeof lon !== "number") {
    return res
      .status(400)
      .json({ error: "mac, lat (number) and lon (number) are required" });
  }
  const received_at = new Date().toISOString();
  const stmt = db.prepare(
    `INSERT INTO locations (mac, lat, lon, sent_at, received_at, ssid) VALUES (?, ?, ?, ?, ?, ?)`,
  );
  stmt.run(
    mac,
    lat,
    lon,
    sent_at || null,
    received_at,
    ssid || null,
    function (err) {
      if (err) {
        console.error(err);
        return res.status(500).json({ error: "db error" });
      }
      const id = this.lastID;
      db.get("SELECT * FROM locations WHERE id = ?", [id], (err2, row) => {
        if (err2) {
          console.error(err2);
        }
        // Emit new location to websocket clients
        io.emit("new-location", row);
        res.json({ ok: true, id, row });
      });
    },
  );
  stmt.finalize();
});

// API: get latest location per device (one point per MAC)
app.get("/api/locations", (req, res) => {
  const q = `
    SELECT l.* FROM locations l
    INNER JOIN (
      SELECT mac, MAX(id) as maxid FROM locations GROUP BY mac
    ) m ON l.mac = m.mac AND l.id = m.maxid
  `;
  db.all(q, [], (err, rows) => {
    if (err) {
      console.error(err);
      return res.status(500).json({ error: "db error" });
    }
    res.json(rows);
  });
});

// Serve index.html at root
app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "public", "index.html"));
});

// Start server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Server listening on http://localhost:${PORT}`);
});
