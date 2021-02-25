#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <LoRa.h>

#define TX 5//d3//3
#define RX 4//d4//4
#define interruptPin 3//23
void blink();
bool state = 0;
bool fl=0;
SoftwareSerial ss(RX, TX);
//TinyGPSPlus gps;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Start");
    pinMode(interruptPin, INPUT_PULLUP);

   // Serial.begin(9600);
  while (!Serial);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);

  Serial.println("LoRa Sender");

  if (!LoRa.begin(923E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //LoRa.setSyncWord(0xF3);
  LoRa.setTxPower(20);  
  ss.begin(9600);

}
typedef struct
{
  char hhhmmss[20];
  char A[20];
  char llll[20];
  char a[20];
  char yyyy[20];
  char a2[20];
  char xx1[20];
  char xx2[20];
  char ddmmyy[20];
  char xx3[20];
  char ahh[20];
  char EW[20];
  char checksum[20];
} gps_data;


void data2str(char data[], gps_data *gps, int gps_length);
void loop() {
  char data[200];
  String buf_msg;
  String result;
  gps_data gps={0x0};

  while (ss.available() > 0)
  {
    if (ss.find("$GPRMC,")) 
    {
       result = ss.readStringUntil('\n');
      result.toCharArray(data, result.length());
      data2str(data, &gps,result.length());
      LoRa.beginPacket();
      LoRa.print("A=\"");
      LoRa.print(gps.A);
      char lati[11];
      char longi[11];
     dtostrf((atoi(gps.llll)/100)*1.0+((((long) (atof(gps.llll)*100000)%10000000)/6000000.0)),3,6,lati);
    
    // dtostrf(((((long) (atof(gps.llll)*100000)))),3,6,lati);
     dtostrf((atoi(gps.yyyy)/100)*1.0+((((long) (atof(gps.yyyy)*100000)%10000000)/6000000.0)),3,6,longi);

     //dtostrf((atoi(gps.yyyy)/100)*1.0+((((long) (atof(gps.yyyy)*100000)%1000000)/6000000.0)),3,6,longi);
      LoRa.print("\"&l=");
      LoRa.print(lati);
      LoRa.print("&y=");
      LoRa.print(longi);
      LoRa.endPacket();


    }
  }

  delay(1000);
}

void data2str(char data[], gps_data *gps, int gps_length)
{
  int i = 0;
  int start_ = 0;
  Serial.println(data);

  for (; data[i] != ','; i++)
  {
    gps->hhhmmss[i - start_] = data[i];
  }
  gps->hhhmmss[i - start_] = '\0';
  start_ = i + 1;
  for (i = i + 1; data[i] != ',' && i <  start_ + 20; i++)
  {
    gps->A[i - start_] = data[i];
    // Serial.print("%d %c\n",i, data[i]);
    // Serial.print("%d %c\n",i, gps->A[i - start_]);

  }
  gps->A[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_ + 20; i++)
  {
    gps->llll[i - start_] = data[i];
  }
  gps->llll[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i <  start_ + 20; i++)
  {
    gps->a[i - start_] = data[i];
  }
  gps->a[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_ + 20; i++)
  {
    gps->yyyy[i - start_] = data[i];
  }
  gps->yyyy[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_  + 20; i++)
  {
    gps->a2[i - start_] = data[i];
  }
  gps->a2[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_  + 20; i++)
  {
    gps->xx1[i - start_] = data[i];
  }
  gps->xx1[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_ + 20; i++)
  {
    gps->xx2[i - start_] = data[i];
  }
  gps->xx2[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_  + 20; i++)
  {
    gps->ddmmyy[i - start_] = data[i];
  }
  gps->ddmmyy[i - start_] = '\0';
  start_ = i + 1 ;

  for (i = i + 1; data[i] != ',' && i < start_ + 20; i++)
  {
    gps->xx3[i - start_] = data[i];
  }
  gps->xx3[i - start_] = '\0';
  
  start_ = i + 1 ;
  for (i = i + 1; data[i] != ',' && i < start_ + 20; i++)
  {
    gps->ahh[i - start_] = data[i];
  }
  gps->ahh[i - start_] = '\0';
  
   start_ = i + 1 ;
  for (i = i + 1;  i < start_ + 1 &&i<gps_length-1; i++)
  {
    gps->EW[i - start_] = data[i];
  }
  gps->EW[i - start_] = '\0';
  start_ = i + 1 ;
  for (i = i + 1;   i < start_ + 2 &&i<gps_length-1; i++)
  {
    gps->checksum[i - start_] = data[i];
  }
  gps->checksum[i - start_] = '\0';
}


void blink() {
if(fl==1){
  state = !state;
  Serial.println(state);
    uint8_t index;
  String L80_GPSStandby = "$PMTK255,2,3000,12000,18000,72000*15<CR><LF>"; 

  for (index = 0; index < L80_GPSStandby.length(); index++)
  {
    ss.write(L80_GPSStandby[index]);
    Serial.print(L80_GPSStandby[index]);
    Serial.print(" ");
  }
      Serial.print("\n");
}
fl=1;
}
