/*
 * Unipolar 2 + 2Phase stepper motor driver(mitsumi M42SP-7) 48 step per rotation..
 * ULN2003 Motor Driver and DigiSpark AtTiny85
 * 
 * 2021-08-04
 * tek465b.github.io
 */


void setup() {

  byte rpm = 74;// min = 74 ideal = 116
  byte tmrcmp = 64453/(rpm*3.424); //using 4.28:1 gear reduction ratio.

  cli();
  TCCR0A = 0;
  TCCR0B = 0;
  TCNT0  = 0;
  OCR0A = tmrcmp;
  TCCR0A |= (1 << WGM01); //CTC mode
  TCCR0B |= (1 << CS02); //256 prescaler, 16.5M/256=64.453kHz
  TIMSK |= (1 << OCIE0A); //Compare interrupt
  sei();

  DDRB |= B00100111; //pin 0, 1, 2, 5 as output

}


const byte MotorStep[] = {B00000011, B00000110, B00100100, B00100001}; //Stepper cycle 1 to 4

ISR(TIMER0_COMPA_vect) {
  static byte i = 0;
  byte tmp = PORTB & B11011000; //prepare register turning off all 4 pin  leaving restalone.
  PORTB = tmp | MotorStep[i%4]; // set pin on leaving rest alone.
  i++;
}

void loop() {


}
