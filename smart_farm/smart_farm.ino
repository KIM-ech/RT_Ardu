#include<SoftwareSerial.h>
SoftwareSerial bt(2,3);
#define water A0
#include "DHT.h"
#include<Servo.h>
#define DHTPIN 10  
#define led 8
#define fan 9
#define ir 12
#define serv 11
Servo servo;
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);
#include <Wire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  bt.begin(9600);
  pinMode(water,OUTPUT);
  pinMode(ir,INPUT);
  digitalWrite(water,HIGH);
   pinMode(led,OUTPUT);
  digitalWrite(led,HIGH);
  pinMode(fan,OUTPUT);
  digitalWrite(fan,HIGH);
  dht.begin();  
  mlx.begin(); 
  servo.attach(serv); 
}
char a='x';
bool manual_mode=false;
bool water_on=false;
bool led_on=false;
bool fan_on=false;
bool door_on=false;
int max_temp=28;
int max_brightness=500;
int min_earth_humid=10;
int brightness=0;
int earth_humid=0;
int pir=0;
int obj_temp=30;
int max_obj_temp=32;
void loop() {
  brightness=analogRead(A2);
  earth_humid=analogRead(A3);
  pir=digitalRead(ir);
  obj_temp=mlx.readObjectTempC();
  Serial.println(obj_temp);
  if(pir==1 and obj_temp>max_obj_temp)tone(6, 322, 150);
  if(manual_mode==false){ //자동모드일경우
    
     float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.println(t);
  String msg=String(t)+","+String(h)+"\n";
  bt.write(msg.c_str());
  if (max_temp<t){
    //설정한온도보다 높아지면 팬을켠다.  
     digitalWrite(fan,LOW);
     fan_on=true;
  }
  else{
     digitalWrite(fan,HIGH);  
      fan_on=false;
  }
  if(brightness>max_brightness){ //어두워지면 
      digitalWrite(led,LOW);
      led_on=true;
  }
  else{  led_on=false;
      digitalWrite(led,HIGH);
  }
  if(min_earth_humid>earth_humid){
     digitalWrite(water,LOW);
       water_on=true;
  }
  else{
      water_on=false;
     digitalWrite(water,HIGH);
  }
  Serial.print(brightness);
  Serial.print(" / ");
  Serial.print(pir);
  Serial.print(" / ");
  Serial.println(earth_humid);
  
  
  }
  if(bt.available()){
    a=bt.read();
    if(a=='a'){
      if(manual_mode==false)manual_mode=true;
      else manual_mode=false;  
    }
    if(manual_mode==true){
        if(a=='w'){
          if(water_on==false){
            water_on=true;
            Serial.println("water pump on");
            digitalWrite(water,LOW);
          }
          else{
            Serial.println("water pump off");
            water_on=false;  
            digitalWrite(water,HIGH);
          }
        }
        else if(a=='l'){
           if(led_on==false){
            led_on=true;
            digitalWrite(led,LOW);
          }
          else{
            led_on=false;  
            digitalWrite(led,HIGH);
          }
        }
        else if(a=='f'){
           if(fan_on==false){
            fan_on=true;
            digitalWrite(fan,LOW);
          }
          else{
            fan_on=false;  
            digitalWrite(fan,HIGH);
          }
        }
         else if(a=='o'){
           if(door_on==false){
            door_on=true;
            servo.write(60);
          }
          else{
            door_on=false;  
            servo.write(0);
          }
        }
    }
  }
  delay(1000);
}
