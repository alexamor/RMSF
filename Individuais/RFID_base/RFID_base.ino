
int ledOpen = 8;
int switchReed = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledOpen, OUTPUT);
  pinMode(switchReed, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(switchReed) == HIGH){
    digitalWrite(ledOpen, LOW);
    Serial.println("Your door is Closed");
  }
  else{
    digitalWrite(ledOpen, HIGH);
    Serial.println("Your door is Open");
  }
  delay(1);

}
