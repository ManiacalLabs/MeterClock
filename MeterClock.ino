//RGB Meter Clock
//by Dan, of ManiacalLabs.com
//Ft. FastLED light control by focalintent: github.com/FastLED/FastLED
//colortable.h by Adam of ManiacalLabs.com

#include "Wire.h"
#include "SPI.h"
#include "colortable.h"
#include "FastLED.h"

#define MAX_SEC_PWM_VAL 166
#define MAX_MIN_PWM_VAL 169 
#define MAX_HOUR_PWM_VAL 227

#define NUM_LEDS 6

// Number of RGB LEDs in strand:
int nLEDs = NUM_LEDS;
CRGB leds[NUM_LEDS];

int SEC_addr = 0x09;
int MIN_addr = 0x0A;
int HR_addr = 0x0B;

int secPin = 6;
int minPin = 10;
int hourPin = 9;

int secMeterVal = 0;
int minMeterVal = 0;
int hourMeterVal = 0;

int i = 0; //used for RTC

int seconds=0, minutes=0, hours=0;
int prev_sec=0, prev_min=0, prev_hour=0;

void setup() {  
  
  Wire.begin(); //Init I2C interface
  
  //Serial.begin(9600);  //uncomment for debugging
  
  //  SetClock();          //Set the time in the function below, then uncomment to set time.
  //Once time is set, comment out this call and re-upload the sketch.

  GetTime();  //read time from RTC

  FastLED.addLeds<LPD8806, BRG>(leds, NUM_LEDS); //Init LEDs (LPD8806, BRG channel order, standard SPI pins: CLK=13, DATA=11)

  //Color Test on startup
  setMeterColor('s', 127, 0, 0);
  setMeterColor('m', 0, 127, 0);
  setMeterColor('h', 0, 0, 127);  
  FastLED.show();
  
  delay(1000);
}

void loop() {
  /*while(true)
   {
   byte s = 0;
   while(s < 60)
   {
   RGB sC = colorMinSec[s];
   for(byte i=0; i<9; i++)
   strip.setPixelColor(i, strip.Color(sC.r, sC.b, sC.g));
   
   Serial.print(s,DEC);
   Serial.print(": ");Serial.print(sC.r,DEC);
   Serial.print(",");Serial.print(sC.g,DEC);
   Serial.print(",");Serial.println(sC.b,DEC);
   strip.show();
   delay(250);
   s++;
   }
   
   for(byte i=0; i<9; i++)
   strip.setPixelColor(i, strip.Color(0, 0, 0));
   strip.show();
   delay(1000);
   }*/
   
  GetTime(); 

  //  Serial.print(" hue: ");Serial.print(h,DEC);
  //  Serial.print(" x: ");Serial.println(x,DEC);
  //  Serial.print("hue: ");Serial.println(h,DEC);
  //  Serial.print("R: ");Serial.print(HR.r,DEC);
  //  Serial.print("G: ");Serial.print(HR.g,DEC);
  //  Serial.print("B: ");Serial.println(HR.b,DEC);

  secMeterVal = (float(seconds)/60)*MAX_SEC_PWM_VAL; 
  analogWrite(secPin, int(secMeterVal));
  TRGB sColor = colorMinSec[seconds];
  setMeterColor('s', sColor.r, sColor.g, sColor.b);
  
  prev_sec=seconds;
 
  minMeterVal = (float(minutes)/60)*MAX_MIN_PWM_VAL;
  analogWrite(minPin, minMeterVal);
  TRGB mColor = colorMinSec[minutes];
  setMeterColor('m', mColor.r, mColor.g, mColor.b);
  
  prev_min=minutes;
 
  if(hours > 12){ //Only show 12hr time on meter, but use 24hr time for meter color
    hourMeterVal = (float(hours-12)/12)*MAX_HOUR_PWM_VAL;
  }
  else{
    hourMeterVal = (float(hours)/12)*MAX_HOUR_PWM_VAL;
  }
  analogWrite(hourPin, hourMeterVal);
  TRGB hColor = color24h[hours];
  setMeterColor('h', hColor.r, hColor.g, hColor.b);

  prev_hour=hours;

  //  Serial.print(" seconds: ");Serial.println(seconds,DEC);
  //  Serial.print(" R ");Serial.println(sColor.r,DEC);
  //  Serial.print(" G ");Serial.println(sColor.g,DEC);
  //  Serial.print(" B ");Serial.println(sColor.b,DEC);



  FastLED.show();
  delay(200);

  
  //Dial Testing and Calibration
  //----------
  /*
  seconds=seconds+5;
  if(seconds > 60){seconds=0;}
  minutes=minutes+5;
  if(minutes > 60){minutes=0;}
  hours++;
  if(hours >= 12){hours=0;}
  */
  /*
  seconds=60;
  minutes=60;
  hours=12;
  */
  //----------
}

static void GetTime()
{
  Wire.beginTransmission(104); // 104 is DS3231 device address
  Wire.write(i); // start at register 0
  Wire.endTransmission();
  Wire.requestFrom(104, 3); // request three bytes (seconds, minutes, hours)

  while(Wire.available())
  { 
    seconds = Wire.read(); // get seconds
    minutes = Wire.read(); // get minutes
    hours = Wire.read();   // get hours

    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
    minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
    hours = (((hours & 0b00110000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
  }
}

void SetClock()
{
  Wire.beginTransmission(104);
  Wire.write(i);
  Wire.write(0x80); //sec
  Wire.write(0x22 + 0x80); //min
  Wire.write(0x19 + 0x80); //hour (24)
  Wire.endTransmission();
}

void setMeterColor(char c, int r, int g, int b)
{
  if (c == 's')
  {
    leds[0] = CRGB::CRGB(r, g, b);
    leds[1] = CRGB::CRGB(r, g, b);
  }

  if (c == 'm')
  {
    leds[2] = CRGB::CRGB(r, g, b);
    leds[3] = CRGB::CRGB(r, g, b);
  }

  if (c == 'h')
  {
    leds[4] = CRGB::CRGB(r, g, b);
    leds[5] = CRGB::CRGB(r, g, b);
  }
}
