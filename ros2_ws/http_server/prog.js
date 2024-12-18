// api routes 

var apiTestRoute                = "/api/test";
var apiGetTemperatureRoute      = "/api/get/temperature";
var apiGetContaminationRoute    = "/api/get/contamination";

// config

var pollDataInterval = 1000;

// other vars

// utilities

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
