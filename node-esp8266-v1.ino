#include "env.h"
#include "talkative.h"

const int RELAY_PIN = 2;
bool relay_state = true;

Talkative talker;

void peripheralCallback (MQTT::MessageData& md) {
  MQTT::Message &message = md.message;

  char* msg = new char[message.payloadlen + 1]();
  memcpy(msg,message.payload,message.payloadlen);

  StaticJsonBuffer<30> reqJsonBuffer;
  JsonObject& reqRoot = reqJsonBuffer.parseObject(msg);
  const char* type = reqRoot["type"];
  Serial.println(type);
  delete msg;

  //TODO: add validation if has valid attributes {type}

  if (strcmp(type, "turn-on") == 0) {
    digitalWrite(RELAY_PIN, HIGH);
    relay_state = true;
  } else if (strcmp(type, "turn-off") == 0) {
    digitalWrite(RELAY_PIN, LOW);
    relay_state = false;
  } else if (strcmp(type, "toggle") == 0) {
    digitalWrite(RELAY_PIN, !relay_state);
    relay_state = !relay_state;
  } else if (strcmp(type, "get-info") == 0) {
    //nothing
  } else {
    Serial.println("Invalid type");
    return;
  }

  StaticJsonBuffer<200> resJsonBuffer;
  JsonObject& resRoot = resJsonBuffer.createObject();
  resRoot["type"] = type;
  resRoot["state"] = relay_state;

  char buf[100];
  resRoot.printTo(buf, 100);

  char topic[39] = "/devices/";
  strcat(topic, DEVICE_ID);
  strcat(topic, "/peripheral:response");

  talker.publish(topic, buf);
}

void subscriber () {
  char topic[30] = "/devices/";
  strcat(topic, DEVICE_ID);
  strcat(topic, "/peripheral");

  talker.subscribe("/devices/0000000001/peripheral", peripheralCallback);
}

void setup() {
    Serial.begin (115200);
    delay (2000);
    Serial.setDebugOutput(1);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);

    talkativeParams params = {
      WIFI_SSID,
      WIFI_PASSWORD,
      AWS_ENDPOINT,
      AWS_KEY,
      AWS_SECRET,
      AWS_REGION,
      PORT
    };

    talker = Talkative(params);
    talker.setup();
    if (talker.connectMQTT()){
      subscriber();
    }
}

void loop() {
  if (talker.reconnect()) {
    subscriber();
  }
}
