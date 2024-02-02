#include <WiFi.h> // include the wifi library

#include "DHT20.h"  //we need a special ESP library for the DHT sensor
DHT20 dht;  //establishing an object named dht

const int buttonPin = 5; // the button is connected to GPIO5
int buttonState = 0; // defining and initalizing buttonState as low 

/*we only want button actions to be triggered upon button push, not the entire
time while the button IS pushed... and we used this variable to help*/
int lastbuttonState = 0; 

//declaring variables to store humidity and temperature
float h;
float tc;
float tf;

// WiFi credentials - enter network name and password here
// For UVA wahoo network, see comments on each line
const char *ssid = "*****"; // "wahoo"
const char *pwd = "*****"; // ""

// The below funtion connects to wifi and
// prints out the mac address of your ESP32

void wifi_init()  
{
  // Get MAC Address:
  Serial.print("Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  WiFi.mode(WIFI_STA); // Acting as a client
  WiFi.begin(ssid, pwd);
  Serial.println("\nConnecting to Wifi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  pinMode(buttonPin, INPUT); // typical Arduino syntax
  Serial.begin(115200); //starting up the serial monitor at 9600 baud rate

/* following two lines are necessary to set up I2C
communication with the DHT20
*/
  Wire.begin();
  dht.begin();   

// ESP32-C3-DevKitC-02 I2C communication pins
// SDA is GPIO8
// SCL is  GPIO9
//          +--------------+
//  VDD ----| 1            |
//  SDA ----| 2    DHT20   |
//  GND ----| 3            |
//  SCL ----| 4            |
//          +--------------+

  // Call the wifi_init function to initialize the WiFi connection
  wifi_init();

  delay(1000);
}

void loop() {

// Loop from the DHT20 Example code to read the data
if (millis() - dht.lastRead() >= 1000)
  {
    //  READ DATA
    uint32_t start = micros();
    int status = dht.read();
    uint32_t stop = micros();

    // Read the humidity
    h = dht.getHumidity();
    
    // Read temperature as Celsius
    tc = dht.getTemperature();
    
    // Convert Celsius to Fahrenheit 
    tf = tc*9/5+32;
    }

buttonState=digitalRead(buttonPin);  // read the button, HiGH means the button is pushed


// if the button is pushed and the prior state was that the button was unpushed, do the loop
if (buttonState == HIGH && buttonState!=lastbuttonState) 
  {
     // print it all out to the serial monitor
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(tc);
    Serial.print(F("°C, "));
    Serial.print(tf);
    Serial.println(F("°F"));

    delay(1500);  //this delay helps "debounce" the button
  } //end the button press loop

lastbuttonState=buttonState; //updating the reading of the button

} //end the void loop
