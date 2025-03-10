// Lynkurs-3-mWD-SD
// Kode for uttesting av program for måling og overføring av måledata til server
// Testoppstillingen innholder følgende:
//
// MKR NB 1500 m/GSM-antenne
// MKR GPS GPS-mottaker
// DS18B20 Temperatursensor for måling i vann
// SD-kort terminal for lagring av data
//
// Nils Kr. Rossing og Thor Inge Hansen 03.02.23


// Inkludering av biblioteker:
#include <DS18B20.h>        // Bibliotek for avlesning av temperatursensoren
#include <OneWire.h>        // Bibliotek som etablerer entrådsbuss til temp. sensoren
#include <Arduino_MKRGPS.h> // Bibliotek som leser av GPS
#include <MKRNB.h>          // Bibliotek som overførerdata til server via GPRS
#include <Arduino_MKRENV.h> // Bibliotek for lesing av miljøkortet ENV om det brukes
#include "ArduinoLowPower.h"// Bibliotek som legger MKR i dvale
#include <WDTZero.h>        // Bibliotek som håndterer vakthunden
#include <SD.h>             // Bibliotek for håndtering av SD-kort 
#include <SPI.h>            // Bibilotek for håndtering av serie-bus

// Deklarasjon av instanser:
WDTZero MyWatchDoggy;   // Deklarerer instansen til vakthunden MyWatchDoggy av typen WDTZero
NBClient client;        // Deklarerer instansen som opererer mot serveren
GPRS gprs;              // Deklarerer instansen som opprettet kontakt med GPRS-nettet
NB nbAccess;            // Deklarerer instansen som som gir tilgang til GPRS-nettet
DS18B20 ds(0);          // Deklarerer instansen som kommuniserer med temp. sensoren
File myFile;            // Deklarerer et objekt for filbehandling, kun ved bruk ved lagring på SD-kort

// Deklarasjon av konstanter:
#define DEBUG           // Kommenter bort denne dersom Serial.print ikke skal inkluderes
//#define LED           // Kommenter bort denne dersom LED indikasjon ikke ønskes

// Deklarasjon av variabler:
// Temperatursensor DS18B20
uint8_t address[] = {40, 250, 31, 218, 4, 0, 0, 52};
uint8_t selected;

// Oppkobling til server
char server[] = "sensor.marin.ntnu.no";  // Navnet på serveren der data legges
char path[] = "/cgi-bin/tof.cgi?";       // Angir cgi-kode for plassering i file
char filename[]="EVU-kurs23.txt";           // Navnet på fil for lagring av data
int port = 80;                           // Port 80 er default for HTTP
boolean connected = false;               // Status oppkobling til GPRS
char buffer[128];                        // Deklarsjon av buffer med tilhørende lengde

volatile unsigned long num = 0;    // Måling nummer
float w_temperature        = 0;    // Vanntemperatur
float BatVolt              = 0;    // Variabel for å holde batterispenningen
long Pause                 = 5000; // En ev. pause i programloopen
int SleepTime              = 5000; // Tid for dvale, for lengre tider enn 32 sek, sett verdien rett inn i argumentet

float GPSlat               = 0;    // GPS breddegrad
float GPSlong              = 0;    // GPS lengdegrad
unsigned long epochTime    = 0;    // GPS tidsstempel fra 1.1.1980
int numSat                 = 0;    // GPS antall satellitter
int maxNoChecks            = 10000; // Antall runder for lesing av GPS-data
int noChecks               = 0;    // Antall nødvendige sjekk av GPS    

// Deklarasjon av porter
int BatVoltPin = A3;             // Analog port for tilkobling av batterispenning
const int SD_CS_PIN = 4;         // Port for styring av SD-kort terminal, fabrikkbestemt

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Port for innebygget LED, definert som utgang                                           

  analogReadResolution(12);      // Angir oppløsning for AD-konverter

  #ifdef DEBUG 
     Serial.begin(115200);       // Setter opp data hastighet til monitor
  #endif
  
  if (!GPS.begin())              // Initialisering av GPS
  {
   #ifdef DEBUG 
     Serial.println("GPS initialisering mislyktes"); 
   #endif
  }

  SPI.begin();

   if(!SD.begin(SD_CS_PIN)) 
  {
    #ifdef DEBUG
    Serial.println("Initialisering SD terminal mislyktes");
    Serial.println("Har du satt i SD-kort?");
    #endif
  }

  // Lag en file med navnet DataFile.txt - Maks 8 karakterer + .txt
  myFile = SD.open("DataFile.txt", FILE_WRITE);
  delay(1000);
  myFile.close();
  delay(100);

  // Initialiserer dvale-funksjon
  LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, dummy, CHANGE);

  // Initialisering av "vakthund"
  MyWatchDoggy.attachShutdown(myshutdown); // Starter funksjonen myshutdown() ved reset
  //MyWatchDoggy.setup(WDT_SOFTCYCLE1M);     // Setter intervalltiden for "vakthunden"  

  // Opprett tilkobling til 4G - GPRS-tjeneste
  connectToGPRS();
  
  delay(1000);  
}

void loop() {
  //readGPSdata1();     // Inkluder GPS
  readWaterTemp();      // Inkluder måling av temp. i vann
  //readBatVolt();      // Inkluder måling av baterispenning om den er implementert
 
  makeString();         // Bygge opp bufferet for overføring av data
  //sdPrint();          // Skriv data til SD-terminal
  //connectToGPRS();    // Koble til GPRS-nettverket
  //connectToServer();  // Koble opp mot server og overfør data om den ikke alt er oppkoblet
  
  printData();
  printDataString();    // Skriv ut bufferet
  //MyWatchDoggy.clear(); // Resetter vakthunden
  
  num++;                // Målingens nummer fra start
  //GoToSleep();        // Legg microkontrolleren og GPS'en i dvale
  delay(Pause);         // Ev. legg inn en pause i loopen     
}

// Funksjonen leser av vanntemperaturen
void readWaterTemp()
{
  w_temperature = ds.getTempC();
}

// Funksjonen leser av batterispenningen
void readBatVolt(){
  BatVolt = analogRead(ADC_BATTERY)*(4.25/4096); //volts
}

// Funksjonen leser av GPS-data med bruk av en for-loop
void readGPSdata1()
{
  noChecks = 0;

  for (int i = 0; i < maxNoChecks; i++)
  {
    // Check if there is new GPS data available
    if (GPS.available())
    {
  
      // If there is, read GPS values
      GPSlat   = GPS.latitude();
      GPSlong  = GPS.longitude();
      epochTime  = GPS.getTime();
      numSat = GPS.satellites();
      
      break;
    }

    noChecks++;

    #ifdef DEBUG
      if (noChecks == maxNoChecks) Serial.println("Mislykket henting av posisjon fra GPS");
    #endif
      
  }
}

// Funksjonen bygger opp datastrengen for overføring
void makeString()
{  
   sprintf(buffer,"/cgi-bin/tof.cgi?%s,%u;%u;%.6f;%.6f;%i;%.1f;%.2f", filename, num, epochTime, GPSlat, GPSlong, numSat, w_temperature, BatVolt); 
}

// Funksjonen skriver ut bufferet til monitoren
void printDataString()
{
  #ifdef DEBUG
  Serial.println(buffer);
  Serial.println();
  #endif
}

// Funksjonen kobler opp til GPRS-nettverket (4G)
void connectToGPRS()
{
  while (!connected) 
  {
    if ((nbAccess.begin("", true, true) == NB_READY) && (gprs.attachGPRS() == GPRS_READY)) 
    {
      connected = true;
      
      #ifdef DEBUG 
        Serial.println("Vellykket tilkobling til GPRS"); 
      #endif
      
      #ifdef LED 
        flash(1); 
      #endif
    }  
    else
    {
      #ifdef DEBUG 
        Serial.println("Mislykket tilkobling til GPRS"); 
      #endif

      #ifdef LED 
        flash(5); 
      #endif
      
      break;
    }
  }
}

void sdPrint(){

  myFile = SD.open("DataFile.txt", FILE_WRITE);
  delay(1000);

  if (myFile) 
   {
    myFile.println(buffer);
    myFile.close();
   }
}

// Funksjonen kobler opp til server og skriver databuffer til serveren
void connectToServer()
{   
  if (client.connect(server, port))
  {  
    client.print("GET "); // Gjør et HTTP request:
    client.print(buffer);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    #ifdef DEBUG 
      Serial.print("Vellykket overføring til server av datasett nr.: "); 
      Serial.println(num);
    #endif
    
    #ifdef LED 
      flash(2); 
    #endif
  } 
  else
  {
    #ifdef DEBUG 
      Serial.print("Mislykket overføring til server av datasett nr.: "); 
      Serial.println(num); 
    #endif
     
    #ifdef LED 
      flash(10); 
    #endif
  }
}

// Funksjonen blinker et angitt antall ganger som en hjelp til debugging
void flash(int number)
{ 
  delay(1000);
  for(int i=0; i<number; i++)
  {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  }
 delay(1000);
}

// Funksjonen legger kretsen i dvale
void GoToSleep() // Legg mikrokontrolleren og GPS'en i dvale
 {
   #ifdef DEBUG 
     Serial.println("Går i dvale"); 
   #endif
         
   #ifdef LED 
     flash(3); 
   #endif
    
   GPS.standby();
   LowPower.deepSleep(SleepTime);
 }

// Returfunksjon etter endt dvale
void dummy()
{
   GPS.wakeup(); 
   NVIC_SystemReset(); // Resett programmet
}

void myshutdown()
{
  #ifdef DEBUG
    Serial.println("Resetter MKR");
  #endif
}

void printData(){
  #ifdef DEBUG
    Serial.print("GPS sjekk : "); Serial.println(noChecks);
    Serial.print("Breddegrad: "); Serial.println(GPSlat,6);
    Serial.print("Lengdegrad: "); Serial.println(GPSlong,6);
    Serial.print("Epoketid  : "); Serial.println(epochTime);
    Serial.print("Antall sat: "); Serial.println(numSat);
    Serial.print("Temp. vann: "); Serial.println(w_temperature);
    Serial.print("Batterisp.: "); Serial.println(BatVolt);
    Serial.print("Måling nr.: "); Serial.println(num);
    Serial.println();
  #endif
}
