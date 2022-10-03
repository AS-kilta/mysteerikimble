void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

int meas;

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
