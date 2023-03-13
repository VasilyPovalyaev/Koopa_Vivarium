/**
 * Add gobals here
 */
 let seconds 	= null;
 let otaTimerVar =  null;
 let wifiConnectInterval = null;
 
 /**
  * Initialize functions here.
  */
 $(document).ready(function(){
     getUpdateStatus();
     getConnectInfo();
     $("#connect_wifi").on("click", function(){
         checkCredentials();
     });
     $("#disconnect_wifi").on("click", function(){
         disconnect_wifi();
     });
 });   
 
 /**
  * Gets file name and size for display on the web page.
  */        
 function getFileInfo() 
 {
     let x = document.getElementById("selected_file");
     let file = x.files[0];
 
     document.getElementById("file_info").innerHTML = "<h4>File: " + file.name + "<br>" + "Size: " + file.size + " bytes</h4>";
 }
 
 /**
  * Handles the firmware update.
  */
 function updateFirmware() 
 {
     // Form Data
     let formData = new FormData();
     let fileSelect = document.getElementById("selected_file");
     
     if (fileSelect.files && fileSelect.files.length == 1) 
     {
         let file = fileSelect.files[0];
         formData.set("file", file, file.name);
         document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";
 
         // Http Request
         let request = new XMLHttpRequest();
 
         request.upload.addEventListener("progress", updateProgress);
         request.open('POST', "/OTAupdate");
         request.responseType = "blob";
         request.send(formData);
     } 
     else 
     {
         window.alert('Select A File First')
     }
 }
 
 /**
  * Progress on transfers from the server to the client (downloads).
  */
 function updateProgress(oEvent) 
 {
     if (oEvent.lengthComputable) 
     {
         getUpdateStatus();
     } 
     else 
     {
         window.alert('total size is unknown')
     }
 }
 
 /**
  * Posts the firmware udpate status.
  */
 function getUpdateStatus() 
 {
     let xhr = new XMLHttpRequest();
     let requestURL = "/OTAstatus";
     xhr.open('POST', requestURL, false);
     xhr.send('ota_update_status');
 
     if (xhr.readyState == 4 && xhr.status == 200) 
     {		
         let response = JSON.parse(xhr.responseText);
                         
          document.getElementById("latest_firmware").innerHTML = response.compile_date + " - " + response.compile_time
 
         // If flashing was complete it will return a 1, else -1
         // A return of 0 is just for information on the Latest Firmware request
         if (response.ota_update_status == 1) 
         {
             // Set the countdown timer time
             seconds = 10;
             // Start the countdown timer
             otaRebootTimer();
         } 
         else if (response.ota_update_status == -1)
         {
             document.getElementById("ota_update_status").innerHTML = "!!! Upload Error !!!";
         }
     }
 }
 
 /**
  * Displays the reboot countdown.
  */
 function otaRebootTimer() 
 {	
     document.getElementById("ota_update_status").innerHTML = "OTA Firmware Update Complete. This page will close shortly, Rebooting in: " + seconds;
 
     if (--seconds == 0) 
     {
         clearTimeout(otaTimerVar);
         window.location.reload();
     } 
     else 
     {
         otaTimerVar = setTimeout(otaRebootTimer, 1000);
     }
 }
 
 
 /**
  * Clears the connection status interval
  */
 
 function stopWifiConnectStatusInterval()
 {
     if (wifiConnectInterval != null)
     {
         clearInterval(wifiConnectInterval);
         wifiConnectInterval = null;
     }
 }
 
 /**
  * Gets the WiFI connection status
  */
 
 function getWifiConnectStatus()
 {
     let xhr = new XMLHttpRequest();
     let requestURL = "/wifiConnectStatus";
     xhr.open('POST', requestURL, false);
     xhr.send('wifi_connect_status');
 
     if (xhr.readyState == 4 && xhr.status == 200)
     {
         let response = JSON.parse(xhr.responseText);
 
         document.getElementById("wifi_connect_status").innerHTML = "Connecting...";
 
         if (response.wifi_connect_status == 2)
         {
             document.getElementById("wifi_connect_status").innerHTML = "<h4 class='rd'>Failed to Connect. Please check your AP credentials and compatability</h4>";
             stopWifiConnectStatusInterval();
         }
         else if (response.wifi_connect_status == 3)
         {
             document.getElementById("wifi_connect_status").innerHTML = "<h4 class='gr'>Connection Success!</h4>";
             stopWifiConnectStatusInterval();
             getConnectInfo()
         }
     }
 
 }
 
 /**
  * starts the interval for checking the connection status
  */
 function startWifiConnectStatusInterval()
 {
     wifiConnectInterval = setInterval(getWifiConnectStatus, 2800);
 
 }
 
 /**
  * Connect WiFi function called using the SSID and Password entered
  */
 function connectWifi()
 {
     //get the ssid and password
     let selectedSSID = $("#connect_ssid").val();
     let pwd = $("#connect_pass").val();
    //  getConnectInfo();
 
     $.ajax({
         url: '/wifiConnect.json',
         datatype: 'json',
         method: 'POST',
         cache: false,
         headers: {'my-connect-ssid': selectedSSID, 'my-connect-pwd': pwd},
         data: {'timestamp': Date.now()}
     });
 
     startWifiConnectStatusInterval();
 }
 
 /**
  * Checks the credentials on connect_wifi button click
  */
 
 function checkCredentials()
 {
     let errorList = "";
     let credsOk = true;
 
     let selectedSSID = $("#connect_ssid").val();
     let pwd = $("#connect_pass").val();
 
     if(selectedSSID == "")
     {
         errorList += "<h4 class='rd'>SSID cannot be empty!</h4>";
         credsOk = false;
     }
 
     if(pwd == "")
     {
         errorList += "<h4 class='rd'>Password cannot be empty!</h4>";
         credsOk = false;
     }
 
     if(!credsOk)
     {
         $("#wifi_connect_credentials_errors").html(errorList);
     }
     else
     {
         $("#wifi_connect_credentails_errors").html("");
         connectWifi();
     }
 
 }
 
 /**
  * Shows the wifi password if the box is checked
  */
 function showPassword()
 {
     let x = document.getElementById("connect_pass");
     if (x.type === "password")
     {
         x.type = "text";
     }
     else
     {
         x.type = "password"
     }
 }

 /**
  * Gets the connection info for dispalying on the webpage
  */

 function getConnectInfo()
 {
    $.getJSON('/wifiConnectInfo.json', function(data)
    {
        $("#connected_ap_label").html("Connected to: ");
        $("#connected_ap").text(data["ap"]);

        $("#ip_address_label").html("IP Adress: ");
        $("#wifi_connect_ip").text(data["ip"]);

        $("#netmask_label").html("Netmask: ");
        $("#wifi_connect_netmask").text(data["netmask"]);

        $("#gateway_label").html("Gateway: ");
        $("#wifi_connect_gw").text(data["gateway"]);

        document.getElementById('disconnect_wifi').style.display = 'block';
    });
 }

 /**
  * Disconnects the WiFi once the button is pressed and reloads the webpage
  */
 function disconnect_wifi()
 {
    $.ajax({
        url: '/wifiDisconnect.json',
        dataType: 'json',
        method: 'DELETE',
        cache: false,
        data: { 'timestamp': Date.now() }
    });
    //update the web page
    setTimeout("location.reload(true);", 2000);
 }