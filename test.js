// node file

var EventSource = require('eventsource');

var deviceID = "54ff6e066678574948210367";
var accessToken = "8b8997fcad2f1720ffdd200a84f3e855d629bd71";
var url = "https://api.spark.io/v1/devices/" + deviceID + "/events/?access_token=" + accessToken;

var es = new EventSource(url);

es.addEventListener('button', function(e) {
    console.log(e.data);
    var data = JSON.parse(e.data);
    console.log(data['data']);
});

console.log('started!');
