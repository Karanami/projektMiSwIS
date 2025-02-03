// api routes 

var apiTestRoute                = "/api/test";
var apiGetTemperatureRoute      = "/api/get/temperature";
var apiGetContaminationRoute    = "/api/get/contamination";
var apiSetWantDirtRoute         = "/api/set/want_dirt"
var apiGetWantDirtRoute         = "/api/get/want_dirt"
var apiSetWantTempRoute         = "/api/set/want_temp"
var apiGetWantTempRoute         = "/api/get/want_temp"

// config

var pollDataInterval = 1000;

var temperature = 0.0;
var dirt = 0.0;
var want_temp = 0.0;
var want_dirt = 0.0;

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
    httpGetAsync(apiGetWantTempRoute, apiGetWantTemp);
    httpGetAsync(apiGetWantDirtRoute, apiGetWantDirt);

    document.getElementById("textTemp").value = want_temp;
    document.getElementById("textDirt").value = want_dirt;

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

function apiGetWantDirt(responseText) 
{
    const jsonObj = JSON.parse(responseText);

    if(jsonObj == null) { console.log("/api/get/want_dirt: respone is empty!"); return; }
    if(jsonObj.wantDirt == null) { console.log("/api/get/want_dirt: invalid response!"); return; }

    console.log("/api/get/want_dirt: got: " + jsonObj.wantDirt);

    want_dirt = jsonObj.wantDirt;

    //TODO add some thingy and slap this data in it
}

function apiGetWantTemp(responseText) 
{
    const jsonObj = JSON.parse(responseText);

    if(jsonObj == null) { console.log("/api/get/want_temp: respone is empty!"); return; }
    if(jsonObj.wantTemp == null) { console.log("/api/get/want_temp: invalid response!"); return; }

    console.log("/api/get/want_temp: got: " + jsonObj.wantTemp);

    want_temp = jsonObj.wantTemp;

    //TODO add some thingy and slap this data in it
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

    dirt = jsonObj.contamination;

    console.log("/api/get/contamination: got: " + jsonObj.contamination);

    //TODO add some thingy and slap this data in it  
}

function onButtonClick() 
{
    console.log("i was cliked");
    httpGetAsync(apiTestRoute, apiTestCallback);   
}

// set up

//const labels = [0.1, 0.2, 0.3, 0.4, 0.5];
//const data = [10, 20, 15, 25, 30, 22];

function getCurrentTime() {
    const now = new Date();
    return now.toLocaleTimeString();
}

var ctx1;
var ctx2;
var mychart1;
var mychart2;

// Funkcja do aktualizacji wykresu
function updateChart() {
    // Pobranie nowego czasu
    if(temperature == 0.0) return;
    const currentTime = getCurrentTime();
    
    // Dodanie nowego punktu do wykresu
    myChart1.data.labels.push(currentTime);
    myChart1.data.datasets[0].data.push(temperature); // Losowe dane

    // Utrzymywanie stałej liczby punktów (np. 10 ostatnich)
    if (myChart1.data.labels.length > 20) {
        myChart1.data.labels.shift(); // Usuwanie najstarszego elementu
        myChart1.data.datasets[0].data.shift();
    }

    myChart1.update();

    if(dirt == 0.0) return;
    
    // Dodanie nowego punktu do wykresu
    myChart2.data.labels.push(currentTime);
    myChart2.data.datasets[0].data.push(dirt); // Losowe dane

    // Utrzymywanie stałej liczby punktów (np. 10 ostatnich)
    if (myChart2.data.labels.length > 20) {
        myChart2.data.labels.shift(); // Usuwanie najstarszego elementu
        myChart2.data.datasets[0].data.shift();
    }

    myChart2.update();
}

// Aktualizacja co sekundę

function loadStuff()
{
    loadCanvas();

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

    setInterval(updateChart, 2000);
}

function loadCanvas()
{
    ctx1 = document.getElementById('myChart1');
    ctx2 = document.getElementById('myChart2');
    myChart1 = new Chart(ctx1.getContext('2d'), {
        type: 'line',
        data: {
            labels: [], // Początkowo pusta oś X
            datasets: [{
                label: 'Temperatura degC',
                data: [],
                borderColor: 'blue',
                backgroundColor: 'rgba(0, 0, 255, 0.3)',
                borderWidth: 2
            }]
        }
    });    
    myChart2 = new Chart(ctx2.getContext('2d'), {
        type: 'line',
        data: {
            labels: [], // Początkowo pusta oś X
            datasets: [{
                label: 'Zanieczyszczenie ppm',
                data: [],
                borderColor: 'blue',
                backgroundColor: 'rgba(0, 0, 255, 0.3)',
                borderWidth: 2
            }]
        }
    });   
}

function setWantTemp()
{
    let inputValue = parseInt(document.getElementById("setTemp").value);
    httpPost(apiSetWantTempRoute, [inputValue])
}

function setWantDirt()
{
    let inputValue = parseInt(document.getElementById("setDirt").value);
    httpPost(apiSetWantDirtRoute, [inputValue])
}