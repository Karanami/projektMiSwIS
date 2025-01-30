// api routes 

var apiTestRoute                = "/api/test";
var apiGetTemperatureRoute      = "/api/get/temperature";
var apiGetContaminationRoute    = "/api/get/contamination";
var apiSetWantTemp              = "/api/set/want_temp"

// config

var pollDataInterval = 1000;

var temperature = 0.0;

// other vars

// utilities

function httpPost(resource, data)
{
    if(resource == null) { console.log("httpPost: resource is null"); return; }
    if(data == null) { console.log("httpPost: data is null"); return; }

    var xmlHttp = new XMLHttpRequest();
    xmlHttp.open("POST", resource);
    xmlHttp.setRequestHeader("Content-Type", "application/json; charset=UTF-8");
    const body = JSON.stringify(data);
    xmlHttp.onload = () => {
        if (xmlHttp.readyState == 4 && xmlHttp.status == 201) {
          console.log(JSON.parse(xmlHttp.responseText));
        } else {
          console.log(`Error: ${xmlHttp.status}`);
        }
      };
    xmlHttp.send(body);
}

function httpGetAsync(resource, callback)
{
    if(resource == null) { console.log("httpGetAsync: resource is null"); return; }
    if(callback == null) { console.log("httpGetAsync: callback is null"); return; }
    
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() 
    { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
        {
            callback(xmlHttp.responseText);
        }
    }
    xmlHttp.open("GET", resource, true); // true for asynchronous 
    xmlHttp.send(null);
}

function pollData() 
{
    // for now just send req every sec or so~ 
    httpGetAsync(apiGetTemperatureRoute, apiGetTemperature);
    httpGetAsync(apiGetContaminationRoute, apiGetContamination);

    httpPost(apiSetWantTemp, [30.0]);

    console.log("pollData: data requested");
}

// api callbacks

function apiTestCallback(responseText)
{
    console.log("/api/test: got a response!: \"" + responseText + "\"");

    const jsonObj = JSON.parse(responseText);

    if(jsonObj == null) { console.log("/api/test: respone is empty!"); return; }
    if(jsonObj.server_version == null) { console.log("/api/test: invalid response!"); return; }
    if(jsonObj.server_version != "v1.0") { console.log("/api/test: wrong version"); return; }

    console.log("/api/test: response ok");

    return true;
}

function apiGetTemperature(responseText) 
{
    const jsonObj = JSON.parse(responseText);

    if(jsonObj == null) { console.log("/api/get/temperature: respone is empty!"); return; }
    if(jsonObj.temperature == null) { console.log("/api/get/temperature: invalid response!"); return; }

    console.log("/api/get/temperature: got: " + jsonObj.temperature);

    temperature = jsonObj.temperature;

    //TODO add some thingy and slap this data in it
}

function apiGetContamination(responseText) 
{
    const jsonObj = JSON.parse(responseText);

    if(jsonObj == null) { console.log("/api/get/contamination: respone is empty!"); return; }
    if(jsonObj.contamination == null) { console.log("/api/get/contamination: invalid response!"); return; }

    console.log("/api/get/contamination: got: " + jsonObj.contamination);

    //TODO add some thingy and slap this data in it  
}

function onButtonClick() 
{
    console.log("i was cliked");
    httpGetAsync(apiTestRoute, apiTestCallback);   
}

// set up

httpGetAsync(apiTestRoute, (param) => 
{
    if(apiTestCallback(param) == true)
    {
        console.log("Staring data polling...");
        setInterval(pollData, pollDataInterval);
    }
    else
    {
        console.log("Server verification failed!");
    }
})

//const labels = [0.1, 0.2, 0.3, 0.4, 0.5];
//const data = [10, 20, 15, 25, 30, 22];

function getCurrentTime() {
    const now = new Date();
    return now.toLocaleTimeString();
}

// Funkcja do aktualizacji wykresu
function updateChart() {
    // Pobranie nowego czasu
    if(temperature == 0.0) return;
    const currentTime = getCurrentTime();
    
    // Dodanie nowego punktu do wykresu
    myChart.data.labels.push(currentTime);
    myChart.data.datasets[0].data.push(temperature); // Losowe dane

    // Utrzymywanie stałej liczby punktów (np. 10 ostatnich)
    if (myChart.data.labels.length > 20) {
        myChart.data.labels.shift(); // Usuwanie najstarszego elementu
        myChart.data.datasets[0].data.shift();
    }

    myChart.update();
}

// Aktualizacja co sekundę
setInterval(updateChart, 1000);