#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <SoftwareSerial.h>
#include<Wire.h>
#include<LiquidCrystal.h>
#include <LiquidCrystal_I2C.h> 
#include "DHT.h"       

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883 
#define AIO_USERNAME    "username"
#define AIO_KEY         "key"
#define DHTTYPE DHT11   
#define dht_dpin 0

LiquidCrystal_I2C lcd(0x3F,16,2);
DHT dht(dht_dpin, DHTTYPE);
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish Temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temperature");
Adafruit_MQTT_Publish Humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish Soil_Moisture= Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Soil Moisture");
Adafruit_MQTT_Publish Motor_Condition = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Motor Condition");

const char* ssid     = "name";     
const char* password = "password"; 
int v=0;

const int soil_sensor_pin = A0;


void setup()
{ 
  Serial.begin(9600);  
  lcd.begin();
  lcd.clear();
  dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting"); 
    }
  connect();
}

void connect()
{
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) 
  {
    switch (ret) 
    {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }
    if(ret >= 0)
      mqtt.disconnect();
    Serial.println(F("Retrying connection..."));
    delay(1000);
  }
  
  Serial.println(F("Adafruit IO Connected!"));
  }

void loop() 
{   
   if(! mqtt.ping(3))
     {
     if(! mqtt.connected())
     connect();} 
     float moisture_percentage; 
     moisture_percentage = ( 100.00 - ( (analogRead(soil_sensor_pin)/1023.00) * 100.00 ) );
     Serial.print("Soil Moisture(in Percentage) = ");
     Serial.print(moisture_percentage);
     Serial.println("%");
    float h = dht.readHumidity();
    float t = dht.readTemperature();         
    if(moisture_percentage>100)
    {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("R.H.");
    lcd.print(" :");
    lcd.print(h);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("TEMP");
    lcd.write((byte)0);
    lcd.print(" :");
    lcd.print(t);
    lcd.print((char)223);
    lcd.print("C");
    delay(2000);
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Soil Moisture");
    lcd.print(" :");
    lcd.print(moisture_percentage);
    lcd.print("%");
    delay(2000);
    }
    else if(moisture_percentage>=0 && moisture_percentage<=100)
    {
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("R.H.");
    lcd.print(" :");
    lcd.print(h);
    lcd.print("%");
    lcd.setCursor(0,1);
    lcd.print("TEMP");
    lcd.write((byte)0);
    lcd.print(" :");
    lcd.print(t);
    lcd.print((char)223);
    lcd.print("C");
    delay(2000);
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Soil Moisture");
    lcd.print(":");
    lcd.print(moisture_percentage);
    lcd.print("%");
    delay(2000);
    }

    if(moisture_percentage >= 0 && moisture_percentage <= 30)
    {
      digitalWrite(2, HIGH);
      v=1;
    }
    else if(moisture_percentage>=30 && moisture_percentage<= 100)
    {
      digitalWrite(2, LOW);
      v=0;
    }
    if (! Temperature.publish(t))
    {                     
      Serial.println(F("Failed"));
    } 
     if (! Humidity.publish(h)) 
     {                     
      Serial.println(F("Failed"));
    }
    if (! Soil_Moisture.publish(moisture_percentage)) 
     {                     
      Serial.println(F("Failed"));
    }
    if (! Motor_Condition.publish(v)) 
     {                     
      Serial.println(F("Failed"));
    }
        else 
    {
      Serial.println(F("Sent!"));
      delay(2000);
    }
}
