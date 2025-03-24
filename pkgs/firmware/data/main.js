const gateway = `ws://${window.location.hostname}/ws`;
let websocket;
let chartInstance = null;

// Init web socket when the page loads
window.addEventListener("load", onload);

function onload(event) {
  initWebSocket();
}

function getReadings() {
  websocket.send("getReadings");
}

function initWebSocket() {
  console.log("Trying to open a WebSocket connection…");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

// TODO: figure out what to do with this, see main.cpp as well
// When websocket is established, call the getReadings() function
function onOpen(event) {
  console.log("Connection opened");
  // getReadings();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
  const msg = JSON.parse(event.data);
  console.log("received message from server:", msg);

  switch (msg.type) {
    case "history":
      console.log('RECEIVED HISTORYYYYY');
      updateChart(msg.data);
      break;
    case "latest_reading":
      const latestReading = msg.data;
      document.getElementById("co2").innerHTML = latestReading.co2;
      document.getElementById("temperature").innerHTML = latestReading.temperature;
      document.getElementById("humidity").innerHTML = latestReading.humidity;
      break;
  }
}


function updateChart(data) {
  const labels = data.map((entry) =>
    new Date(entry.timestamp).toLocaleTimeString()
  );
  const co2Values = data.map((entry) => entry.co2);

  const ctx = document.getElementById("historyChart").getContext("2d");

  // Destroy existing chart if it exists
  if (chartInstance) {
    chartInstance.destroy();
  }

  // Create a new chart instance
  chartInstance = new Chart(ctx, {
    type: "line",
    data: {
      labels: labels,
      datasets: [{ label: "CO2", data: co2Values, borderColor: "red" }],
    },
  });
}
