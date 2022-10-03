void setup() {
  // put your setup code here, to run once:
  pinMode(A0, INPUT);
  Serial.begin(9600);
}

int meas;

void loop() {
  // put your main code here, to run repeatedly:
  meas = analogRead(A0);
  Serial.println(meas);
}
