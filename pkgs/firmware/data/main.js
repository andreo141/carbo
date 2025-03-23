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

// When websocket is established, call the getReadings() function
function onOpen(event) {
  console.log("Connection opened");
  getReadings();
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
  const myObj = JSON.parse(event.data);
  const values = myObj.data;
    console.log("history data:", values);

    if (values && Array.isArray(values)) {
      updateChart(values); // Pass only the array
    } else {
      console.error("Invalid data format:", myObj);
    }

    const latestValues = values[values.length - 1];
    document.getElementById("co2").innerHTML = latestValues.co2;
    document.getElementById("temperature").innerHTML = latestValues.temp;
    document.getElementById("humidity").innerHTML = latestValues.humidity;
    console.log('latest values:', latestValues);
}

function updateChart(data) {
  const labels = data.map(entry => new Date(entry.timestamp).toLocaleTimeString());
  const co2Values = data.map(entry => entry.co2);

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
      datasets: [
        { label: "CO2", data: co2Values, borderColor: "red" },
      ]
    }
  });
}

