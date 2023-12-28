
#ifndef MqttH
#define MqttH

#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>
#include <vector>
#include <string>
#include <AbstractDigitalInput.h>
#include <AbstractDigitalOutput.h>


/// @brief connects to wifiand mqtt, handles Requests and keeps theconnection open.
class Mqtt {
  Mqtt() : mqttClient(wifiClient) {
    mqttClient.onMessage(onMqttMessage);
  }
  static Mqtt theInstance;
  static String wifiSsid;
  static String wifiPassword;
  static String mqttIp;
  static int mqttPort;
  static String ownIp;
  static bool initialized;

public:

  /// @brief do the basic setup before the first poll
  static void setup(String  _wifiSsid, String _wifiPassword, String _mqttIp, int _mqttPort, String _ownIp = "") {
    initialized = true;
    wifiSsid = _wifiSsid;
    wifiPassword = _wifiPassword;
    mqttIp = _mqttIp;
    mqttPort = _mqttPort;
    ownIp = _ownIp;
  }

  /// @brief get the singleton instance
  /// @return the singleton instance
  static Mqtt* instance() {
    return &theInstance;
  }

  /// @brief observer for Mqtt and wifi events
  class Observer {
  public:
    struct ConnectionStatus {
    public:
      bool wifiConnected = false;
      bool mqttConnected = false;

      bool isEqual(const ConnectionStatus& rhs) const {
        return this->mqttConnected == rhs.mqttConnected
          && this->wifiConnected == rhs.wifiConnected;
      }
    };
    /// @brief called when the connection status has changed
    /// @param status the status
    virtual void onConnectionStatusChange(ConnectionStatus status) = 0;
  };

  /// @brief set the observer
  /// @param observer the observer
  void setObserver(Observer* observer) {
    this->observer = observer;
  }

  /// @brief callback interface for a registered mqtt topic
  class IMqttCallback {
  public:
    /// @brief a message to a registered
    /// @param content
    virtual void messageReceived(String content) = 0;
  };

  /// @brief subscribe to a new topic
  /// @param messageTopic the topic
  /// @param callback the callback
  void subscribe(String messageTopic, IMqttCallback* callback) {
    subscriptions.push_back(std::make_pair(messageTopic, callback));
  }

  /// @brief send a message to a specific topic
  /// @param topic the topic
  /// @param parameters the parameters, if any
  /// @return true id transfer to the server worked
  bool sendMessage(String topic, std::vector<String> parameters) {
    mqttClient.beginMessage(topic);
    for(auto parameter : parameters) {
      mqttClient.print(parameter);
    }
    return mqttClient.endMessage() != 0;
  }


  /// @brief poll the connection
  void poll() {
    if(!initialized) {
      observer->onConnectionStatusChange(connectionStatus);
    }

    if(firstpoll) {
      if(observer) {
        observer->onConnectionStatusChange(connectionStatus);
        firstpoll = false;
      }
    }

    Observer::ConnectionStatus localStatus;
    if(!WiFi.connected()) {
      if(ownIp != "") {
        IPAddress ip;
        if(ip.fromString(ownIp)) {
          WiFi.config(ip); // disable dhcp
        }
      }
      WiFi.begin(wifiSsid.c_str(), wifiPassword.c_str());
    } else if(!mqttClient.connected()) {
      mqttClient.setId("pico");
      mqttClient.connect(mqttIp.c_str(), mqttPort);

      if(mqttClient.connected()) {
        // do subscriptions again for the case we missed one because of the missing connection.
        for(auto& topic : instance()->subscriptions) {
          int ret = mqttClient.subscribe(topic.first.c_str(), 0);
          delay(200);
        }
      }
    } else {
      mqttClient.poll();
    }

    localStatus.wifiConnected = WiFi.connected();
    localStatus.mqttConnected = mqttClient.connected();

    if(observer) {
      if(!localStatus.isEqual(connectionStatus)) {
        observer->onConnectionStatusChange(localStatus);
        connectionStatus = localStatus;
      }
      firstpoll = false;
    }
  }

private:
  WiFiClient wifiClient;
  MqttClient mqttClient;

  // the state observer
  Observer* observer = nullptr;
  // the initial states
  Observer::ConnectionStatus connectionStatus = { .wifiConnected = false, .mqttConnected = false };
  bool firstpoll = true;

  // all the registered topics
  std::vector<std::pair<String, IMqttCallback*>> subscriptions;

  /// @brief the callback registered to the mqtt client
  /// @param messageSize 
  static void onMqttMessage(int messageSize) {

    if(messageSize == 0) {
      return;
    }

    // get the topic now, its not possible after the mqttClient.read() call
    // because if internal state of the MqttClient
    String messageTopic = instance()->mqttClient.messageTopic();

    String content = "";
    // use the Stream interface to print the contents
    while(instance()->mqttClient.available()) {
      content += (char)instance()->mqttClient.read();
    }

    // find the topic in the list
    for(auto topic : instance()->subscriptions) {
      if(topic.first == messageTopic) {
        topic.second->messageReceived(content);
      }
    }
  }
};

/// @brief initialisation of the singleton instance
Mqtt Mqtt::theInstance;
String Mqtt::wifiSsid;
String Mqtt::wifiPassword;
String Mqtt::mqttIp;
int Mqtt::mqttPort;
String Mqtt::ownIp;
bool Mqtt::initialized = false;

/// @brief outputs a new status to a mqtt topic, response is not checked!
class OutputMqtt : public AbstractDigitalOutput {
  String topic;
  String toggleCommand;
public:
  OutputMqtt(String topic, String toggleCommand) :
    AbstractDigitalOutput(false),
    topic(topic),
    toggleCommand(toggleCommand) {}

  void begin() {}
  void doWrite(int value) {
    Mqtt::instance()->sendMessage(topic, { toggleCommand });
  }
};

/// @brief input that is connected to a mqtt subscription
class InputMqtt : public AbstractDigitalInput, public Mqtt::IMqttCallback {
  String topic;
  String onString;
  String offString;
  bool status = false;

public:
  /// @brief constructor
  /// @param onString the content string that sets the value to true
  /// @param offString the content string that sets the value to false
  InputMqtt(String topic, String onString, String offString) :
    AbstractDigitalInput(),
    topic(topic),
    onString(onString),
    offString(offString) {}

  virtual ~InputMqtt() {}

  /// @copydoc Mqtt::IMqttCallback::messageReceived()
  void messageReceived(String content) override {
    bool changed = true;
    if(content == onString) {
      if(!status) {
        changed = true;
        status = true;
      }
    } else if(content == offString) {
      if(status) {
        changed = true;
        status = false;
      }
    } else {
      // ignore it
    }

    if(changed) {
      if(output) {
        output->write(status);
      }
    }
  }

  /// @copydoc IInput::begin()
  void begin() override {
    Mqtt::instance()->subscribe(topic, this);
  }

  /// @copydoc IInput::onRead()
  int onRead() override {
    return status;
  }

private:
};

#endif
