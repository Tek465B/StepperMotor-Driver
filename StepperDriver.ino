/*
   Unipolar 2 + 2 Phase stepper motor driver(mitsumi M42SP-7) 48 step per rotation..
   ULN2003 Motor Driver(add 2k resistor to com pin) and DigiSpark AtTiny85(make sure reset is disabled)

   2021-08-04
   tek465b.github.io
*/

#include <DigisparkOLED.h>
#include <Wire.h>

#define SoftStart


byte rpmStart = 30;// start 30 ideal = 105-110
byte rpmMax = 105;
byte currentRPM = rpmStart;
byte StartStepSize = 5;
byte StartStepDelayMs = 250;
unsigned long previousMillis = 0;
int interval = 1000;

void setup() {

  DDRB |= B00111010; //pin 0, 1, 2, 5 as output

  cli();
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  //OCR0A = tmrcmp;
  SetRPM(rpmStart);
  TCCR0A |= (1 << WGM01); //CTC mode
  TCCR0B |= (1 << CS02); //1024 prescaler, 16.5M/1024=16.113k
  TCCR0B |= (1 << CS00);
  TIMSK |= (1 << OCIE0A); //Compare interrupt
  sei();

  oled.begin();
  oled.clear();
  oled.setFont(FONT8X16);
  
#ifdef SoftStart
  while (currentRPM < rpmMax) {
    currentRPM += StartStepSize;
    SetRPM(currentRPM);
    RefreshDisplay();
    delay(StartStepDelayMs);
  }
#endif


}


const byte MotorStep[] = {B00001010, B00011000, B00110000, B00100010}; //Stepper cycle 1 to 4

ISR(TIMER0_COMPA_vect) {
  static byte i = 0;
  byte tmp = PORTB & B11000101; //prepare register turning off all 4 pin  leaving restalone.
  PORTB = tmp | MotorStep[i % 4]; // set pin on leaving rest alone.
  i++;
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    RefreshDisplay();
  }

}

void SetRPM (byte rpm) {
  byte tmrcmp = 16113 / (rpm * 3.2); //using 4.28:1 gear reduction ratio.
  OCR0A = tmrcmp;
}

void RefreshDisplay() {
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  //char oledbuffer[15];
  //sprintf(oledbuffer, "RPM: %d", currentRPM);
  //oled.clear();
  oled.setCursor(0, 0);
  //oled.setFont(FONT8X16);
  oled.print("RPM: ");
  oled.print(int(currentRPM));
  oled.setCursor(0, 3);
  //oled.setFont(FONT8X16);
  //sprintf(oledbuffer, "Time: %d:%d:%d", (hr % 24), (min % 60), (sec % 60));
  oled.print("Time: ");
  oled.print(hr % 24);
  oled.print(":");
  oled.print(min % 60);
  oled.print(":");
  oled.print(sec % 60);

}
