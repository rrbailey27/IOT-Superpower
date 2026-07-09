#include <WiFi.h> // include the wifi library
#include "DHT20.h"  //we need a special ESP library for the DHT sensor

DHT20 dht;  //establishing an object named dht using the DHT20 library

// the DHT20 uses I2C, we want to use these two pins for I2C
int sda_pin = 6; // GPIO6 as I2C SDA
int scl_pin = 7; // GPIO7 as I2C SCL

const int buttonPin = 5; // the button is connected to GPIO5
int buttonState = 0; // defining and initializing buttonState as low 

/*a variable we need for for event-driven programming 
we only want button actions to be triggered upon button push, not the entire
time while the button IS pushed... */
int lastbuttonState = 0; 

//declaring variables to store humidity and temperature
float h;
float tc;
float tf;

// WiFi credentials - enter network name and password here
// For UVA wahoo network, see comments on each line
const char *ssid = "*****"; // "wahoo"
const char *pwd = "*****"; // ""


/* update the hostDomain to be the IP address of your AWS server
leave the hostPort at 80 – this is the port used for http
hostDir is the directory and file we are going to call in the HTTP request
leave that line as is; we will need this directory (esp32_dht20_superpower) 
and a file named esp32_add2database.php on our server
*/

const char * hostDomain = "##.##.##.##";
const int hostPort = 80;
const char * hostDir = "/rodman/esp32_dht20_superpower/esp32_add2database.php";


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

// the printline function just prints dashes until something completes!
void printLine()
{
  Serial.println();
  for (int i=0; i<30; i++)
    Serial.print("-");
  Serial.println();
}

void senddata(const char * host, uint8_t port, const char * directory, float tempc, float tempf, float humidity)
/* senddata has 6 parameters – 1)the host server where we are send data
2) the port is always 80
3) the directory is the directory on the host where the php script is that adds a database entry
4-6) and the three datafields we are sending for this example
the variable names in the void senddata() line are the names used within this function
these names do not need to match the names used outside of this function
*/
{

// OUR FIRST order of business is connecting to the server
  printLine(); 
// printline is just a function that prints a line of dots until we connect

  Serial.println("Connecting to domain: " + String(host));

  // Use WiFiClient class to connect to the server
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    return;
  }
  Serial.println("Connected!");
  printLine();

// OUR SECOND order of business is to form an HTTP GET request

/* the below uses an HTTP get request to make an entry into the database
the php file receives the data "event" which is passed to 
that file using the "?<variablename>=<value>" format following
the name of the php file.  IF we were sending TempC of 0, 
Temp F of 32, and humidity of 50, the entire string printed to the server is
 GET /rodman/esp32_dht20_superpower/esp32_add2database.php?temp_php_C=0&temp_php_F=32$humidity_php=50
*/
client.print("GET " + String(directory));
client.print("?temp_php_C=");
client.print(String(tempc));
client.print("&temp_php_F=");
client.print(String(tempf));
client.print("&humidity_php=");
client.print(String(humidity));

// our THIRD step is to do some formal stuff related to the HTTP request
/*we include info about the type of protocol to use (HTTP 1.1)
the host name (Which is the server IP address)
and then close the connection to the host and stop the connection
collectively this is like typing the following into your browser
where 00.00.00.00 would be replaced with your server’s IP address
http://00.00.00.00/rodman/esp32SF_helloworld/esp32_add2database.php?event=fromscript
*/

  client.println(" HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Connection: close");
  client.println();
  client.println();
  client.stop();

    // The server will reply after it receives the http GET request  
    // Here, we read all the lines of the reply from server and print them to Serial
  while (client.available()) 
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

    // Close the wifi connection

  Serial.println();
  Serial.println("Closing connection");
  client.stop();

  // Final bracket to end the senddata function
}


void setup() {
  pinMode(buttonPin, INPUT); // typical Arduino syntax
  Serial.begin(115200); //starting up the serial monitor at 9600 baud rate

/* following two lines are necessary to set up I2C
communication with the DHT20 */
  Wire.setPins(sda_pin, scl_pin); // Set the I2C pins before begin
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

buttonState=digitalRead(buttonPin);  // read the button, HiGH means the button is pushed

// if the button is pushed and the prior state was that the button was unpushed, do the loop
if (buttonState == HIGH && buttonState!=lastbuttonState) 
  {
    //read the data from the DHT20
    int status = dht.read();

    if (status == DHT20_OK) {  // valid DHT20 read
        
      // Read the humidity
      h = dht.getHumidity();
    
      // Read temperature as Celsius
      tc = dht.getTemperature();
    
      // Convert Celsius to Fahrenheit 
      tf = tc*9/5+32;
        
      // print it all out to the serial monitor
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("%  Temperature: "));
      Serial.print(tc);
      Serial.print(F("°C, "));
      Serial.print(tf);
      Serial.println(F("°F"));

    } else {  // if read was not successful
        Serial.println("Invalid read");
    }


    senddata(hostDomain, hostPort, hostDir, tc, tf, h);


    delay(1000);  //this delay helps "debounce" the button
  } //end the button press loop

lastbuttonState=buttonState; //updating the reading of the button

} //end the void loop
