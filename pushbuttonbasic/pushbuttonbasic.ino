const int led = 3;
const int button = 16;
int temp = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);

}z

void loop() {
  // put your main code here, to run repeatedly:
  temp = digitalRead(button);
  if(temp ==  HIGH){
    digitalWrite(led, HIGH);
    Serial.println("ligado");
  }
  else{
    digitalWrite(led, LOW);
    Serial.println("desligado");
  }

}
