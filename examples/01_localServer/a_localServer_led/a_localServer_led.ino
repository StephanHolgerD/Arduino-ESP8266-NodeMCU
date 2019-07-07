/*
 *  LED - local Server - simple example
 *  ------------------------------------
 *
 * LED controlled by local Webpage (served by the ESP-Module it self)
 *
 * for NodeMCU ESP8266 | autumn 2018 | tomek n. (https://cirg.io) for retune festival
 * based on the example by https://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/
 *
 * (tested with Arduino IDE 1.6.5)
 *
 */

#include <ESP8266WiFi.h>


int ledPin = LED_BUILTIN; // GPIO16 --> can also declared 16 or D0 

const char* ssid = "cirg";  // name of your network
const char* password = "workshop";  // pw for your network

//your local server running on the ESP-Module
WiFiServer server(80);

void setup() {
  
  // start serial connection // for debuging 
  Serial.begin(115200);
  delay(10);

  // setup the LED
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);  // wiring for the onboard LED at the NodeMCU is inverted

  // start printing some infos to the serial console // for debuging 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // connect to the local wifi
  WiFi.mode(WIFI_STA);  // set wifi mode to Station
  WiFi.begin(ssid, password);

  // wait while there is no wifi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // just print some thing to the serial console to see you are still alive
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address, so we know where to connect to
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());  // the current IP adress of the ESP-Module 
  Serial.println("/");

}

void loop() {

  // Check if a client (browser) has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  /////// get the clients data

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();


  ////// Match the request
  boolean value = LOW;  

  if (request.indexOf("/LED=ON") != -1)  {
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    value = LOW;
  }

  // Set ledPin according to the request
  boolean nodeMCU_ledValue = !value; // invert the value, because of the LED wiring at the NodeMCU board
  digitalWrite(ledPin, nodeMCU_ledValue);


  //// Return html response to the client (browser)
  
  // note: if you want to create your own html response, but this return/arduino syntax is just such a pain to write
  // you can use the processing (p5 / processing.org) hmtl parser to transfer a .html file into arduino usabble code.
  // check out the »x_p5_htmlParsing« folder
     
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one

  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.print("Led pin is now: ");

  if (value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}

