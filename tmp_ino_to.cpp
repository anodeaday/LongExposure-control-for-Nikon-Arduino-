#include <Arduino.h>
#include <Arduino.h>




























#include <LiquidCrystal.h>






void setup();

void LowPower();

void WakeUp();

void loop();

int lengthofStops(int stopValue);


void PrintStops();


void checkForCancel();
void ReadButtons();




void updateEncoder();

void TimerUp();

void TimerDown();

void getCountDown();
void TakePicture();
#line 34 "LongExposure_Stops.ino"
int encoderPin1 = 2;
int encoderPin2 = 3;
int encoderSwitchPin = 10; //push button switch

bool b_takingPicture = false;

volatile int lastEncoded = 0;
volatile long encoderValue = 0;

long lastencoderValue = 0;

int lastMSB = 0;
int lastLSB = 0;
int button = 0;











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






unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 80;    // the debounce time; increase if the output flickers
long PowerSaveCount = 0;
long PowerSaveLength = 10000;
bool powerSaving = false;



int captureHours=0;
int captureMinutes=0;
int captureSeconds=10;

int captureMillis=0;

int counterHours = 0;
int counterMinutes = 0;
int counterSeconds = 0;

String captureTime = "00:00:10";


int cursorIndex=0;

bool b_capturing=false;
bool b_StopsOrTime = false;

bool buttonPressed=true;


int pinForCapture = 13;

int captureButton = 9;
int cancelButton = 8;


LiquidCrystal lcd(11, 12, 5, 4, 6, 7);           // select the pins used on the LCD panel


int lcd_key     = 0;
int adc_key_in  = 0;


#define HOUR 3600
#define MIN 60



void setup() {


pinMode(pinForCapture,OUTPUT); //Camera Pin
pinMode(cancelButton,INPUT);
pinMode(captureButton,INPUT);

digitalWrite(cancelButton,LOW);
digitalWrite(captureButton,LOW);


lcd.clear();
lcd.begin(16, 2);
lcd.setCursor(0, 0);
lcd.print("LONGEST EXPOSURE");
lcd.setCursor(0, 1);
lcd.print("   CALCULATOR   ");

delay(1500);
lcd.createChar(0, leftArrow);
lcd.createChar(1, rightArrow);

lcd.createChar(2, upArrow);

pinMode(pinForCapture,OUTPUT);
digitalWrite(pinForCapture,LOW);
lastDebounceTime = millis();



pinMode(encoderPin1, INPUT);
pinMode(encoderPin2, INPUT);

pinMode(encoderSwitchPin, INPUT);

digitalWrite(encoderPin1, HIGH); //turn pullup resistor on
digitalWrite(encoderPin2, HIGH); //turn pullup resistor on






attachInterrupt(0, updateEncoder, CHANGE);
attachInterrupt(1, updateEncoder, CHANGE);




}

void LowPower(){


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



if (!b_capturing){












if(digitalRead(encoderSwitchPin)==LOW){

b_StopsOrTime = !b_StopsOrTime;


}else{



}


ReadButtons();
delay(100);

if (buttonPressed){



PrintStops();

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


lcd.setCursor(0,1);
lcd.println("                ");
lcd.setCursor(8-(captureTime.length()*0.5),1);
lcd.print(captureTime);
}


void checkForCancel(){
if (digitalRead(cancelButton) == HIGH){
b_capturing=false;
}
}

void ReadButtons(){

if (digitalRead(captureButton)==HIGH){
b_capturing=true;
TakePicture();
buttonPressed=true;
}
}




void updateEncoder(){ //Update the encoder Turns
if (!b_capturing)
{
int MSB = digitalRead(encoderPin1); //MSB = most significant bit
int LSB = digitalRead(encoderPin2); //LSB = least significant bit

int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value

if(sum == 0b1011) {
if (b_StopsOrTime){

cursorIndex++;
}
else {
TimerUp();
}

}
if(sum == 0b1000) {
if (b_StopsOrTime){

cursorIndex--;
}
else {
TimerDown();
}


}





lastEncoded = encoded; //store this value for next time
}
}







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

unsigned long time_target=(long)tempExposureSeconds;


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

digitalWrite(pinForCapture,LOW);


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