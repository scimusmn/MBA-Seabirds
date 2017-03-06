// Value read from A1, in case plus mode is activated
float duty9 = .5;
// Float numbers to calculate duty for PWM 1 and PWM 2
float duty10 = .5;

int dirToggle = 12;
int enbBut = 2;

int dirOut = 7;
int enbOut = 10;

void setTimerOneFrequency(int hz){
  if(hz >= 16 && hz <= 1000000){
    ICR1 = 1000000 / hz;
    OCR1B = duty10*ICR1; //set duty cycle of pin 10
    OCR1A = duty9*ICR1; //set duty cycle of pin 9
  }
}

void pulseOutSetup() {
   TCCR1A = _BV(COM1A1) | _BV(COM1B1) ; // phase and frequency correct mode. NON-inverted mode
  // TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1A0) | _BV(COM1B0) ;
  //phase/frequency correct mode. SELECT THIS FOR INVERTED OUTPUTS.
  TCCR1B = _BV(WGM13) | _BV(CS11);
  // Select mode 8 and select divide by 8 on main clock.
}

void setup(){

  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  pinMode(dirOut,OUTPUT);
  pinMode(enbOut,OUTPUT);
  pinMode(dirToggle,INPUT);
  pinMode(enbBut,INPUT_PULLUP);

  digitalWrite(enbOut,HIGH);
  
  pulseOutSetup();
  setTimerOneFrequency(16);

  duty9 = .25;
}

unsigned long freq = 100;

void loop(){
  digitalWrite(enbOut,!digitalRead(enbBut));
  digitalWrite(dirOut,digitalRead(dirToggle));

  //setTimerOneFrequency(analogRead(0)*16+16);
  /*delay(2000);
  while(freq < 1000){
    freq++;
    setTimerOneFrequency(freq);
    delay(1);
  }
  delay(1000);
  while(freq > 16){
    freq--;
    setTimerOneFrequency(freq);
    delay(1);
  }*/
}
