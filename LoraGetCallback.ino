
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

const char* ssid = "HUAWEI P9";
const char* password = "1278612786";

//Your Domain name with URL path or IP address with path
String serverName = "http://onesmartaccess.ddns.net:1880/update-sensor";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;


void setup() {
  // put your setup code here, to run once:
  //initialize Serial Monitor
  
  while (!Serial);
 Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());  
  Serial.println("LoRa Receiver");
  //SPI.begin(SCK, MISO, MOSI, CS);
  SPI.begin(18, 19, 23, 5);
  //setup LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  //LoRa.begin(923E6);
  //replace the LoRa.begin(---E-) argument with your location's frequency 
  //433E6 for Asia
  //866E6 for Europe
  //915E6 for North America
  while (!LoRa.begin(923E6)) {
    Serial.println(".");
    delay(500);
  }
   // Change sync word (0xF3) to match the receiver
  // The sync word assures you don't get LoRa messages from other LoRa transceivers
  // ranges from 0-0xFF
  //LoRa.setSyncWord(0xF3);
  LoRa.onReceive(onReceive);
  LoRa.enableCrc();
  Serial.println("LoRa Initializing OK!");
  LoRa.receive();
}
bool flag=0;
String LoRaData;
String RSSI_;
void loop() {
  // put your main code here, to run repeatedly:
    if (flag==1){
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;   
      http.begin(serverName);
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=PET1_key&"+LoRaData+"&RSSI="+RSSI_;           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    flag=0;
    }
}



void onReceive(int packetSize) {
    Serial.print("Received packet ' :");
    // read packet
    while (LoRa.available()) {
    LoRaData = LoRa.readString();
    Serial.print(LoRaData); 
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
    RSSI_=String(LoRa.packetRssi());
    }
    flag=1;
}
