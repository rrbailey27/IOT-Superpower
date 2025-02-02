#include <WiFi.h> // include the wifi library

void setup() {
  Serial.begin(115200); //starting up the serial monitor at 9600 baud rate
  WiFi.mode(WIFI_STA); // Set WiFi mode to station
}

void loop() {
  Serial.print("Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  delay(5000);
}
