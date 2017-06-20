#ifndef C_TALKATIVE_H
#define C_TALKATIVE_H

//Client MQTT Dependencies
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>

//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;

struct TalkativeParams {
  char* wifi_ssid;
  char* wifi_password;
  char* aws_endpoint;
  char* aws_key;
  char* aws_secret;
  char* aws_region;
  int port;
};

class Talkative {
  private:
    bool reconnected;
    TalkativeParams params;
    MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> *client;

  public:
    Talkative (TalkativeParams params);
    void configureAWS ();
    bool connectMQTT ();
    void connectWIFI ();
    bool subscribe (char topic[], void (callback) (MQTT::MessageData&));
    int publish (char topic[], char payload[]);
    bool reconnect ();
    void setup();
};

#endif
