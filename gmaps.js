var apiKey = 'AIzaSyDFyqGWRCugmoTLX7IaK2up2wR6KjJ37iE';
var gm = require('googlemaps');

gm.config('key', apiKey);

function parseResponse(data) {
    legs = data['routes'][0]['legs'][0];
    steps = legs['steps'];
    console.log(steps);
}

function getDirections(lat, lon) {

    gm.directions(
	lat + ',' + lon,//origin
	'37.866441,-122.265410',//destination
	function(err, data){ 	//callback
	    return parseResponse(data);
	},						
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
