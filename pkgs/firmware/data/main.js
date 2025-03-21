// Establish WebSocket connection
const socket = new WebSocket("ws://" + window.location.hostname + "/ws");

// When the WebSocket connection opens
socket.onopen = function () {
  console.log("Connected to WebSocket server");
};

// When receiving data from the WebSocket
socket.onmessage = function (event) {
  const data = JSON.parse(event.data); // Parse the incoming data

  // Update the UI with the sensor readings
  document.getElementById("co2").innerText = data.co2;
  document.getElementById("temperature").innerText =
    data.temperature.toFixed(1);
  document.getElementById("humidity").innerText = data.humidity.toFixed(0);

  // Optional: Log the values to the console
  console.log(
    "CO2: " +
      data.co2 +
      ", Temp: " +
      data.temperature +
      "°C, Humidity: " +
      data.humidity +
      "%"
  );
};

// When an error occurs
socket.onerror = function (error) {
  console.log("WebSocket Error: " + error);
};

// When the WebSocket connection is closed
socket.onclose = function () {
  console.log("WebSocket connection closed");
};

// Optional: Send a message to the server to get initial data
// socket.send('getReadings');
