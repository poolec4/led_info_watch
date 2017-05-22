var temp_units;
var location_status;
var zip_code;
var latitude;
var longitude;

function fetchWeather(pos) {
  var xhr = new XMLHttpRequest();

  if (location_status == 1 && zip_code != 'undefined' && zip_code !== 0)
  {
    console.log("Based on custom zip");
    url = "http://api.openweathermap.org/data/2.5/weather?zip=" + zip_code + ",us" + "&appid=0a69eddcd5c88ed0db3e5f0d4c183cff";
  }
  else if (location_status == 2 && latitude != 'undefined' && latitude !== 0 && longitude != 'undefined' && longitude !== 0)
  {  
    console.log("Based on custom lat/long");
    url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
    latitude + "&lon=" + longitude + "&appid=0a69eddcd5c88ed0db3e5f0d4c183cff";
  }
  else
  {
    console.log("Based on current location");
    url = "http://api.openweathermap.org/data/2.5/weather?lat=" +
    pos.coords.latitude + "&lon=" + pos.coords.longitude + "&appid=0a69eddcd5c88ed0db3e5f0d4c183cff";
    console.log("Latitude: " + pos.coords.latitude);
    console.log("Longitude: " + pos.coords.longitude); 
  }
  
  //console.log("URL:" + url);

  xhr.open('GET',url,true);
  xhr.onload = function() {
    if(xhr.readyState === 4) {
      if(xhr.status === 200) {
        //console.log(xhr.responseText);
        var json = JSON.parse(xhr.responseText);

        var temperature;
        console.log("Temp_units " + temp_units);

        if (temp_units == 0)
        {
          temperature = Math.round(json.main.temp - 273.15);
        }
        
        if(temp_units == 1)
        {
          temperature = Math.round((json.main.temp - 273.15) * 1.8 + 32);
        }
        
        console.log("Temperature is " + temperature);

        // Conditions
        var conditions = json.weather[0].main;      
        console.log("Conditions are " + conditions);
        
        // Location
        var location = json.name;      
        console.log("Location is " + location);

        Pebble.sendAppMessage({
          "KEY_TEMPERATURE": temperature,
          "KEY_CONDITIONS": conditions,
          "KEY_POS": location
          });
      } else {
        console.log('Error');
      }
    }
  };
  xhr.send(null);
}

function locationSuccess(pos) {
  console.log(" ** Location Success **");
  fetchWeather(pos);
}

function locationError(err){
  console.log("Error reuesting location.");
  fetchWeather(pos);
}

var locationOptions = {
  'timeout': 15000,
  'maximumAge': 60000
};

Pebble.addEventListener("showConfiguration", function(e) {
  watch_info = Pebble.getActiveWatchInfo()

  if (watch_info.platform == "basalt") {
    Pebble.openURL('http://cobweb.seas.gwu.edu/~poolec/block_time/block_time_settings_page_current');
  }
  else {
    Pebble.openURL('http://cobweb.seas.gwu.edu/~poolec/block_time/block_time_settings_page_current_bw');
  }
});

Pebble.addEventListener("webviewclosed", function(e){
  var values;
  console.log("Configuration closed");
  console.log("Response = " + e.response.length + "   " + e.response);
  if (e.response !== undefined && e.response !== '' && e.response !== 'CANCELLED') {
    console.log("User hit save");
    values = JSON.parse(decodeURIComponent(e.response));
    console.log("stringified options: " + JSON.stringify((values)));
    //console.log("config values: " + values);

    var json = JSON.parse(e.response);

    temp_units = json.KEY_UNIT;
    location_status = json.KEY_LOCATION;
    zip_code = json.KEY_ZIP_CODE;
    latitude = json.KEY_LATITUDE;
    longitude = json.KEY_LONGITUDE;
    
    //console.log("Latitude: " + latitude);
    //console.log("Longitude: " + longitude);

    window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,locationOptions);

    Pebble.sendAppMessage(values,
      function(e) {
        console.log("Config sent to Pebble successfully!");
      },
      function(e) {
        console.log("Error sending config data to Pebble!");
      }
    );  
  }
});

//start to listen for the watchface to be opened
Pebble.addEventListener('ready', function(e) {
    console.log("PebbleKit JS ready!");
    console.log(e.type);
    Pebble.sendAppMessage({'KEY_JSREADY': 1});
});

//listen for when a message is received from the app
Pebble.addEventListener('appmessage', function(e) {

    console.log("AppMessage received!");

    console.log('Received appmessage: ' + JSON.stringify(e.payload));
    
    temp_units = JSON.stringify(e.payload.KEY_UNIT);
    location_status = JSON.stringify(e.payload.KEY_LOCATION);
    zip_code = JSON.stringify(e.payload.KEY_ZIP_CODE);
    latitude = JSON.stringify(e.payload.KEY_LATITUDE);
    longitude = JSON.stringify(e.payload.KEY_LONGITUDE);

    window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError,locationOptions);

  });
