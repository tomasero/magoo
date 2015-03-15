// node file

var EventSource = require('eventsource');

var deviceID = "54ff6e066678574948210367";
var accessToken = "8b8997fcad2f1720ffdd200a84f3e855d629bd71";
var url = "https://api.spark.io/v1/devices/" + deviceID + "/events/?access_token=" + accessToken;

var es = new EventSource(url);

es.addEventListener('GPS', function(e) {
    var data = JSON.parse(e.data);
    //console.log(data['data']);
    var gps = JSON.parse(data['data']);
    
    var lat = parseLatitude(gps['lat']);    
    var lon = parseLongitude(gps['lon']);

    console.log('lat: ' + lat + ', lon: ' + lon);
    getDirections(lat, lon, function(err, data) {
        console.log(parseResponse(data));
    });
});

// es.addEventListener('GPS_RAW', function(e) {
//     var data = JSON.parse(e.data);
//     console.log(data['data']);
// });


// adapted from
// http://arduinodev.woofex.net/2013/02/06/adafruit_gps_forma/
function convertDegMinToDecDeg (degMin) {
    // get minutes
    var min = degMin % 100;
    // get degrees
    var deg = Math.floor(degMin / 100);
        
    return deg + (min / 60.0);
}

function parseLongitude(lon) {
    var out = convertDegMinToDecDeg(parseFloat(lon.substring(1)));
    if(lon[0] == 'W') {
        out = -1 * out;
    }
    return out;
}

function parseLatitude(lon) {
    var out = convertDegMinToDecDeg(parseFloat(lon.substring(1)));
    if(lon[0] == 'S') {
        out = -1 * out;
    }
    return out;
}

var apiKey = 'AIzaSyDFyqGWRCugmoTLX7IaK2up2wR6KjJ37iE';
var gm = require('googlemaps');

gm.config('key', apiKey);

function parseResponse(data) {
    legs = data['routes'][0]['legs'][0];
    steps = legs['steps'];
    //console.log(steps);
    return steps;
}

function getDirections(lat, lon, callback) {
    
    gm.directions(
	lat + ',' + lon,//origin
	'37.866441,-122.265410',//destination
        callback,
	// function(err, data){ 	//callback
	//     return parseResponse(data);
	// },						
	null,					//sensor
	'walking',				//mode
	null,					//waypoints
	false,					//alternatives
	null,					//avoid
	'metric'				//units
	//language
	//departure
    );

}
