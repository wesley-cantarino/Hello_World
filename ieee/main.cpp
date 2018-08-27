#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Arduino.h>

#include <SPI.h>
#include <MFRC522.h>
#define RST_PIN    5
#define SS_PIN     15
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance

const char* ssid = "h'(x)";
const char* password = "T5e5L0e9C7o7M0u2N7i4C4a0C6o4E0s";

ESP8266WebServer server(80);

#define RED_LED D4
#define GRE_LED D3
#define BUZZER  D2
const int button = A0;

String name_tag = "";

void handleRoot() {
  String textoHTML;

  //textoHTML += "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n";
  //textoHTML += "<html lang=\"en\">";
  textoHTML += "<!DOCTYPE HTML>";
  textoHTML += "<html>";
    textoHTML += "<head>";
      textoHTML += " <meta http-equiv='refresh' content='3'>";
      textoHTML += "<title>ESP8266 Server</title>";
    textoHTML += "</head>";
    textoHTML += "<body>";
      textoHTML += "<center>";
      textoHTML += "<h1>TAG</h1>";
      textoHTML += name_tag;
    textoHTML += "</body>";
  textoHTML += "</html>";
   
  server.send(200, "text/html", textoHTML);
}

void handleNotFound(){
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

void efeito (){
  digitalWrite(GRE_LED, HIGH);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BUZZER, HIGH);

  delay(800);
  digitalWrite(BUZZER, LOW);

  delay(500);

  digitalWrite(GRE_LED, LOW);
  digitalWrite(RED_LED, LOW);
  delay(500);
}

void function_RFID (){
  // Procura por cartao RFID
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;
  // Seleciona o cartao RFID
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;
  //Mostra UID na serial
  Serial.print("UID da tag :");
  String conteudo= "";
  for (byte i = 0; i < mfrc522.uid.size; i++){
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  conteudo.toUpperCase();

  if (conteudo.substring(1) == "01 D1 FC 52" || "03 1C A0 F6"){
    efeito();
    name_tag = conteudo.substring(1);
  }
}

void setup(void){
  pinMode(GRE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  SPI.begin();          // Inicia  SPI bus
  mfrc522.PCD_Init();   // Inicia MFRC522

  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
}

void loop(void){
  function_RFID();
  server.handleClient();
}