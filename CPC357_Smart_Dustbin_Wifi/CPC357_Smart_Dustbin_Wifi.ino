#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "LadyFaza-2.4GHz";
const char* password = "ladyfaza9779";
const char* mqtt_server = "34.60.95.18"; 
const int mqtt_port = 1883;

const int BIN_HEIGHT = 22;
const int GAS_THRESHOLD = 2000;

WiFiClient espClient;
PubSubClient client(espClient);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
Servo myServo;

LiquidCrystal_I2C lcd(0x27, 16, 2); 

#define TRIG_PIN 5
#define ECHO_PIN 6
#define PIR_PIN 21
#define SERVO_PIN 14
#define GAS_PIN A0  
#define GPS_RX 16
#define GPS_TX 15

unsigned long lastMsg = 0;
bool isLidOpen = false;
unsigned long lidTimer = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(42, 41); 
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0); lcd.print("Smart Dustbin");
  lcd.setCursor(0, 1); lcd.print("Initializing...");
  delay(2000);

  WiFi.begin(ssid, password);
  Serial.print("WiFi Connecting");
  lcd.setCursor(0, 1); lcd.print("WiFi Connecting");
  
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println(" Connected!");
  lcd.setCursor(0, 1); lcd.print("WiFi Connected! ");
  delay(1000);

  client.setServer(mqtt_server, mqtt_port);

  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT);
  pinMode(PIR_PIN, INPUT); 
  pinMode(GAS_PIN, INPUT);
  
  myServo.attach(SERVO_PIN); 
  myServo.write(0); 
  delay(500); 
  myServo.detach();
  
  lcd.clear();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP32_Dustbin")) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed rc="); Serial.print(client.state());
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();
  
  while (gpsSerial.available() > 0) gps.encode(gpsSerial.read());

  if (digitalRead(PIR_PIN) == HIGH && !isLidOpen) {
    Serial.println("Motion Detected! Opening Lid.");
    
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("Motion Detected!");
    lcd.setCursor(0, 1); lcd.print("Opening Lid...");
    
    myServo.attach(SERVO_PIN); myServo.write(90); delay(500);
    isLidOpen = true; lidTimer = millis();
  }
  
  if (isLidOpen && (millis() - lidTimer > 10000)) {
     Serial.println("Time up. Closing Lid.");
     myServo.attach(SERVO_PIN);
     for(int i=90; i>=0; i-=2) { myServo.write(i); delay(10); }
     isLidOpen = false; myServo.detach();
     
     lcd.clear();
  }

  if (millis() - lastMsg > 3000) {
    lastMsg = millis();
    
    digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
    int distance = duration * 0.034 / 2;
    int percent = map(distance, BIN_HEIGHT, 2, 0, 100);
    if(percent < 0) percent = 0; 
    if(percent > 100) percent = 100;
    
    int gasRaw = analogRead(GAS_PIN);
    
    Serial.print("Dist: "); Serial.print(distance); Serial.print("cm | ");
    Serial.print("Fill: "); Serial.print(percent); Serial.print("% | ");
    Serial.print("Gas: "); Serial.println(gasRaw);

    if (!isLidOpen) {
      lcd.setCursor(0, 0);
      lcd.print("Bin Level: "); lcd.print(percent); lcd.print("%   ");
      
      lcd.setCursor(0, 1);
      lcd.print("Gas: "); 
      if(gasRaw > GAS_THRESHOLD) {
        lcd.print("HIGH!   ");
      } else {
        lcd.print("LOW     ");
      }
    }

    StaticJsonDocument<200> doc;
    doc["fill"] = percent;
    doc["gas"] = gasRaw; 
    doc["lat"] = gps.location.isValid() ? gps.location.lat() : 0.0;
    doc["lng"] = gps.location.isValid() ? gps.location.lng() : 0.0;
    
    char buffer[256];
    serializeJson(doc, buffer);
    client.publish("dustbin/data", buffer);
    Serial.println("Sent Data!");
  }
}