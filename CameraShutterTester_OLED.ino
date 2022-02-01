#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

long StartTime;
long StopTime;
bool ShutterFired = false;
bool RisingFlag = false;
bool FallingFlag = false;
long TestNumber = 0;

void setup() {
  // Initial OLED Screen
  u8g2.begin();
  u8g2.clearBuffer();                 // clear the internal memory
  u8g2.setFont(u8g2_font_helvB10_tr); // choose a suitable font
  u8g2.drawStr(0,12,"Time    :");     // write something to the internal memory
  u8g2.drawStr(0,29,"Speed :");       // write something to the internal memory
  u8g2.sendBuffer();                  // transfer internal memory to the display
  
  Serial.begin(9600);
  Serial.println("Shutter Speed Tester STARTED");
  attachInterrupt(digitalPinToInterrupt(2), CLOCK, CHANGE);
}

void loop() {
  // Shutter opens
  if (RisingFlag == true) {
    StartTime = micros();
    RisingFlag = false;
  }
  // Shutter closes
  if (FallingFlag == true) {
    StopTime = micros();
    FallingFlag = false;
    // Shutter has now FIRED
    ShutterFired = true;
  }
  // Shutter fired
  if (ShutterFired == true) {
    TestNumber = TestNumber + 1;
    // Print results to console
    Serial.print("--- Shutter Test No. ");
    Serial.print(TestNumber);
    Serial.println(" ---");
    Serial.print("Start:          ");
    Serial.println(StartTime);
    Serial.print("Stop:           ");
    Serial.println(StopTime);
    long ShutterFiredDurationMicroSecs = (StopTime - StartTime);
    Serial.print("Duration:       ");
    Serial.print(ShutterFiredDurationMicroSecs);
    Serial.println(" μs");
    float ShutterFiredDurationSecs = (float)ShutterFiredDurationMicroSecs / 1000000;
    Serial.print("Duration:       ");
    Serial.print(ShutterFiredDurationSecs, 5);
    Serial.println(" s");
    float ShutterFiredDurationSecsInverse = 1 / ShutterFiredDurationSecs;
    Serial.print("Shutter Speed:  1/");
    Serial.println(ShutterFiredDurationSecsInverse, 0); // gets rid of 2 decimal places
    Serial.println();
    
    // Print results to OLED - Shutter Time
    char ShutterTimeResult[8];
    dtostrf(ShutterFiredDurationSecs, -8, 5, ShutterTimeResult);
    DISPLAY_SHUTTER_TIME(ShutterTimeResult);
    
    // Print results to OLED - Shutter Speed
    int ShutterSpeed = round(ShutterFiredDurationSecsInverse);
    char ShutterSpeedResult[8];
    if (ShutterSpeed == 0) {
      strcpy(ShutterSpeedResult, "too slow");
    }
    else {
      sprintf(ShutterSpeedResult, "1/%d", ShutterSpeed);  
    }
    
    strcat(ShutterSpeedResult, "     ");
    DISPLAY_SHUTTER_SPEED(ShutterSpeedResult);
    
    // Reset 'ShutterFired'
    ShutterFired = false;    
  }
}

void CLOCK() {
  if (digitalRead(2) == LOW) {
    RisingFlag = true;
    FallingFlag = false;
  }
  if (digitalRead(2) == HIGH) {
    FallingFlag = true;
    RisingFlag = false;
  }
}

void DISPLAY_SHUTTER_TIME(char* ShutterTime) {
  u8g2.setFont(u8g2_font_8x13_mf);  
  u8g2.drawStr(60,12,ShutterTime);
  u8g2.sendBuffer();
}

void DISPLAY_SHUTTER_SPEED(char* ShutterSpeed) {
  u8g2.setFont(u8g2_font_8x13_mf);  
  u8g2.drawStr(60,29,ShutterSpeed);
  u8g2.sendBuffer();
}
