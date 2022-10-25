/**
 * Initialize functions here.
 */
 $(document).ready(function(){
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
 /**
  * Sets the interval for getting updated sensor values
  */
 function startDHTSensorInterval()
 {
     setInterval(getDHTSensorValues, 5000);
 }
