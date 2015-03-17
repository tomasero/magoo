// node file
var EventSource = require('eventsource');
var math = require('math');
var rest = require('restler');

var deviceID = "54ff6e066678574948210367";
var accessToken = "8b8997fcad2f1720ffdd200a84f3e855d629bd71";
var accessTokenURI = "/?access_token=" + accessToken;
var device = "https://api.spark.io/v1/devices/" + deviceID;
var eventsURI = device + "/events" + accessTokenURI;
var eventSrc = new EventSource(eventsURI);
var magnetometerURI = device + '/compass' + accessTokenURI;
var vibrateURI = device + "/vibrate" + accessTokenURI;

eventSrc.addEventListener('GPS', function(e) {
    var data = JSON.parse(e.data);
    //console.log(data['data']);
    var gps = JSON.parse(data['data']);
    
    var lat = parseLatitude(gps['lat']);    
    var lon = parseLongitude(gps['lon']);

    console.log('lat: ' + lat + ', lon: ' + lon);
    getDirections(lat, lon, function(err, data) {
        if (data != undefined) {
            parsed = parseResponse(data);
            if (parsed.length > 0) {
                var nextStep = getNextStep(parsed);
                var angle = getAngle(lat, lon, nextStep.lat, nextStep.lon);
                isSameDirection(angle%360);
            } else {
                console.log('No GPS connection!');
            }
        }
    });
});

eventSrc.addEventListener('status', function (e) {
    var data = JSON.parse(e.data);
    console.log(data);
});
// es.addEventListener('GPS_RAW', function(e) {
//     var data = JSON.parse(e.data);
//     console.log(data['data']);
// });

eventSrc.addEventListener('pressed', function (e) {
    var data = JSON.parse(e.data)['data'];
    if (data == 'release') {
        //run the while loop
    }
});


function sendVibrate(on) {
    rest.post(vibrateURI).on('complete', function (data, response) {
        // console.log(response);
    });
}

function isSameDirection(correctDir) {
    getMagnetometer(function (myDir) {
        var range = 3;
        console.log('correctDir: ' + correctDir);
        console.log('myDir: ' + myDir);
        if (isWithinRange (range, correctDir, myDir)) {
            sendVibrate(1);
        } else {
            sendVibrate(0);
        }
    })
}

function isWithinRange(range, correctDir, myDir) {
    var low = (correctDir - range) % 360
    var high = (correctDir + range) % 360
    console.log(myDir > low && myDir < high);
    return myDir > low && myDir < high;
}

function getMagnetometer(callback) {
    rest.get(magnetometerURI).on('complete', function (data) {
        var dir = data['result'];
        callback(dir);
    });
}


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
    if (data != undefined) {
        routes = data['routes'];
        legs = routes[0]['legs'][0];
        steps = legs['steps'];
        return steps;     
    }
    return [];
}

function getDirections(lat, lon, callback) {
    
    gm.directions(
	lat + ',' + lon,//origin
	'37.866441,-122.265410',//destination
        callback,
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

function getNextStep(data) {
    var nextStep = data[0]['end_location'];
    var lat = nextStep['lat'];
    var lon = nextStep['lng'];
    return {'lat': lat, 'lon': lon};
}

function getAngle(lat1, lon1, lat2, lon2) {
    console.log(lat1, lon1, lat2, lon2);
    var num = lat2 - lat1;
    var denom = lon2 - lon1;
    var res = math.atan(num/denom)*180/math.PI;
    return res + 360;
}
