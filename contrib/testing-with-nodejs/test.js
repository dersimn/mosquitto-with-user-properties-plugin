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
    console.log('-------------------');
    console.log('Payload:', packet);
    console.log('Received by us:', now);
    console.log('Diff (received by us/received by broker):', now - packet?.properties?.userProperties?.['$TIMESTAMP']);
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
