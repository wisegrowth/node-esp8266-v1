#include "talkative.h"
#include "util.h"

//WIFI Dependencies
#include <ESP8266WiFiMulti.h>

//AWS MQTT Websocket Dependencies
#include "AWSWebSocketClient.h"

AWSWebSocketClient awsWSclient(1000);
IPStack ipstack(awsWSclient);

Talkative::Talkative (TalkativeParams params) {
  this->params = params;
  this->reconnected = false;

  MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> *client = NULL;
}

void Talkative::configureAWS () {
  awsWSclient.setAWSRegion(this->params.aws_region);
  awsWSclient.setAWSDomain(this->params.aws_endpoint);
  awsWSclient.setAWSKeyID(this->params.aws_key);
  awsWSclient.setAWSSecretKey(this->params.aws_secret);
  awsWSclient.setUseSSL(true);
}

bool Talkative::connectMQTT () {
  if (client == NULL) {
    client = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
  } else {
    if (client->isConnected ()) {
      client->disconnect ();
    }

    delete client;
    client = new MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers>(ipstack);
  }

  int statusConnection = ipstack.connect(this->params.aws_endpoint, this->params.port);
  if (statusConnection != 1) {
    Serial.println(F("Error while connect to websocket server"));
    return false;
  }

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.MQTTVersion = 3;
  char* clientID = Util::generateClientID();
  data.clientID.cstring = clientID;
  statusConnection = client->connect(data);
  delete[] clientID;

  if (statusConnection != 0) {
    Serial.print(F("Error while try connect to MQTT server, status: "));
    Serial.println(statusConnection);
    return false;
  }

  Serial.println(F("MQTT connected successful"));
  return true;
}

void Talkative::connectWIFI () {
  ESP8266WiFiMulti WiFiMulti;
  
  WiFiMulti.addAP(this->params.wifi_ssid, this->params.wifi_password);
  while(WiFiMulti.run() != WL_CONNECTED) { delay(100); }
  Serial.println (F("WIFI Connected"));
}

bool Talkative::subscribe (char topic[], void (callback) (MQTT::MessageData&)) {
  int response = client->subscribe(topic, MQTT::QOS1, callback);
  if (response != 0) {
    Serial.print(F("MQTT subscribe status: "));
    Serial.println(response);
    return false;
  }

  Serial.print(F("MQTT subscribed to: "));
  Serial.println(topic);
  return true;
}

int Talkative::publish (char topic[], char payload[]) {
  char buf[100];
  strcpy(buf, payload);

  MQTT::Message message;
  message.qos = MQTT::QOS1;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf) + 1;

  return client->publish(topic, message);
}

bool Talkative::reconnect () {
  if (awsWSclient.connected()) {
      client->yield();
      return false;
  } else {
    if (this->connectMQTT()){
      this->reconnected = true;
      return true;
    }

    return false;
  }
}

void Talkative::setup () {
  this->configureAWS();
  this->connectWIFI();
}
