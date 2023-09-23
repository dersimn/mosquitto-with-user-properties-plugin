var mqtt = require('mqtt')
var client  = mqtt.connect('mqtt://localhost', {
    clientId: 'myClient123-retained',
    username: 'myUser-retained',
    password: 'myPassword-retained',
    protocolVersion: 5
})

client.on('error', function (error) {
    console.log(error)
})

client.on('connect', function () {
    client.subscribe('test-retained', {
        // For details,
        // see https://github.com/mqttjs/MQTT.js/tree/v5.0.5#mqttclientsubscribetopictopic-arraytopic-object-options-callback
        // and https://docs.oasis-open.org/mqtt/mqtt/v5.0/os/mqtt-v5.0-os.html#_Toc3901169


        //  true: received messages will keep the retain flag they were published with
        // false: only stale messages (messages who were saved by the broker) have retain flag set to true
        rap: false,

        // 0: Send retained messages at the time of the subscribe
        // 1: Send retained messages at subscribe only if the subscription does not currently exist
        // 2: Do not send retained messages at the time of the subscribe
        rh: 0,

        //  true: messages with a ClientID equal to the ClientID of the publishing connection are not sent
        //        i.e. we won't see out own messages here
        nl: false,
    });
})

client.on('message', function (topic, message, packet) {
    const now = Date.now();
    console.log('-------------------');
    console.log('Payload:', packet);
    console.log('Received by us:', now);
    console.log('Diff (received by us/received by broker):', now - packet?.properties?.userProperties?.['$TIMESTAMP']);
})

var ticker = 0;
setTimeout(() => {
    client.publish('test-retained', JSON.stringify({val: ticker++}), {
        retain: true,
        properties: {
            userProperties: {
                'foo': 'bar'
            }
        }
    })
}, 1000);
