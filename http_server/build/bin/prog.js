function httpGetAsync(resource, callback)
{
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            callback(xmlHttp.responseText);
    }
    xmlHttp.open("GET", "http://172.21.56.127:8000" + resource, true); // true for asynchronous 
    xmlHttp.send(null);
}

function apiTestCallback(responseText)
{
    console.log("got a response!");
    document.getElementById("httpResponse").value = responseText;  
}

function onButtonClick() 
{
    console.log("i was cliked");
    httpGetAsync("/api/test", apiTestCallback);   
}
