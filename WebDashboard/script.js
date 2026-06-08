import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.2/firebase-app.js";
import {
  getDatabase,
  ref,
  onValue,
  query,
  limitToLast
} from "https://www.gstatic.com/firebasejs/10.12.2/firebase-database.js";

// Replace this with your Firebase web config
const firebaseConfig = {
  apiKey: "AIzaSyDfWpfhd8saLRFPt45TunqR1wL41otV7cY",
  databaseURL: "https://eecs113-final-default-rtdb.firebaseio.com/"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

const modeEl = document.getElementById("mode");
const dateTimeEl = document.getElementById("dateTime");
const tempEl = document.getElementById("temperature");
const humidityEl = document.getElementById("humidity");
const lightEl = document.getElementById("lightStatus");
const objectEl = document.getElementById("objectDetected");
const distanceEl = document.getElementById("distance");
const statusCard = document.getElementById("statusCard");
const logsEl = document.getElementById("logs");

const statusRef = ref(database, "smartHome/currentStatus");

onValue(statusRef, (snapshot) => {
  const data = snapshot.val();

  if (!data) {
    modeEl.textContent = "Mode: No Data";
    return;
  }

  modeEl.textContent = `Mode: ${data.mode}`;
  dateTimeEl.textContent = `Date/Time: ${data.dateTime}`;
  tempEl.textContent = `${data.temperatureF} °F`;
  humidityEl.textContent = `${data.humidity}%`;
  lightEl.textContent = data.lightStatus;
  objectEl.textContent = data.objectDetected ? "Detected" : "Not Detected";
  distanceEl.textContent = data.distanceCm >= 0 ? `${data.distanceCm} cm` : "N/A";

  statusCard.className = "status-card";

  if (data.mode === "Home") {
    statusCard.classList.add("status-home");
  } else if (data.mode === "Away") {
    statusCard.classList.add("status-away");
  } else if (data.mode === "Alert") {
    statusCard.classList.add("status-alert");
  }
});

const logsRef = query(ref(database, "smartHome/eventLogs"), limitToLast(10));

onValue(logsRef, (snapshot) => {
  const logs = snapshot.val();

  logsEl.innerHTML = "";

  if (!logs) {
    logsEl.innerHTML = "<p>No logs yet.</p>";
    return;
  }

  const logArray = Object.values(logs).reverse();

  logArray.forEach((log) => {
    const div = document.createElement("div");
    div.className = "log-item";

    div.innerHTML = `
      <strong>${log.eventType}</strong><br>
      Mode: ${log.mode}<br>
      Time: ${log.dateTime}<br>
      Light: ${log.lightStatus}<br>
      Object: ${log.objectDetected ? "Detected" : "Not Detected"}<br>
      Distance: ${log.distanceCm >= 0 ? log.distanceCm + " cm" : "N/A"}
    `;

    logsEl.appendChild(div);
  });
});