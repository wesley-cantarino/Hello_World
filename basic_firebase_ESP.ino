/***************************/
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
/***************************/

/***************************/
char ssid [] = "CAS_IOT";
char password [] = "CAS_IOT_2018_WESLEY";

char FIREBASE_HOST [] = "cas-test-14fcd.firebaseio.com";
char FIREBASE_AUTH [] = "";
/***************************/

/***************************/
ESP8266WebServer server(80);
/***************************/
void handleRoot (){
  String textHTML;

  textHTML += "<!DOCTYPE HTML>";
  textHTML += "<html>";
    textHTML += "<head>";
      textHTML += " <meta http-equiv='refresh' content='3'>";
      textHTML += "<title>ESP8266 Server</title>";
      textHTML += "<style>";
        textHTML += "body{background-color: rgb(235, 235, 245, 100);}";
      textHTML += "</style>";
    textHTML += "</head>";

    textHTML += "<body>";
      textHTML += "<center>";
      textHTML += "<h1>HELLO, WORLD</h1>";
      textHTML += "<h3>";
        textHTML += millis();
      textHTML += "</h3>";
    textHTML += "</body>";
  textHTML += "</html>";

  server.send(200, "text/html", textHTML);
}

void handleNotFound (){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
/***************************/

void setup (){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  if (WiFi.status() != WL_CONNECTED) { //original era while mas if tbm funciona
    delay(200);
    Serial.print(".");
  }

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop (){
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);

  server.handleClient();
  Firebase.pushString(":D", String(millis()));
}
