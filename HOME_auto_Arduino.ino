#define DEBUG true
#define trigPin 13//ultrasonic sensor
#define echoPin 12//ultrasonic sensor
#include <Wire.h>
#include <LiquidCrystal.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include "RTClib.h"
RTC_DS1307 RTC;

int ldrPin = A0;      // select the input pin for ldr
int ldrv = 0;         // analog value from ldr

int Contrast=1;       //contrast value for lcd

int automatic=0;    
boolean autol=false;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);//lcd conection settings

int prevs=0;

int c=0;
int sensorPin = A2 ,sensorValue = 0,scount=0;//Sound sensor                                                                                                                                                                                                                                                                                                                                                                                                                             ;
boolean lockLow = true;
boolean takeLowTime;  

int calibrationTime = 15;         // give some time to my lazy sensors to wake up ;)   

int pirPin = 25;                 //PIR pin for output reading 
long unsigned int lowIn;         //pir the time when the sensor outputs a low impulse
long unsigned int pause = 5000;  //pir the amount of milliseconds the sensor has to be low 

int tankOut=0;                   //output of water tank

bool light1=false,light2=false,fan=false;

int light_1=24,light_2=23,fan_=26;
char junk;//bluetooth
String inputString="";//bluetooth
//IR
int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;
//wifi
String ssid ="skyfall007_2.4";
String password="Masterkaki1";
String data;
String server = "www.homeiot.ml"; 
String uri = "/register.php";
byte dat [5];


    void setup()
    {    
      Serial.begin(115200);
      Serial1.begin(115200); //esp 
      reset();
      connectWifi();
      irrecv.enableIRIn();
      pinMode(light_1, OUTPUT);
      pinMode(22, OUTPUT);
      digitalWrite(22, HIGH);
      pinMode(light_2, OUTPUT);
      pinMode(pirPin, INPUT);
      digitalWrite(pirPin, LOW);
      Wire.begin();//RTC
      RTC.begin();//RTC
      lcd.begin(16, 2);//lcd
      lcd.setCursor(0,0);
      lcd.print("System loading");//lcd
      analogWrite(8,28836);//lcd
      lcd.setCursor(0,1);
      for(int i = 0; i < calibrationTime; i++){lcd.print(".");  delay(1000);}//  Serial.print(".");
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
      Serial.println(" done");
//RTC.adjust(DateTime(__DATE__, __TIME__));
 once1();


 }
     
    void loop()
    {   DateTime now1 = RTC.now();//time RTC 
    
        int tnk=0;//tank temp  
delay(100);
//bluetooth or any serial
char inChar;
 if(Serial.available()){      inChar = (char)Serial.read();    Serial.println(inChar);
          if(inChar == 'a'){ light1=true;  Serial.print("light by serial");}  
          if(inChar == 'b'){ light1=false; Serial.print("light by serial"); } 
          if(inChar == 'c'){ light2=true; Serial.print("light by serial");}  
          if(inChar == 'd'){ light2=false; Serial.print("light by serial");} 
         if(inChar == 'e'){ fan=true; Serial.print("fan by serial");}  
          if(inChar == 'f'){ fan=false; Serial.print("fan by serial");}  
  }
//IR TV remote
int temp=0;
if (irrecv.decode(&results)){    
  temp=results.value%10000; //used modulo to make it low
Serial.println(results.value); 
irrecv.resume(); } // Receive the next value
    
          if(temp == 2919){ light1=true; Serial.print("light by remote");}  
          if(temp ==2921){ light1=false;Serial.print("light by remote");  } 
          if(temp ==2920){ light2=true; Serial.print("light by remote");}  
          if(temp ==2912){ light2=false; Serial.print("light by remote");} 
          if(temp ==3023){ fan=true; Serial.print("fan by remote");}  
          if(temp ==3024){ fan=false; Serial.print("fan by remote");} 
  //motion detection 
  ldrv = analogRead(ldrPin);//Serial.println(ldrv);
if(ismotiond()){
  if(ldrv<100&&autol==false){
    automatic=20;
    autol=true;
    Serial.println("light start");
    light1=true;}
      c++; //c is counter for motion detection .used modulo to make it in 0-99 for lcd 
      c%=99;
      } 
//Lights
if(automatic==0&&autol==true) {   autol=false;      Serial.println("light off");    light1=false;}

if(light1)   digitalWrite(light_1, HIGH); 
else digitalWrite(light_1, LOW);

if(light2){ digitalWrite(light_2, HIGH);
   if(autol) {light1=false; automatic=0;}} 
                   else{digitalWrite(light_2, LOW);}  
  if(fan){digitalWrite(fan_, HIGH);}else{ digitalWrite(fan_, LOW);}
  //sound
 sensorValue = analogRead(sensorPin); //Serial.print("sound->");Serial.println(sensorValue);
 if(sensorValue>999){
  scount++;
  if(scount==100){scount=0;}
  }
 //water tank
   tnk=tank();  
   int t=now1.second();
   
    if(now1.minute()%2==0&&t<=5){
    tnk=tank();
    if(tnk>=2){  
    Serial.print(tnk);
   Serial.println("% water");
    tankOut=100-tnk;
    }
    }
    if(t!=prevs){
      prevs=t;// nothing complex it only update values on lcd every other second 
      lcd.clear();
     lcd.setCursor(0,0);
     lcd.print(now1.day());lcd.print("/");
      lcd.print(now1.month());lcd.print("/");
      lcd.print(now1.year()%100);lcd.print("");
      lcd.print(now1.hour());lcd.print(":");
      lcd.print(now1.minute());lcd.print(":");
      lcd.print(now1.second());
      lcd.setCursor(0,1);  
      lcd.print(tankOut);
      lcd.print("%w p");
      lcd.print(c);
      lcd.print(" S");
      lcd.print(scount);
      if(automatic>0){///time delay for automatic light decreases 
    automatic--;}
    }
    if(now1.minute()%5==0&&(t==5)){//update values on webserver in the interval of 5 sesonds 
      String date=String(now1.day())+"/"+String(now1.month())+"/"+String(now1.year());
      String time=String(now1.hour())+":"+String(now1.minute());
      data="tank=" + String(tankOut) + "&light1=" + String(light1) + "&light2=" + String(light2) + "&fan=" + String(fan) + "&date=" +date+"&time="+time;
      httppost();//send data
      data="";
    }

    }
     
 
    
    //function to calculate water level
    int tank(){
  int temp=0;
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;                                                                                                                                         
  //Serial.print("DISTANCE:");
  //Serial.println(distance);
  if (distance <= 200 && distance >0){//logic for totel %
   temp=((distance-20)*100)/90;// 90 is totel length of my tank
  }return temp;
}

//this function is used for startup of system to avoid delays and get values faster beacause some sensor may give 0/garbage values
void once1(){
  lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("done ;)");
  DateTime now1 = RTC.now();//time RTC 
int tnk=0;//tank temp 
for(int i=0;i<15;i++){
    tnk=tank();if(tnk!=0&&tnk!=1){  
    tankOut=100-tnk;
  delay(100);  
  }
}
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(now1.day());lcd.print("/");
      lcd.print(now1.month());lcd.print("/");
      lcd.print(now1.year()%100);lcd.print(" ");
      lcd.print(now1.hour());lcd.print(":");
      lcd.print(now1.minute());lcd.print(":");
      lcd.print(now1.second());
      lcd.setCursor(0,1);  
      lcd.print(tankOut);
      lcd.print("%w p");
      lcd.print(c);
      lcd.print(" S");
      lcd.print(scount);
  delay(100);
 
  }

//function for cheacking if there is any motion 
   
bool ismotiond(){
  if(digitalRead(pirPin)== HIGH){
    if(lockLow){  
        lockLow = false;  delay(50);
         }         
         takeLowTime = true;
       }
 if(digitalRead(pirPin) == LOW){       
        if(takeLowTime){
        lowIn = millis();          //save the time of the transition from high to LOW
        takeLowTime = false;       //make sure this is only done at the start of a LOW phase
        }
 
       if(!lockLow && millis() - lowIn > pause){  
         lockLow = true;   delay(50); 
         return true;
         //end
           }else {
            
            return false;
            }
       }
    }
//wifi
/*
httppost function builds a http packet and send it to webserver 

*/
void httppost () {

Serial1.println("AT+CIPSTART=\"TCP\",\"www.homeiot.ml\",80"); //AT+CIPSTART=\"TCP\",\"" + server + "\",80");//start a TCP connection.

if( Serial1.find("OK")) {

Serial.println("TCP connection ready");

} delay(1000);

String postRequest =

"POST " + uri + " HTTP/1.0\r\n" +

"Host:www.homeiot.ml\r\n" +

"Accept: *" + "/" + "*\r\n" +

"Content-Length: " + data.length() + "\r\n" +

"Content-Type: application/x-www-form-urlencoded\r\n" +

"\r\n" + data;

String sendCmd = "AT+CIPSEND=";//number of caracters to be sent.

Serial1.print(sendCmd);

Serial1.println(postRequest.length() );

delay(500);

if(Serial1.find(">")) { Serial.println("Sending.."); Serial1.print(postRequest);

if( Serial1.find("SEND OK")) { Serial.println("Packet sent");

while (Serial1.available()) {
String tmp = Serial1.readString();
Serial.println(tmp);
}
// close the connection
Serial1.println("AT+CIPCLOSE");
}

}}
// wifi reset function
void reset() {
Serial1.println("AT+RST");
delay(1000);
if(Serial1.find("OK") ) Serial.println("Module Reset");
}
// make a tcp connection
void connectWifi() {
Serial1.println("AT+CWMODE=1");
if(Serial1.find("OK") ) Serial.println("station mode");
String cmd = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";
Serial1.println(cmd);
Serial.println("delay4sec");
delay(4000);
if(Serial1.find("OK")||Serial1.find("IP")) {
Serial.println("Connected!");
}
else {
connectWifi();
Serial.println("Cannot connect to wifi"); }
}
