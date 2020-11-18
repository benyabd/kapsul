#include <ESP8266WiFi.h>
#include <Wire.h>
#include <FirebaseArduino.h>
#include "DHT.h"

//setting up OLED
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Setting up host.
#define FIREBASE_HOST "crog-international.firebaseio.com"
#define FIREBASE_AUTH "8c7ic84mJjvsbRUFxCqhzCAQq1IK0VHlOM1uSZQ7"
#define WIFI_SSID "SOEGIARTO"
#define WIFI_PASSWORD "soegiarto45"
//
#define DHTPIN 13       //D
#define DHTTYPE DHT11   // DHT 11
//setting LED Strip
#include <FastLED.h>
#define NUM_LEDS 45
#define LED_PIN 3     //pin d
#define COLOR_ORDER GRB
#define LED_TYPE    WS2812B
CRGB leds[NUM_LEDS];
DHT dht(DHTPIN, DHTTYPE);
//setting
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(9600);
  dht.begin();
//  setup I/O
  pinMode(D0, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
//setting LFastled
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
  int pinNo =0;
// setting OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize I2C addr to 0x3C ( for 128x64 Display )
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(" CROG Capsule Hotel  ");
  display.display();
// connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);  
}

void loop() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("R01");
//DHT READ
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
//  Serial.println("Suhu Sensor :");
//  Serial.println(h);
//  Serial.println(t);
//  Serial.println(f);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

// Parse Data  
  
  
//def user

  String nama = Firebase.getString("data/Nama");
  String alamat = Firebase.getString("data/Alamat");
//ruang  
  String sandi = Firebase.getString("ruang/sandi");
  String ts = Firebase.getString("ruang/jammulai");
  String te = Firebase.getString("ruang/jamselesai");
  String ds = Firebase.getString("ruang/tglmulai");
  String de = Firebase.getString("ruang/tglselesai");
  int sr = Firebase.getInt("ruang/status");
  
//warna lampu
  int r = Firebase.getInt("LED/red");
  int g = Firebase.getInt("LED/green");
  int b = Firebase.getInt("LED/blue");
  int brightness = Firebase.getInt("LED/brightness");
// Statistik ruang
  int door = Firebase.getInt("stats/door");
  int lamp = Firebase.getInt("stats/lamp");
  int elct = Firebase.getInt("stats/electricity");
//suhu ruang
  float hum = Firebase.getFloat("suhu/hum");
  float ctemp = Firebase.getFloat("suhu/suhuc");
  float ftemp = Firebase.getFloat("suhu/suhuf"); 

 
//Serial.print("Start :");
//Serial.print(ds);
//Serial.print(" ");
//Serial.print(ts);
//Serial.print("| End :");
//Serial.print(de);
//Serial.print(" ");
//Serial.println(te);
//Serial.print("Booked by :");
//Serial.print(nama);
//Serial.print("Alamat :");
//Serial.println(alamat);
//Serial.print("Hum :");
//Serial.print(hum);
//Serial.print(" suhu (celcius) :");
//Serial.print(ctemp);
//Serial.print(" suhu (fahrenheit) :");
//Serial.println(ftemp);
//Serial.print("RGB R :");
//Serial.print(r);
//Serial.print(", G :");
//Serial.print(g);
//Serial.print(", B :");
//Serial.print(b);
//Serial.print(", Intensity :");
//Serial.print(brightness);
//Serial.println("%");
//Serial.print("Pintu :");
//Serial.println(door);
//Serial.print("Lampu :");
//Serial.println(lamp);
//Serial.print("Listrik :");
//Serial.println(elct);
 

// ========= Mulai QR CODE Reader
  if(Serial.available()){
        String ch;
        ch = Serial.readString();
        ch.trim();
        if(ch==sandi){
          Firebase.setInt("ruang/status",2);
          Firebase.setInt("stats/door",1);
          Firebase.setInt("stats/electricity",1);
          Firebase.setInt("stats/lamp",1);
          
          Serial.print("Sandi diterima");
          display.clearDisplay();
          display.setCursor(2, 0);
          display.print("Welcome ");
          display.print(nama);
          display.display();
          delay(1000);
          display.clearDisplay();
        }else{
          Serial.print("Sandi gagal : ");
          Serial.println(ch);
          Serial.print("harusnya : ");
          Serial.println(sandi);
          display.clearDisplay();
          display.setCursor(2, 0);
          display.print("QR Code Salah ");
          display.print(nama);
          display.display();
          delay(1000);
          display.clearDisplay();
        }
  }
//print
//cek statusruang --> LED
  String statusruang = "kosong";
  if(sr==1) statusruang = "booked";
  else if(sr==2) statusruang = "checked in";
  else statusruang = "kosong";
//end
  if(sr==2){
    digitalWrite(D0, lamp);
    digitalWrite(D4, elct);
    digitalWrite(D5, door);
    display.setCursor(20, 0);
    display.print("Checked");
    //display end    
    //display lamp    
    
    display.setCursor(0, 26);
    display.print("Lamp ");
    if(lamp==1) display.print("ON ");
    else display.print("OFF ");
    display.print("Door ");
    if(door==1) display.print("Locked");
    else display.print("Unlock");
    
    display.setCursor(0, 36);
    display.print("End ");
    display.print(de);      //date end
    display.print(" | ");
    display.print(te);      //time end

    display.setCursor(0, 46);
    display.print("Welcome ");
    display.print(nama);

//    end display
    FastLED.setBrightness(brightness);
      for (int pinNo = 0; pinNo <= NUM_LEDS-1; pinNo++) {
        leds[pinNo] = CRGB( r, g, b);    
      }
      FastLED.show(); // Update the LEDs
  }else{    
    display.setCursor(20, 0);
    if(sr==1)    
    display.print("Booked");
    else display.print("Kosong -");
//    end display
    digitalWrite(D0, LOW);
    digitalWrite(D4, LOW);
    digitalWrite(D5, LOW);
    FastLED.setBrightness(0);
    for (int pinNo = 0; pinNo <= NUM_LEDS-1; pinNo++) {
        leds[pinNo] = CRGB( 0, 0, 0);    
      }
    FastLED.show(); // Update the LEDs
  }
// QR end.
//display suhu
    display.setCursor(0, 16);
    display.print("T ");
    display.print(t);
    display.print("'C");
    display.print("Hum ");
    display.print(hum);
    display.print("%");  
    
  Firebase.setFloat("suhu/suhuc", t);
  Firebase.setFloat("suhu/suhuf", f);
  Firebase.setFloat("suhu/hum", h);
  display.display();
//  Serial.print("nama: ");
//  Serial.println(Firebase.getString("data/Nama"));
  delay(500);
}
