FROM alpine as plugin-builder

RUN apk add gcc
RUN apk add git
RUN apk add make
RUN apk add musl-dev

RUN mkdir /develop && \
    cd /develop && \
    git clone https://github.com/eclipse/mosquitto .

COPY . /develop/plugins/add-properties

WORKDIR /develop/plugins/add-properties

RUN make

# ---

FROM eclipse-mosquitto

COPY mosquitto.conf /mosquitto/config/mosquitto.conf
COPY --from=plugin-builder /develop/plugins/add-properties/add_properties.so /mosquitto/plugins/add_properties.so

EXPOSE 1883
EXPOSE 9001
