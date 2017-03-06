int dir = 3;
int en = 4;
int pot = 1;

int d = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(dir,OUTPUT);
  pinMode(en,OUTPUT);

  digitalWrite(dir,1);
  digitalWrite(en,1);

  if(analogRead(pot) >= 100) d = 0;
  else d = 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  int newRead = analogRead(pot);
  if(d == 1 && newRead > 900){
    d = 0;
    digitalWrite(en, 0);
    delay(250);
    digitalWrite(dir, d);
    delay(250);
    digitalWrite(en, 1);
  } else if(d == 0 && newRead<100){
    d = 1;
    digitalWrite(en, 0);
    delay(250);
    digitalWrite(dir, d);
    delay(250);
    digitalWrite(en, 1);
  }

  digitalWrite(dir, d);
}
