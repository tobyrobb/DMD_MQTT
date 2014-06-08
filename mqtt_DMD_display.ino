/*

08 jun 2014

some code lifted from the web
 
Toby Robb

This is a test sketch for the Arduino running the DMD led display 
through the web and ethernet.

Write to the display via an MQTT server.

Publish message to the topic "DMD"

the dmd display connects to D9,10,11,12,13 and gnd

as per its subboard
 
there is also the ability to send commands to the UNO


NOTES:

Install yourself an MQTT server such as mosquitto for linux or pc

Plug in your Arduino DMD and ethernet combo

Publish messages on your MQTT server to the DMD topic


*/

// Includes

#include <Ethernet.h>
#include <PubSubClient.h>
#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //  The Freetronics driver library
#include <TimerOne.h>   //
#include "SystemFont5x7.h"

// Defines
#define mqttTopic "DMD"  // choose the MQTT topic the DMD will listen to for messages

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
 
// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBE, 0xAF, 0xFE, 0x02 };
byte server[] = { 192,168,2,10 };  // This the address 

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

char message_buff[100];
String message;

void setup(){
  
// Setup the serial

  Serial.begin(9600); //Just for debugging
  Serial.println("Beginning Setup"); //Just for debugging

if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
  }
  if (client.connect("arduinoClient")) {
    client.subscribe(mqttTopic);
  }
  Serial.print("server is at "); //Just for debugging
  Serial.println(Ethernet.localIP());//Just for debugging

 // setup the DMD required code
 //initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 5000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
   dmd.selectFont(SystemFont5x7);  
   
   delay(2000);   // Wait 2 seconds
   
   message = "Waiting for message...";  //The first message
   showMessage();   // Shows the message string on the DMD

   delay(2000);   // Wait 2 seconds

   dmd.clearScreen( true );

}

void loop(){
  
 client.loop();  //Waits for messages then calls callback function when one arrives
 
 Serial.println(message);  //Just for debugging
 
 showMessage();   // Shows the message string on the DMD
 
 delay(1000);    //Wait for one second
 
 dmd.clearScreen( true );

}

// all the functions begin here

 
void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
 String msgString = String(message_buff);
  
  Serial.println(msgString);
  message = msgString;
  
  checkCommand(msgString);   // lets check if there was a command issued via message

}

void ScanDMD()
{ 
  dmd.scanDisplayBySPI();
}
 

void showMessage(){
  
  char marqueeString[256];
    int myTextLength = message.length();
    message.toCharArray(marqueeString, myTextLength+1);
    
    dmd.drawMarquee(marqueeString,myTextLength,(32*DISPLAYS_ACROSS)-1,5);
    long start=millis();
   long timer=start;
   boolean ret=false;
   while(!ret){
     if ((timer+30) < millis()) {
       ret=dmd.stepMarquee(-1,0);
       timer=millis();
     }
     
   }
}

void checkCommand(String inString){

  // some examples of commands you could run
    
    if (inString.equals("freetronics")){
   Serial.println("www.freetronics.com for all your Adruino needs !");   //Just for debugging
   message = "www.freetronics.com for all your Adruino needs !";
  
  }
  
  
   if (inString.equals("arduino")){
  Serial.println("Programmed using Arduino V1.5 and a Freetronics DMD LED Display");   //Just for debugging
   message = "Programmed using Arduino V1.5 and a Freetronics DMD LED Display";
       }
 
   if (inString.equals("relayOn")){
  Serial.println("Turning on the relay");   //Just for debugging
   message = "Turning on the relay";
   //digitalWrite(relayPin, HIGH);
       }
       
 }
