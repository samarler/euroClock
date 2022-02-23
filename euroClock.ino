/*****************************************************************************
 Version 0.2.21.22
 This code is for a module that provides a clock rates of 1ppb, 24ppb and 48ppb.
 BPM is adjusted (between 40 and 180) using a POT and is displayed on a 7 segement 
 3 digit led controled by a MAX72XX connected bia hardware SPI.

 Code is written for the Adafruit ItsyBitsy 32u4 5V.
 
 Pin assignments:
   11 - Clock out for 1ppb
   10 - Clock out for 24ppb
   9 - Clock out for 48ppb
   7 - CS for SPI connection to MAX72xx
  A1 - Connected to 10k pot for internal clock rate

 Written by Scott Marler. GNU General Public License v3.0, check license.txt
 for more information.
 

 **************************************************************************/
/*
 * Include needed libraries and define values for DigitLed72xx
 */
#include <SPI.h>
#include <DigitLed72xx.h>
#define LOAD_PIN 7
#define NCHIP 2
DigitLed72xx ld = DigitLed72xx(LOAD_PIN, NCHIP);

/*
 * Initialize global variables
 */
unsigned long lastUpdate1;  //used for 1ppb timing
unsigned long lastUpdate24; //used for 24ppb timing
unsigned long lastUpdate48; //used for 48ppb timing
int pins[3] = {9,10,11};    //array of pins used for clock pulses


void setup() {
  ld.on(2);            //Start led display
  pinMode(11,OUTPUT);  //1ppb
  pinMode(10,OUTPUT);  //24ppb
  pinMode(9,OUTPUT);   //48ppb
  testLEDs();          //Test leds and led display
}

void loop() { 
/*
 * Read bpm value and print to led display
 * Determine milliseconds between 1ppb
 */
  int bpm = bpmRead();
  if(bpm < 100) {ld.clear();}
  ld.printDigit(bpm);
  int ticks = 60000/bpm;
/*
 * Pulse the clock outputs based on time between pulses
 */
  if( millis() - lastUpdate1 > ticks) {
    pulse(3);
    lastUpdate1 = millis();
  }
  else if( millis() - lastUpdate24 > ticks/24) {
      pulse(2);
      lastUpdate24 = millis();
      
  }
  else if( millis() - lastUpdate48 > ticks/48) {
      pulse(1);
      lastUpdate48 = millis();
  }

}

/*
 * Pulse the clock out pins
 */
void pulse(int num) {
  unsigned long Stime = millis();
  for(int i=0;i<num;i++) {
    digitalWrite(pins[i],HIGH);
  }
  while(millis() - Stime < 3) {}
  for(int i=0;i<num;i++) {
    digitalWrite(pins[i],LOW);
  }
}

/*
 * Read value from pot and map to bmp (40 to 180)
 */
int bpmRead() {
  int bpmSample = 0;
  for(int i=0;i<10;i++) {
    bpmSample = bpmSample + analogRead(A1);
  }
  int bpm = map(bpmSample/10,0,1023,40,180);
  return bpm;
}

/*
 * Routine run on setup to give visual indiction that the clock is functioning
 */
void testLEDs() {
  for(int i=2;i>=0;i--) {
    digitalWrite(pins[i],HIGH);
    ld.printDigit(i*111);
    delay(500);
  }
  delay(500);
  for(int i=2;i>=0;i--) {
    digitalWrite(pins[i],LOW);
    ld.printDigit((3-i)*111);
    delay(500);
  }
  delay(1000);  
  for(int i=0;i<3;i++) {digitalWrite(pins[i],HIGH);}
  ld.printDigit(123);
  delay(500);
  for(int i=0;i<3;i++) {digitalWrite(pins[i],LOW);}
  ld.clear();
  delay(500);
}
