var mqtt = require('mqtt')
var client  = mqtt.connect('mqtt://localhost', {
    clientId: 'myClient123',
    username: 'myUser',
    password: 'myPassword',
    protocolVersion: 5
})

client.on('error', function (error) {
    console.log(error)
})

client.on('connect', function () {
    client.subscribe('#');
})

client.on('message', function (topic, message, packet) {
    const now = Date.now();
    console.log(packet, now, now - packet?.properties?.userProperties?.['$TIMESTAMP']);
})

var ticker = 0;
setInterval(() => {
    client.publish('test', JSON.stringify({val: ticker++}), {
        properties: {
            userProperties: {
                'foo': 'bar'
            }
        }
    })
}, 1000);
