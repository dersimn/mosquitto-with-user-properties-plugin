## Dev

    docker build -t test .
    docker run -d --rm -p 1883:1883 -p 9001:9001 test