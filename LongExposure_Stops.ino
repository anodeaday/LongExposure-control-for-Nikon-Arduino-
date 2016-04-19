#include <Arduino.h>

////////////////////////////////////////////////////////////////////////
//
//    Long exposure Interface
//
//
//    This app let's you select a timer and then execute a
//    Long exposure capture via a connected Nikon / Canon camera.
//    The LCD displays the timer for the camera and then a
//    countdown once the shot has been initiated.
//    The capture can be cancelled through a button press.
//
//
////////////////////////////////////////////////////////////////////////

#include <LiquidCrystal.h>


byte upArrow[8] = {
	0b00100,
	0b01110,
	0b11111,
	0b01110,
	0b01110,
	0b01110,
	0b01110,
	0b01110
};

byte leftArrow[8] = {
	0b00010,
	0b00110,
	0b01110,
	0b11110,
	0b11110,
	0b01110,
	0b00110,
	0b00010
};

byte rightArrow[8] = {
	0b01000,
	0b01100,
	0b01110,
	0b01111,
	0b01111,
	0b01110,
	0b01100,
	0b01000
};

const String exposureList[] = {
	"1/8000" ,
	"1/6400" ,
	"1/5000" ,
	"1/4000" ,
	"1/3200" ,
	"1/2500" ,
	"1/2000" ,
	"1/1600" ,
	"1/1250" ,
	"1/1000" ,
	"1/800" ,
	"1/640" ,
	"1/500" ,
	"1/400" ,
	"1/320" ,
	"1/250" ,
	"1/200" ,
	"1/160" ,
	"1/125" ,
	"1/100" ,
	"1/80" ,
	"1/60" ,
	"1/50" ,
	"1/40" ,
	"1/30" ,
	"1/20" ,
	"1/15" ,
	"1/13" ,
	"1/10" ,
	"1/8" ,
	"1/6" ,
	"1/5" ,
	"1/4" ,
	"1/3" ,
	"1/2.5" ,
	"1/2" ,
	"1/1.6" ,
	"1/1.3" ,
	"1'" ,
	"1.3'" ,
	"1.6'" ,
	"2'" ,
	"2.5'" ,
	"3'" ,
	"4'" ,
	"5'" ,
	"8'" ,
	"10'" ,
	"13'" ,
	"15'" ,
	"20'" ,
	"25'" ,
	"30'" ,
};

const float exposureListTime[] = {
	0.000125 ,
	0.00015625 ,
	0.0002 ,
	0.00025 ,
	0.0003125 ,
	0.0004 ,
	0.0005 ,
	0.000625 ,
	0.0008 ,
	0.001 ,
	0.00125 ,
	0.0015625 ,
	0.002 ,
	0.0025 ,
	0.003125 ,
	0.004 ,
	0.005 ,
	0.00625 ,
	0.008 ,
	0.01 ,
	0.0125 ,
	0.016666667 ,
	0.02 ,
	0.025 ,
	0.033333333 ,
	0.05 ,
	0.066666667 ,
	0.076923077 ,
	0.1 ,
	0.125 ,
	0.166666667 ,
	0.2 ,
	0.25 ,
	0.33333333 ,
	0.4 ,
	0.5 ,
	0.625 ,
	0.769230769 ,
	1 ,
	1.3 ,
	1.6 ,
	2 ,
	2.5 ,
	3 ,
	4 ,
	5 ,
	8 ,
	10 ,
	13 ,
	15 ,
	20 ,
	25 ,
	30 ,
};

float tempExposureSeconds=10;

int stops = 10;
int exposureindex = 19;


//////////////////////////////////////////  Debounce Variables

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 80;    // the debounce time; increase if the output flickers
long PowerSaveCount = 0;
long PowerSaveLength = 10000;
bool powerSaving = false;


//////////////////////////////////////////  Setup for variables for Timer
int captureHours=0;
int captureMinutes=0;
int captureSeconds=10;

int captureMillis=0;

int counterHours = 0;
int counterMinutes = 0;
int counterSeconds = 0;

String captureTime = "00:00:10";
// String blinkCaptureTime=captureTime;

int cursorIndex=0;

bool b_capturing=false;
bool buttonPressed=true;

//Init Pins for CameraCapture;
int pinForCapture = 2;

//////////////////////////////////////////  Init Pins for LCD

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

////////////////////////////////////////// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define HOUR 3600
#define MIN 60

int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor

    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    // We make this the 1st option for speed reasons since it will be the most likely result

		if ((millis() - lastDebounceTime) > debounceDelay)
		{
					lastDebounceTime = millis();
					if (adc_key_in > 1000) return btnNONE;
			    if (adc_key_in < 50)   return btnRIGHT;
			    if (adc_key_in < 195)  return btnUP;
			    if (adc_key_in < 380)  return btnDOWN;
			    if (adc_key_in < 555)  return btnLEFT;
			    if (adc_key_in < 790)  return btnSELECT;
		}
    return btnNONE;                // when all others fail, return this.
}


void setup() {
	analogWrite(10,80); //Turn down backlight brightness
	pinMode(13,OUTPUT);
	// Serial.begin(9600);
  lcd.clear();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
	lcd.print("LONGEST EXPOSURE");
  lcd.setCursor(0, 1);
	lcd.print("   CALCULATOR   ");
  // lcd.print("");
	delay(1500);
	lcd.createChar(0, leftArrow);
	lcd.createChar(1, rightArrow);

	lcd.createChar(2, upArrow);

  pinMode(pinForCapture,OUTPUT);
  digitalWrite(pinForCapture,LOW);
	lastDebounceTime = millis();



}

void LowPower(){
	// LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
							//  SPI_OFF, USART0_OFF, TWI_OFF);
		digitalWrite(13,LOW);
		analogWrite(10,0); //Turn down backlight brightness
		lcd.noDisplay();
}

void WakeUp(){
	powerSaving=false;

	lcd.display();
	analogWrite(10,80);
}

void loop() {

	// LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,
							//  SPI_OFF, USART0_OFF, TWI_OFF);
// LowPower();

if (!b_capturing){
	if (!powerSaving){
		if (millis() - PowerSaveCount > PowerSaveLength){
			lcd.setCursor(0,0);
			lcd.print(" ENTERING POWER ");
			lcd.setCursor(0,1);
			lcd.print("     SAVING     ");
			delay(800);
			powerSaving=true;
			LowPower();
		}
	}
  ReadButtons();
  delay(50);

  if (buttonPressed){
    // printTime();
		WakeUp();

		PrintStops();
		PowerSaveCount = millis();
    buttonPressed=false;
  }
}

}

int lengthofStops(int stopValue){
	if(stopValue < 10)
  return 0;
	else if(stopValue >= 10)
  return 1;
}

void PrintStops(){
	lcd.setCursor(0,0);
	lcd.println("                ");

	lcd.setCursor(0,0);
	lcd.print(exposureList[exposureindex]);

	lcd.setCursor(8,0);
	lcd.write((uint8_t)cursorIndex);

	lcd.setCursor(15-(lengthofStops(stops)),0);
	lcd.print(stops);

	lcd.setCursor(4,1);
	// float newexposure = exposureListTime[exposureindex]*(pow(2,stops);
	// lcd.print(exposureListTime[exposureindex]*(pow(2,stops)));
	lcd.setCursor(0,1);
	lcd.println("                ");
	lcd.setCursor(8-(captureTime.length()*0.5),1);
	lcd.print(captureTime);
}


void checkForCancel(){
	lcd_key = read_LCD_buttons();   // read the buttons
	switch (lcd_key){               // depending on which button was pushed, we perform an action
			case btnLEFT:{
				b_capturing=false;
				break;
			}
		}
}
//////////////////////////////////////////  Button pressed!
void ReadButtons(){
  lcd_key = read_LCD_buttons();   // read the buttons
  switch (lcd_key){               // depending on which button was pushed, we perform an action

         case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen

              buttonPressed=true;
              // lcd.setCursor(0,1);
              // lcd.println("                ");
              if (cursorIndex<1){
                cursorIndex++;
              }
              // else{
                // cursorIndex=0;
              // }

              break;
         }
         case btnLEFT:{

              buttonPressed=true;
              // lcd.setCursor(0,1);
              // lcd.println("                ");
               if (cursorIndex>0){
                cursorIndex--;
              }
              // else{
                // cursorIndex=3;
              // }
               break;
         }
         case btnUP:{

              buttonPressed=true;
               TimerUp();
               break;
         }
         case btnDOWN:{

              buttonPressed=true;
               TimerDown();
               break;
         }
         case btnSELECT:{

               if (!b_capturing){
                b_capturing=true;
                TakePicture();
               }
               else{
                b_capturing=false;
               }
               break;
         }
         case btnNONE:{

               break;
         }
     }
}

//////////////////////////////////////////  Timer Up

void TimerUp(){

  if (cursorIndex==1){
		if (stops<50){
	    stops++;
		}
  }
	if (cursorIndex==0){

		if (exposureindex > 0){
			exposureindex--;
		}
  }
	getCountDown();

}


//////////////////////////////////////////  Timer Down

void TimerDown(){
	if (cursorIndex==1){
		if (stops>0){
	    stops--;
		}
  }
	if (cursorIndex==0){
		if (exposureindex < 52){
	    exposureindex++;
	  }
  }
	getCountDown();
}

void getCountDown(){
	tempExposureSeconds = exposureListTime[exposureindex]*(pow(2,stops));
	if (tempExposureSeconds < 1){
		captureTime = exposureList[exposureindex+(stops*3)];
	}
	else{
		//Calculate time
		unsigned long time_target=(long)tempExposureSeconds;
		// Serial.print("Exposure time in Seconds: ");
		// Serial.println(time_target);
    long hour = time_target/HOUR;
    long second = time_target % HOUR;
    long minute = second/MIN;
    second = second % MIN ;

		captureHours= hour;
		captureMinutes=minute;
		captureSeconds=second;

		captureTime = "";
		if (captureHours<10){ captureTime = captureTime + "0"+captureHours;} else {captureTime =captureTime + captureHours;};
		captureTime = captureTime + ":";
		if (captureMinutes<10){ captureTime = captureTime + "0"+captureMinutes;} else {captureTime =captureTime + captureMinutes;};
		captureTime = captureTime + ":";
		if (captureSeconds<10){ captureTime = captureTime + "0"+captureSeconds;} else {captureTime =captureTime + captureSeconds;};
	}
}

//////////////////////////////////////////  Take a picture!
void TakePicture(){
  lcd.println("                ");
  lcd.setCursor(0,1);
  lcd.println("                ");

  counterHours = captureHours;
  counterMinutes = captureMinutes;
  counterSeconds = captureSeconds;

	if (tempExposureSeconds < 1){
		lcd.setCursor(0, 0);

    lcd.print(" QUICK PICTURE ");
		lcd.setCursor(0,1);
		lcd.print(exposureList[exposureindex]);

		unsigned int quickExposure = tempExposureSeconds*1000000;
		digitalWrite(pinForCapture,HIGH);
		delayMicroseconds(quickExposure);
		// delay(tempExposureSeconds*1000);
		digitalWrite(pinForCapture,LOW);
		// Serial.print("Quick exposure: ");
		// Serial.println(tempExposureSeconds*1000000);
		b_capturing=false;
	}
	else{
		captureMillis=1000;
		digitalWrite(pinForCapture,HIGH);
	}

  while (b_capturing==true)
  {
    lcd.setCursor(0, 0);
    lcd.print(" TAKING PICTURE ");

    if (captureMillis>0)
    {
      lcd.setCursor(0, 1);

      captureTime = "";
      if (counterHours<10){ captureTime = captureTime + "0"+counterHours;} else {captureTime =captureTime + counterHours;};
      captureTime = captureTime + ":";
      if (counterMinutes<10){ captureTime = captureTime + "0"+counterMinutes;} else {captureTime =captureTime + counterMinutes;};
      captureTime = captureTime + ":";
      if (counterSeconds<10){ captureTime = captureTime + "0"+counterSeconds;} else {captureTime =captureTime + counterSeconds;};

      lcd.setCursor(4,1);
      lcd.print(captureTime);

            if ((counterHours==0) && (counterMinutes==0) && (counterSeconds==0)){
              captureMillis=0;
            }
            else{
              delay(1000);
							checkForCancel();
            }

      if (captureMillis>0)
			{
        if (counterSeconds>0){
          counterSeconds=counterSeconds-1;
        }
        else {
            if (counterMinutes>0){
              counterMinutes=counterMinutes-1;
              counterSeconds=59;
            }
            else {
              if (counterHours>0){
                counterHours=counterHours-1;
                counterMinutes=59;
                counterSeconds=59;
              }
              else{
                captureMillis=0;
              }
            }
        }
      }
    }
//Deleted something. Fuck...
else
    {
      b_capturing=false;
    }
  }
  digitalWrite(pinForCapture,LOW);
  lcd.clear();
	getCountDown();
  buttonPressed=true;
}
