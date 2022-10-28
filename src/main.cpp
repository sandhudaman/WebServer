// I, Damanpreet Singh, student number 000741359, certify that all code submitted is my own work;
// that I have not copied it from any other source.
// I also certify that I have not allowed my work to be copied by others.

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// access credentials for WiFi network.
const char *ssid = "####";
const char *password = "########";

// WiFI server.  Listen on port 80, which is the default TCP port for HTTP
WiFiServer server(80);

// Pin that the  DS18B20 is connected to
const int oneWireBus = D3;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature DS18B20(&oneWire);

String timeSinceEpoch(){

  unsigned long getMillis = millis();
  unsigned long seconds = getMillis / 1000; // 1000 milliseconds in a second
  unsigned long minutes = seconds / 60; // 60 seconds in a minute
  unsigned long hours = minutes / 60; // 60 minutes in an hour

  getMillis %= 1000; // get the remainder of milliseconds
  seconds %= 60; // get the remainder of seconds
  minutes %= 60;  // get the remainder of minutes
  hours %= 24;    // get the remainder of hours

  return String(hours) + ":" + String(minutes) + ":" + String(seconds);

}

void setup()
{
  Serial.begin(115200);
  DS18B20.begin();
  Serial.println("\nWeb Server Demo");

  Serial.printf("\nConnecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n",
                WiFi.localIP().toString().c_str());

}

void loop()
{
  WiFiClient client = server.available();

  float fTemp;
  // ask DS18B20 for the current temperature
  DS18B20.requestTemperatures();

  // fetch the temperature.  We only have 1 sensor, so the index is 0.
  fTemp = DS18B20.getTempCByIndex(0);

  // wait for a client (web browser) to connect
  if (client)
  {
    
    Serial.println("\n>> Client connected");
    Serial.println(">> Client request:");
    while (client.connected())
    {
      // read line by line what the client (web browser) is requesting
      if (client.available())
      {
        // print information about the request (browser, OS, etc)
        String line = client.readStringUntil('\r');
        Serial.print(line);
        // wait for end of client's request
        if (line.length() == 1 && line[0] == '\n')
        {
          // send HTTP response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // Display the HTML web page
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"<meta charset=\"UTF-8\"> ");
          client.println("<link rel=\"icon\" href=\"data:,\">");
          client.println("</style></head><body><h1>Damanpreet Singh 000741359</h1>");
          client.println("<table><tr><th></th><th></th></tr>");
          client.println("<tr><td><Strong>Temperature                             :</Strong></td><td>");
          client.println(String(fTemp));
          client.println(" °C</td></tr>");
          client.println("<tr><td><Strong>IP Address of IOT Device                :</Strong></td><td>");
          client.println(WiFi.localIP().toString());
          client.println(" </td></tr>");
          client.println("<tr><td><Strong>WeMos D1 Mini has been running since    :</Strong></td><td>");
          client.println(timeSinceEpoch());
          client.println(" </td></tr>");
          client.println("</table>");
          client.println("</body></html>");
          Serial.println(">> Response sent to client"); 
          break;
        }
      }
    }

    // keep read client request data (if any).  After that, we can terminate
    // our client connection
    while (client.available())
    {
      client.read();
    }

    // close the connection:
    client.stop();
    Serial.println(">> Client disconnected");
  }
}

