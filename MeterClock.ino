//RGB Meter Clock
//by Dan, of ManiacalLabs.com
//Ft. FastLED light control by focalintent: github.com/FastLED/FastLED
//colortable.h by Adam of ManiacalLabs.com

#include "Wire.h"
#include "SPI.h"
#include "colortable.h"
#include "FastLED.h"

//PWM values coresponding to max meter readings (12 hrs or 60 min/sec)
#define MAX_SEC_PWM_VAL 166
#define MAX_MIN_PWM_VAL 169 
#define MAX_HOUR_PWM_VAL 227

#define NUM_LEDS 6		//number of LEDs in LPD8806 strand

#define NUM_MODES 6		//number of different light modes

// Number of RGB LEDs in strand:
int nLEDs = NUM_LEDS;
CRGB leds[NUM_LEDS];

//Arduino pins for meters. Need to be PWM pins
int secPin = 6;
int minPin = 10;
int hourPin = 9;

//used for buttons
int buttonApin = 4;
int buttonBpin = 5;
int bAdown = 0;
int bBdown = 0;

//PWM value for each meter
int secMeterVal = 0;
int minMeterVal = 0;
int hourMeterVal = 0;

int lightMode = 0;		//used in updateLights() to indicate which display setting to use.

int i = 0;		//used for RTC

int seconds=0, minutes=0, hours=0;		//global time variables


void setup() {  
  
  Wire.begin(); //Init I2C interface
  
  //set button pin modes
  pinMode(buttonApin, INPUT);
  pinMode(buttonBpin, INPUT);
  
  //  SetClock();       //Set the time in the function below, then uncomment to set time.
						//Once time is set, comment out this call and re-upload the sketch.

  GetTime();  //read time from RTC

  FastLED.addLeds<LPD8806, BRG>(leds, NUM_LEDS); //Init LEDs (LPD8806, BRG channel order, standard SPI pins: CLK=13, DATA=11)

  //Color Test on startup
  setMeterColor('s', 0, 0, 0);
  setMeterColor('m', 0, 0, 0);
  setMeterColor('h', 0, 0, 0);  
  FastLED.show();
  setMeterColor('s', 127, 0, 0);
  setMeterColor('m', 0, 127, 0);
  setMeterColor('h', 0, 0, 127);  
  FastLED.show();
  
  delay(1000);
}

void loop() {
   
  GetTime(); 

  processButtons();
  
  updateMeters();

  updateLights(lightMode);

  delay(50);
  
  //Dial Testing and Calibration
  /*//Test meter response
  seconds=seconds+5;
  if(seconds > 60){seconds=0;}
  minutes=minutes+5;
  if(minutes > 60){minutes=0;}
  hours++;
  if(hours >= 12){hours=0;}
  */
  /*//Test maximumm values
  seconds=60;
  minutes=60;
  hours=12;
  */
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

void processButtons()
{
	if(digitalRead(buttonApin))
  {
	  if(!bAdown)
	  {
		lightMode++;
		if(lightMode>(NUM_MODES-1)){lightMode=0;}
		bAdown=1;
	  }
  }
  else if(!digitalRead(buttonApin))
  {
	  if(bAdown){bAdown=0;}
  }
}

void updateMeters()
{
  secMeterVal = (float(seconds)/60)*MAX_SEC_PWM_VAL; 
  analogWrite(secPin, int(secMeterVal));
 
  minMeterVal = (float(minutes)/60)*MAX_MIN_PWM_VAL;
  analogWrite(minPin, minMeterVal);
  
  //Only show 12hr time on meter, but use 24hr time for meter color
  if(hours > 12){hourMeterVal = (float(hours-12)/12)*MAX_HOUR_PWM_VAL;}
  else{hourMeterVal = (float(hours)/12)*MAX_HOUR_PWM_VAL;}
  analogWrite(hourPin, hourMeterVal);
}

void updateLights(int lightMode)
{
	switch(lightMode)
	{
		case 0: //RGB Clock
			{
				TRGB sColor = colorMinSec[seconds];
				setMeterColor('s', sColor.r, sColor.g, sColor.b); 
				TRGB mColor = colorMinSec[minutes];
				setMeterColor('m', mColor.r, mColor.g, mColor.b);  
				TRGB hColor = color24h[hours];
				setMeterColor('h', hColor.r, hColor.g, hColor.b);
			}break;

		case 1: //Solid white, high brightness
			{
				setMeterColor('s', 225, 225, 225); 
				setMeterColor('m', 225, 225, 225);  
				setMeterColor('h', 225, 225, 225);
			}break;
		
		case 2: //Solid white, medium brightness
			{
				setMeterColor('s', 128, 128, 128); 
				setMeterColor('m', 128, 128, 128);  
				setMeterColor('h', 128, 128, 128);
			}break;

		case 3: //Solid white, low brightness
			{
				setMeterColor('s', 64, 64, 64); 
				setMeterColor('m', 64, 64, 64);  
				setMeterColor('h', 64, 64, 64);
			}break;
		case 4: //"Old Timey Radio" Amber
			{
				setMeterColor('s', 194, 114, 0); 
				setMeterColor('m', 194, 114, 0);  
				setMeterColor('h', 194, 114, 0);
			}break;
		case 5: //Mono-chrome Mode
			{
				int secVal = (float(seconds)/60)*200;
				int minVal = (float(minutes)/60)*200;
				int hourVal =( float(hours)/12)*200;
				setMeterColor('s', secVal, secVal, secVal); 
				setMeterColor('m', minVal, minVal, minVal);  
				setMeterColor('h', hourVal, hourVal, hourVal);
			}break;
	}
	FastLED.show();
}