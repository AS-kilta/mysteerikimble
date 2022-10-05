int transistorPins[] = {10, 11};
int transistorPinCount = 2;
int segmentPinStart = 2;
int segmentPinEnd = 9;
int transistorOffset = 2;

int targetNum = 43;
unsigned int countDownMS = 15*60*1000; 
unsigned int timercnt = 0;

char digitStates[10] = {
  B01110111, B01000001, B00111011, B01101011, B01001101, B01101110, B01111110, B01000011, B01111111, B01001111};


void setupTransistorPins() {
  for(int i = 0; i < transistorPinCount; i++){
    pinMode(transistorPins[i], OUTPUT);
  }
}

void setupSegmentPins() {
  for(int i = segmentPinStart; i <= segmentPinEnd; i++){
    pinMode(i, OUTPUT);
  }
}

void setupTimer() {
  cli();
  TCCR0A = 0x00;
  TCCR0B = 0x00;
  TCNT0 = 0;

  // set compare match register for 50hz increments
  //formula = clockSpeed / (herz * prescalar) - 1
  OCR0A = 155;// = (8*10^6) / (50hz*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS02 and CS00 bits for 1024 prescaler
  TCCR0B |= (1 << CS02) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  sei();
}


ISR(TIMER0_COMPA_vect){//timer0 interrupt 50Hz

  countDownMS -= 10;
  int targetMin = countDownMS / 60 / 1000;
  int targetSec = (countDownMS / 1000) % 60;
  int targetCentiSec = (countDownMS / 10) % 100;
 
  targetNum = targetMin * 10000 + targetSec * 100 + targetCentiSec;
  // Serial.println(targetMin);
  int displayIndex = timercnt % transistorPinCount;
  int digit = (targetNum / (int)pow(10, (displayIndex + 2))) % 10;
  char transistorMask = 0x01 << (transistorOffset + displayIndex);

  PORTD = digitStates[digit] << 2; 
  PORTB = transistorMask | (0xC0 & digitStates[digit]) >> 6;

  timercnt++;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  setupTransistorPins();
  setupSegmentPins();
  setupTimer();
  

  Serial.begin(9600);
}

int meas;
int currentMillis;

/* inputs
 *
 * popomatic
 * 3x ADC (hall sensor)
 */

/* outputs
 *
 * 16x goal led
 * 8x 7-segment led
 * 8x 7-segment transistor
 * 1x neopixel (RGB!!)
 */
void loop() {
  // put your main code here, to run repeatedly:
  meas = analogRead(A0);
  Serial.println(meas);
}
