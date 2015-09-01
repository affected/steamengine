#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

int mPin = A0;   // select the input pin for the sound sensor (microphone)
int vPin = A1;   // select the input pin for the vibration sensor
int mValue = 0;  // variable to store the analogic value coming from the sound sensor
int vValue = 0;  // variable to store the analogic value coming from the vibration sensor
int mVal = 0;    // define numeric variables val
int vVal = 0;    // define numeric variables val
int mmin = 2000; // sound min
int vmin = 2000; // vibration min
int mmax = 0;    // sound max
void setup() {
  pinMode (2, INPUT);
  pinMode (4, INPUT);
  Serial.begin(9600);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
/*
  mVal = digitalRead(4);
  vVal = digitalRead(2);
  mValue = analogRead(mPin);
  if( mValue > mmax ) { mmax = mValue; }
  if( mValue < mmin ) { mmin = mValue; }
  vValue = analogRead(vPin);
  if( vValue < vmin ) { vmin = vValue; }
  Serial.println( "Microphone: " + String(mVal) + " " + String(mmin) + "<" + String(mValue) +  "<" + String(mmax) + " Vibration " + String(vVal) + " "  + String(vmin) + "<" + String(vValue ) );
*/

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
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
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
