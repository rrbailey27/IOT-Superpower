#include <WiFi.h> 
#include "DHTesp.h"  //we need a special ESP library for the DHT sensor
#include <MySQL_Connection.h> // added to make the MySQL connection
#include <MySQL_Cursor.h> // added to make the MySQL connection

DHTesp dht;  //establishing an object named dht

const int buttonPin = 5; // the button is connected to GPIO5
int buttonState = 0; // defining and initalizing buttonState as low 

/*we only want button actions to be triggered upon button push, not the entire
time while the button IS pushed... and we used this variable to help*/

int lastbuttonState = 0; 

IPAddress server_addr(52,44,97,82); 
char user[] = "reid";              // MySQL user
char pass[] = "tlpassword";      // MySQL password
char db[] = "helloesp32";          // MySQL database name

//insitiate the MySQL connection
WiFiClient client;
MySQL_Connection conn((Client *)&client);

// define string variables needed for the sensor values and the query
char cchar[8];
char fchar[8];
char hchar[8];
char hicchar[8];
char hifchar[8];
char query[128];

// WiFi credentials
const char *ssid = "ORBI53"; //Wifi Network
const char *pwd = "heavysheep735"; //Wifi Network



void wifi_init()
{
  // Get MAC Address:
  Serial.print("ESP Board MAC Address:  ");
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




void setup() 
{
  pinMode(buttonPin, INPUT); // typical Arduino syntax
  Serial.begin(115200); //starting up the serial monitor at 115200 baud rate

  dht.setup(4, DHTesp::DHT22); // Connect DHT sensor to GPIO 4


  while (!Serial); // Wait for serial port to connect

  // Initialize WiFi
  wifi_init();

  // Connect to MySQL
  if (conn.connect(server_addr, 3306, user, pass, db)) {
     delay(1000);
  } else {
    Serial.println("Connection failed.");
  }
}

void loop() {


buttonState=digitalRead(buttonPin);  // read the button, HiGH means the button is pushed

// if the button is pushed and the prior state was that the button was unpushed, do the loop
//if (buttonState == HIGH && lastbuttonState == LOW) {
 

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.getHumidity();
  
  // Read temperature as Celsius (the default)
  float c = dht.getTemperature();
  
  // Read temperature as Fahrenheit 
  float f = dht.toFahrenheit(t);
  
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  // Compute heat index in Fahrenheit (the default)
  float hif =   dht.computeHeatIndex(f, h, true);
 

// print it all out to the serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(c);
  Serial.print(F("°C, "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C, "));
  Serial.print(hif);
  Serial.println(F("°F"));

  // Create INSERT statement
  // first convert the floating numbers to strings
  dtostrf(c, 1, 2, cchar);
  dtostrf(f, 1, 2, fchar);
  dtostrf(h, 1, 2, hchar);
  dtostrf(hic, 1, 2, hicchar);
  dtostrf(hif, 1, 2, hifchar);
 
 //form the query and store it in the character sring "query" using sprintf
  sprintf(query, "INSERT INTO esp32_dht (tempC, tempF, humidity, heatindexC, heatindexF) VALUES (%s,%s,%s,%s,%s)", cchar, fchar, hchar, hicchar, hifchar);
  //sprintf(query, "INSERT INTO esp32_dht (tempC) VALUES (%f)", t);

// Execute INSERT statement
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;

  // Wait before repeating
  delay(60000);  // Adjust as needed

//}

  lastbuttonState=buttonState; //updating the reading of the button

}
