FROM ubuntu as plugin-builder

ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Berlin

RUN apt-get update
RUN apt-get install -y \
    build-essential \
    git

RUN mkdir /develop && \
    cd /develop && \
    git clone https://github.com/eclipse/mosquitto .

COPY . /develop/plugins/add-properties

WORKDIR /develop/plugins/add-properties

RUN make
