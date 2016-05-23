#include "application.h"
#include "Adafruit_BMP085/Adafruit_BMP085.h"
#include "MQTT/MQTT.h"

#define CONNECTION_TIMEOUT_MSEC     4000    // Timeout interval for MQTT server
#define TCP_FLUSH_DELAY_MSEC        1000    // Emperically found delay required to flush TCP segments before going to sleep.
#define DEEP_SLEEP_PERIOD_SEC       5
#define NUM_READINGS                3       // Device caches this many readings per sensor during a period equal to NUM_READINGS * DEEP_SLEEP_PERIOD_SEC before reporting.
#define NUM_SENSORS                 4
#define MQTT_SERVER_URI             "broker.hivemq.com"
#define MQTT_SERVER_PORT            1883
#define MQTT_CLIENT_NAME            "bu-sph-iot"
#define PIN_PIR_SENSOR              A5
#define PIN_CO_SENSOR               A1
const String sensorTypes[] = { "temp", "pressure", "co", "motion" };

SYSTEM_MODE(MANUAL);    // Set manual mode to control invocation of Particle.connect() and Particle.process()
STARTUP(System.enableFeature(FEATURE_RETAINED_MEMORY));   // Enable Backup RAM for retained variables
retained uint32_t timestamps[NUM_READINGS];
retained float data[NUM_READINGS][NUM_SENSORS];
retained uint16_t wakeCount;
retained bool deviceInit = false;   // Flag to detect first startup

void callback(char* topic, uint8_t* payload, unsigned int length) {};
MQTT client(MQTT_SERVER_URI, MQTT_SERVER_PORT, callback);

Adafruit_BMP085 bmp;

void setup() {
    if(!deviceInit){
        deviceInit = true;
        wakeCount = 0;
        Particle.connect();         // Connecting to Particle cloud syncs RTC
    }
    pinMode(D7, OUTPUT);        // Status indicator LED on D7.
    pinMode(PIN_PIR_SENSOR, INPUT);
    pinMode(PIN_CO_SENSOR, INPUT);
    bmp.begin();


    timestamps[wakeCount] = Time.now();
    data[wakeCount][0] = bmp.readTemperature();
    data[wakeCount][1] = bmp.readPressure();
    data[wakeCount][2] = 1.0;
    data[wakeCount][3] = digitalRead(PIN_PIR_SENSOR);
    wakeCount = (wakeCount + 1) % NUM_READINGS;

    if(wakeCount == 0){
      WiFi.on();
      WiFi.connect();
      while(!WiFi.ready()) { 
        digitalWrite(D7, !digitalRead(D7));
        delay(100);
      };
      client.connect(MQTT_CLIENT_NAME);
      uint32_t connectionTimeout = millis() + CONNECTION_TIMEOUT_MSEC;
      while(!client.isConnected()){
        digitalWrite(D7, !digitalRead(D7));
        delay(100);
        if(millis() > connectionTimeout)  return;
      }
      for(int i=0; i<NUM_SENSORS; i++){
        String topic = String("/bu-sph-iot/" + Particle.deviceID() + "/" + sensorTypes[i]);
        String payload;
        for(int j=0; j<NUM_READINGS; j++){
          payload.concat(String(timestamps[j]) + ",");
          payload.concat(String(data[j][i]) + "\\n");
        }
        client.publish(topic, payload);
        client.loop();
      }
      client.disconnect();
      delay(TCP_FLUSH_DELAY_MSEC);    // This delay allows time to flush TCP output buffer
    }
    System.sleep(SLEEP_MODE_DEEP, DEEP_SLEEP_PERIOD_SEC);  // Disconnects from Particle cloud and switches WiFi and MCU off
}

void loop(){}
