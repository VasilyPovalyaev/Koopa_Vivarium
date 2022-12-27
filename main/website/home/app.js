/**
 * Initialize functions here.
 */
 $(document).ready(function(){
    getDHTSensorValues();
    getCheckboxValues();
    startDHTSensorInterval();
});   

/**
  * gets the temp and humidity values for display on the webpage
  */
 
 function getDHTSensorValues()
 {
     $.getJSON('/dhtSensor.json', function(data) {
         $("#temperature_reading0").text(data["temp0"]);
         $("#humidity_reading0").text(data["humidity0"]);
         $("#temperature_reading1").text(data["temp1"]);
         $("#humidity_reading1").text(data["humidity1"]);
     });
 }

 function getCheckboxValues()
 {
    $.getJSON('/checkbox.json', function(data) {
        $("#lights_switch").prop('checked', ((data["lights"] == "1") ? true : false));
        $("#heater_switch").prop('checked', ((data["heater"] == "1") ? true : false));
    })
 }
 /**
  * Sets the interval for getting updated sensor values
  */
 function startDHTSensorInterval()
 {
     setInterval(getDHTSensorValues, 5000);
 }

 function update_lights()
 {
    var checkbox = document.getElementById("lights_switch");
    
    var xhr = new XMLHttpRequest();
    
    if(checkbox.checked){
        xhr.open("POST", "/lightsON");
        xhr.setRequestHeader("Content-Type", "text/html");

        xhr.send("lightsValue=1");
    }else{
        xhr.open("POST", "/lightsOFF");
        xhr.setRequestHeader("Content-Type", "text/html");

        xhr.send("lightsValue=0");
    }
 }

 function update_heater()
 {
    var checkbox = document.getElementById("heater_switch");
    
    var xhr = new XMLHttpRequest();
    
    if(checkbox.checked){
        xhr.open("POST", "/heaterON");
        xhr.setRequestHeader("Content-Type", "text/html");

        xhr.send("heaterValue=1");
    }else{
        xhr.open("POST", "/heaterOFF");
        xhr.setRequestHeader("Content-Type", "text/html");

        xhr.send("heaterValue=0");
    }
 }
