async function refreshData(){
  const response = await fetch("/data", {method: "GET"});
  if(!response.ok){
    console.log('error while fetching data');
    return;
  }

  const {co2, humidity, temperature} = await response.json();

  document.getElementById("co2").innerText = co2;
  document.getElementById("temperature").innerText = temperature.toFixed(1);
  document.getElementById("humidity").innerText = humidity.toFixed(0);

  console.log(co2, humidity, temperature);
}

refreshData();
const interval = setInterval(refreshData, 5000);
