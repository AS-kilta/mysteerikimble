//int transistorPins[] = {10, 11};
int transistorPinCount = 6;
int transistorPinStart = 2;
int transistorPinEnd = 7;
int transistorOffset = 2;
int sensorPins[] = {A0, A1, A2};
int naksPin = 18; //must be 2, 3, 18, 19, 20, 21
int redPins[] = {31};
int bluePins[] = {33};
int greenPins[] = {35};
int yellowPins[] = {37};

int targetNum = 43;
unsigned long countDownCentiSec = 1000L*10L;//15L*60L*1000L;
unsigned long timercnt = 0;
int naksCount = 0;
unsigned long lastNaksTimer = 0;
bool timerStart = false;
bool blinkState = 0;
unsigned long lastTime = countDownCentiSec;

char digitStates[11] = {
  B11101101, B00101000, B11001011, B01101011, B00101110, B01100111, B11100111, B00101001, B11101111, B01101111, B00000010};


void setupTransistorPins() {
  for(int i = transistorPinStart; i < transistorPinEnd; i++){
    pinMode(i, OUTPUT);
  }
}

void setupSegmentPins() {
  DDRK = 0xFF;
}

void setupSensorPins() {
  for(int i = 0; i < 3; i++){
    pinMode(sensorPins[i], INPUT);    
  }
}

void setupTimer() {
  cli();
  TCCR0A = 0x00;
  TCCR0B = 0x00;
  TCNT0 = 0;

  // set compare match register for 50hz increments
  //formula = clockSpeed / (herz * prescalar) - 1
  OCR0A = 312;// = (16*10^6) / (50hz*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS02 and CS00 bits for 1024 prescaler
  TCCR0B |= (1 << CS02) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  sei();
}

void setupNaksCounter(){
  pinMode(naksPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(naksPin), onNaks, RISING);
}

void onNaks(){
  if(lastNaksTimer > 100) {
    naksCount++;
    lastNaksTimer = 0;
    blinkState = 1;
  }
}

void writeAllColors(int index, int State){
  digitalWrite(redPins[index], State);
  digitalWrite(bluePins[index], State);
  digitalWrite(yellowPins[index], State);
  digitalWrite(greenPins[index], State);
}

void blinkLeds(){
  switch(blinkState){
    case 0:
      return;
    case 1:
      writeAllColors(1, HIGH);
      writeAllColors(2, HIGH);
      writeAllColors(3, HIGH);
      writeAllColors(4, LOW);
      return;
    case 2:
      writeAllColors(1, HIGH);
      writeAllColors(2, HIGH);
      writeAllColors(3, LOW);
      writeAllColors(4, HIGH);
      return;
    case 3:
      writeAllColors(1, HIGH);
      writeAllColors(2, LOW);
      writeAllColors(3, HIGH);
      writeAllColors(4, HIGH);
      return;
    case 4:
      writeAllColors(1, LOW);
      writeAllColors(2, HIGH);
      writeAllColors(3, HIGH);
      writeAllColors(4, HIGH);
      return;
    default:
      writeAllColors(0, HIGH);
      writeAllColors(1, HIGH);
      writeAllColors(2, HIGH);
      writeAllColors(3, HIGH);
      blinkState = 0;
      return;
    
  }  
  unsigned long currTime = countDownCentiSec;
  if((lastTime - currTime) > 50) {
    blinkState++;
    lastTime = currTime;    
  }  
}

void setupLedPins(){
  for(int i = 0; i < 1; i++){
    pinMode(redPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(yellowPins[i], OUTPUT);    
  }
}

int debugIndexOffset = 0;

ISR(TIMER0_COMPA_vect){//timer0 interrupt 50Hz

  if(timerStart){
    if(naksCount < 150) {
      countDownCentiSec -= 4;
    }else{
      countDownCentiSec = 0;
    }

 
 
    int displayIndex = !(timercnt % 2);
    int digitIndex = timercnt % transistorPinCount; 
    char dp = B00010000;

    if((digitIndex + debugIndexOffset) < 2) {
      targetNum = (int)(countDownCentiSec / 60L / 1000L); //minutes
    }else if((digitIndex + debugIndexOffset ) < 4){
      targetNum = (int)((countDownCentiSec / 1000L) % 60L); //seconds
      //targetNum = (int)(countDownCentiSec / 60L / 100L); //minutes
    }else if((digitIndex + debugIndexOffset) < 6){
      targetNum = (int)(countDownCentiSec  % 1000L); //1/100 seconds
    }
    
    int divider = displayIndex ? 10 : 1;
    int digit = (targetNum / divider) % 10;
    // Serial.println(digit);
    //char transistorMask = 0x01 << (transistorOffset + digitIndex);

    if(naksCount >= 150) {
      digit = 10;
    }

    if(digitIndex == 1 || digitIndex == 3){
      PORTK = dp | digitStates[digit];
    }else{
       PORTK = digitStates[digit];   
    }
    
    for(int i = transistorPinStart; i < transistorPinEnd; i++){
      digitalWrite(i, LOW);      
    }
    digitalWrite(digitIndex + transistorOffset, HIGH);

    //PORTD = digitStates[digit] << 2; 
    //PORTB = transistorMask | (0xC0 & digitStates[digit]) >> 6;

    timercnt++;
    lastNaksTimer++;

  }
  
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(countDownCentiSec);
  pinMode(A0, INPUT);
  setupTransistorPins();
  setupSegmentPins();
  setupSensorPins();
  setupNaksCounter();
  setupLedPins();
  setupTimer();
}

int meas0;
int meas1;
int meas2;

bool measure(){
  meas0 = analogRead(sensorPins[0]);
  meas1 = analogRead(sensorPins[1]);
  meas2 = analogRead(sensorPins[2]);
  return (meas0 > 600);// && (meas1 > 800) && (meas2 > 800);  
    
}


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
  if(!timerStart) {
    timerStart = measure();
    if(timerStart){
      digitalWrite(31, HIGH);
      digitalWrite(33, HIGH);
      digitalWrite(35, HIGH);
      digitalWrite(37, HIGH);
    }    
  }

  Serial.println(naksCount);

  //if(blinkState){
  //  blinkLeds();
  //}
}
