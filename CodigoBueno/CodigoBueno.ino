#include <SoftwareSerial.h>
//#include "DHT.h"
#define RX 2
#define TX 3
//#define dht_apin 11 // Analog Pin sensor is connected to
//DHT dhtObject;
String AP = "";       // AP NAME
String PASS = ""; // AP PASSWORD
String API = "R1BKO7ZAALCOZI45";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
  
SoftwareSerial esp8266(RX,TX); 

//Variables para el sensor de temperatura
float suma = 0;
int sensorTemp = A0;
float valorTemp = 0;

//Activador del ventilador en base a temperatura
int motorPin = 9;

//Variables para el sensor de luz
int sensorLuz = A1;
int valorLuz = 0;

//Activador del LED en base a la luminosidad
int ledPin = 10;

void setup() {
  //Estas son las entradas en el Arduino
  pinMode(sensorTemp, INPUT);
  pinMode(sensorLuz, INPUT);
  
  //Estas son las salidas del arduino
  pinMode(motorPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(9600);
  esp8266.begin(115200);
  //dhtObject.setup(11);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  
 String getData = "GET /update?api_key="+ API +"&field1="+getTemperatureValue()+"&field2="+getHumidityValue();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);
 delay(1500);
 countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}


String getTemperatureValue(){
  for(int i=0; i<5; i++){
    sensorTemp = analogRead(A0);
    valorTemp = ((sensorTemp*5000.0)/1023)/10;
  }
  suma = valorTemp/2.0;
  Serial.print("Temperatura en C: ");
  Serial.println(suma);

  if(suma > 30){
    digitalWrite(motorPin,HIGH);
  }
  else{
  	digitalWrite(motorPin,LOW);
  }
  
   return String(suma); 
  
}


String getHumidityValue(){
  //Sensor de luz (fotoresistor)
  valorLuz = analogRead(sensorLuz);
  Serial.println("Valor del fotoresistor:");
  Serial.println(valorLuz);
  
  //Activador del LED que indica que hay demasiada LUZ una vez
  //que pase el umbral de luminosidad especificado
  if(valorLuz > 950){
    digitalWrite(ledPin,HIGH);
  }
  else{
  	digitalWrite(ledPin,LOW);
  }
   return String(valorLuz); 
  
}

void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
