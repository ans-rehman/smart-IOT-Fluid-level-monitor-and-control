/*  TITLE: IoT-based Water Level Indicator using ESP32, Ultrasonic Sensor & Blynk with 0.96" OLED */




#define BLYNK_TEMPLATE_ID "TMPL63zWBnRc3"
#define BLYNK_TEMPLATE_NAME "iot base water level monitering system"
#define BLYNK_AUTH_TOKEN "pTCJQooJZQp68ocM9kh8dAfGQ_KP299T"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "A.R";
char pass[] = "anS@Rehman";

//Set Water Level Distance in CM
int emptyTankDistance = 7.5 ;  //Distance when tank is empty
int fullTankDistance =  2.5 ;  //Distance when tank is full

//Set trigger value in percentage
int triggerPer =   10 ;  //alarm will start when water level drop below triggerPer

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <AceButton.h>
using namespace ace_button; 

// Define connections to sensor
#define TRIGPIN    27  //D27
#define ECHOPIN    26  //D26
#define wifiLed    2   //D2
#define ButtonPin1 12  //D12
#define pump 13  //D13

//Change the virtual pins according the rooms
#define VPIN_BUTTON_0    V0
#define VPIN_BUTTON_1    V1 
#define VPIN_BUTTON_2    V2



float duration;
float distance;
int   waterLevelPer;

char auth[] = BLYNK_AUTH_TOKEN;

ButtonConfig config1;
AceButton button1(&config1);

void handleEvent1(AceButton*, uint8_t, uint8_t);

BlynkTimer timer;

void checkBlynkStatus() { // called every 3 seconds by SimpleTimer

  bool isconnected = Blynk.connected();
  if (isconnected == false) {
    //Serial.println("Blynk Not Connected");
    digitalWrite(wifiLed, LOW);
  }
  if (isconnected == true) {
    digitalWrite(wifiLed, HIGH);
    //Serial.println("Blynk Connected");
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_BUTTON_1);
  Blynk.syncVirtual(VPIN_BUTTON_2);
  Blynk.syncVirtual(VPIN_BUTTON_0);
}

BLYNK_WRITE(V0) // Executes when the value of virtual pin 0 changes
{
  if(param.asInt() == 1)
  {
    // execute this code if the switch widget is now ON
    digitalWrite(2,HIGH);  // Set digital pin 2 HIGH
  }
  else
  {
    // execute this code if the switch widget is now OFF
    digitalWrite(2,LOW);  // Set digital pin 2 LOW    
  }
}


void measureDistance(){
  // Set the trigger pin LOW for 2uS
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
 
  // Set the trigger pin HIGH for 20us to send pulse
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(20);
 
  // Return the trigger pin to LOW
  digitalWrite(TRIGPIN, LOW);
 
  // Measure the width of the incoming pulse
  duration = pulseIn(ECHOPIN, HIGH);
 
  // Determine distance from duration
  // Use 343 metres per second as speed of sound
  // Divide by 1000 as we want millimeters
 
  distance = ((duration / 2) * 0.343)/10;

  requiredlevel=Blynk.virtualRead(VPIN_BUTTON_2);
  if (distance > (fullTankDistance - 10)  && distance < emptyTankDistance){
    waterLevelPer = map((int)distance ,emptyTankDistance, fullTankDistance, 0, 100);
    if (waterlevelPer <= requiredlevel && VPIN_BUTTON_0 == 1){
        digitalwrite(pump,HIGH);
    }
    Blynk.virtualWrite(VPIN_BUTTON_1, waterLevelPer);

    // Print result to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  
  // Delay before repeating measurement
  delay(100);
}

 
void setup() {
  // Set up serial monitor
  Serial.begin(115200);
 
  // Set pinmodes for sensor connections
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(pump, OUTPUT);


  digitalWrite(wifiLed, LOW);
  digitalWrite(pump, LOW);

  config1.setEventHandler(button1Handler);
  
  button1.init(ButtonPin1);


  WiFi.begin(ssid, pass);
  timer.setInterval(2000L, checkBlynkStatus); // check if Blynk server is connected every 2 seconds
  Blynk.config(auth);
  delay(1000);
 
}
 void loop() {

  measureDistance();

  Blynk.run();
  timer.run(); // Initiates SimpleTimer

  button1.check();
   
}

void button1Handler(AceButton* button, uint8_t eventType, uint8_t buttonState) {
  Serial.println("EVENT1");
  switch (eventType) {
    case AceButton::kEventReleased:
      //Serial.println("kEventReleased");
      digitalWrite(BuzzerPin, LOW);
      toggleBuzzer = LOW;
      break;
  }
}
