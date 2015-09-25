#include <SPI.h>
#include <Ethernet.h>
#include <FreqMeasure.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(172, 22, 74, 93); // this is not used

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80); // no we get ip from DCHP

double m=0;
double sum=0;
int count=0;
float frequency=0;

void setup() {
  pinMode (2, INPUT);
  pinMode (4, INPUT);
  Serial.begin(57600);
  FreqMeasure.begin();

  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("http://");
  Serial.println(Ethernet.localIP()); // print the ip to console
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");

          Serial.print("Time: ");  
          Serial.println(millis());

          while (client.connected()) {
            if (FreqMeasure.available()) {
              sum = sum + FreqMeasure.read();
              count = count + 1;
            }
            if( ( millis()-m ) > 1000 ) {
              frequency = FreqMeasure.countToFrequency(sum / count);
              sum = 0;
              count = 0;
            }
            client.print(millis());
            client.print(",");
            client.print(frequency);
            client.print(",");
            client.print(analogRead(0));
            client.print(",");
            client.print(analogRead(1));
            client.print(",");
            client.println(analogRead(2));
          }
          client.println("<br />");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}
