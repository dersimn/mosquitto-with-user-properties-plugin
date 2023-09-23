# Purpose

Plugin that adds userProperties

- `$TIMESTAMP`: Exact unix epoch (in ms) when the broker received a published message
- `$CLIENT_ID`: Client ID of sender
- `$CLIENT_USERNAME`: Client Username of sender

to every message on the broker side.  
Try yourself with:

    cd contrib/testing-with-nodejs
    npm i
    node test-retained

stop with <kbd>Ctrl</kbd> + <kbd>C</kbd>. Wait a few seconds and run

    node test-retained

again. Example output:

```
Payload: Packet {
  cmd: 'publish',
  retain: true,
  qos: 0,
  dup: false,
  length: 135,
  topic: 'test-retained',
  payload: <Buffer 7b 22 76 61 6c 22 3a 30 7d>,
  properties: {
    userProperties: [Object: null prototype] {
      foo: 'bar',
      '$TIMESTAMP': '1695466515661',
      '$CLIENT_ID': 'myClient123-retained',
      '$CLIENT_USERNAME': 'myUser-retained'
    }
  }
}
Received by us: 1695466523203
Diff (received by us/received by broker): 7542
```

As you can see, `$TIMESTAMP` is still the value from our first pass (the exact time the broker received the message). The difference between now and `$TIMESTAMP` is 7542 ms in this example, so we now know how old the retained message is.

# Why?

For example, if you have battery-powered sensors that send values only every few minutes. You probably want to retain these messages to display them in a user interface, but you still want to know how old a received message is. If the sensor doesn't have a built-in RTC, it can't attach a useful timestamp itself - so why not let the broker do it?

# Dev

    docker build -t test .
    docker run -d --rm -p 1883:1883 -p 9001:9001 test
