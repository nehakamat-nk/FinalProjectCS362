//refrences: https://forum.arduino.cc/t/coding-a-timer-with-4-digit-7-segement-display-and-74hc595/1295212
// https://docs.arduino.cc/language-reference/en/functions/random-numbers/random/
// https://docs.arduino.cc/learn/communication/wire/


int led1 = A0;
int led2 = A1;
int led3 = A2;

int x = 0;
long randomNum = 0;
bool timerOn = false;
char fastestTime [4];

int pinA = 2;
int pinB = 3;
int pinC  = 4;
int pinD = 5;
int pinE = 6;
int pinF = 7;
int pinG = 8;
int pinDP = 9;
int D1 = 10;
int D2 = 11;
int D3 = 12;
int D4 = 13;

unsigned long startTime = 0;
unsigned long elapsedTime = 0;
char digits [4];
const int segmentPins[] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};
const int digitPins[] = {D4, D3, D2, D1};

unsigned long previous = 0;
unsigned long interval = 2;
int currentDigit = 0;

const byte digitPatterns[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

#include <Wire.h>

// the setup routine  runs once when you press reset:
void setup() {    
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  // needed for random
  randomSeed(analogRead(A3));

  // debug prints
  // Serial.println("Countdown Timer Started: 00:00");

  //setup all pins
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  // Set digit pins as outputs
  for (int i = 0; i < 4; i++) {
    pinMode(digitPins[i], OUTPUT);
  }
  // setup leds
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

}

// the loop routine runs over  and over again forever:
void loop() {
  //check if we recieve a signal to start clock and select a random led
  if(x == 1){
    if (!timerOn){
      startTime = millis();
      timerOn = true;

      // get random num
      randomNum = random(1, 4); // 1-3
      Serial.println(randomNum);
      // reset all leds 
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
      digitalWrite(led3, LOW);

      // update leds based on what random number is recieved
      if(randomNum == 1){
        digitalWrite(led1, HIGH);
      } else if(randomNum == 2){
        digitalWrite(led2, HIGH);
      } else if(randomNum == 3){
        digitalWrite(led3, HIGH);
      } else{
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
      }

    }
    unsigned long currTime = millis();
    elapsedTime = currTime - startTime;
    
    // figure out all places(1000s, 100s, 10s, 1s)
    digits[0] = (elapsedTime / 1000) % 10;
    digits[1] = (elapsedTime / 100) % 10;
    digits[2] = (elapsedTime / 10) % 10;
    digits[3] = elapsedTime % 10;

    // debug print
    // char buffer[20];
    // sprintf(buffer, "%d.%d%d%d", digits[0], digits[1], digits[2], digits[3]);
    // Serial.println(buffer);
    
    // constanly update the digit display
    if (currTime - previous >= interval) {
      previous = currTime;
      displayDigit(currentDigit, digits[currentDigit]);
      currentDigit = (currentDigit + 1) % 4;
    }

  }
  else if (x == 0){
    // if we recieve anything other than a 0, reset timer and reset LEDS
    if(timerOn){
      timerOn = false;

      fastestTime[0] = digits[0];
      fastestTime[1] = digits[1];
      fastestTime[2] = digits[2];
      fastestTime[3] = digits[3];
    }

    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);

    unsigned long currTime = millis();

    if (currTime - previous >= interval) {
      previous = currTime;
      displayDigit(currentDigit, fastestTime[currentDigit]);
      currentDigit = (currentDigit + 1) % 4;
    }
  }
}

void displayDigit(int digit, int number) {
  // turn off display
  for (int i = 0; i < 4; i++) {
    digitalWrite(digitPins[i], HIGH);
  }
  // select num
  byte pattern = digitPatterns[number];
  // add decimal point
  if(digit == 0){
    pattern |= 0b10000000;
  }
  //update display with new vals
  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], (pattern >> i) & 0x01);
  }
  
  //turn on current digit
  digitalWrite(digitPins[digit], LOW);
}

void receiveEvent(int howMany)
{
  // check if anything is sent
  while (Wire.available()){
    x = Wire.read();
  }

}
