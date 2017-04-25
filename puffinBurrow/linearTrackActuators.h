#include "arduino.h"

/****************************************************
* This library is used to asynchronously drive
* the linear track actuators from OpenBuilds.
 */

////////////////////////////////////////////////////////

//This code is for driving multiple tracks concurrently
// at different speeds, by utilising  mutliple timers in the
// arduino to send the pulse outputs.
// It's not currently functional, but leaving here for later 
// development.

/*static void setTimerOneFrequency(int hz){
  if(hz >= 16 && hz <= 1000000){
    ICR1 = 1000000 / hz;
    OCR1B = .5*ICR1; //set duty cycle of pin 10
    OCR1A = .5*ICR1; //set duty cycle of pin 9
  }
}

static void pulseOutSetup() {
   TCCR1A = _BV(COM1A1) | _BV(COM1B1) ; // phase and frequency correct mode. NON-inverted mode
  // TCCR1A = _BV(COM1A1) | _BV(COM1B1) | _BV(COM1A0) | _BV(COM1B0) ;
  //phase/frequency correct mode. SELECT THIS FOR INVERTED OUTPUTS.
  TCCR1B = _BV(WGM13) | _BV(CS11);
  // Select mode 8 and select divide by 8 on main clock.
}*/

// stores the number of the pin which is currently being pulsed
static int tonePin = 0;

// The actuator class. I'll detail each of the functions as
// they are declared.

class actuator {
public:
  int pulsePin;
  int dirPin;
  int enbPin;

  int lftLm;
  int rgtLm;

  // stores the direction and pulse speed
  int dir;
  int pps;

  //timer for moving by (approximate) count number
  unsigned long tmr;

  //Tone pulser;

  bool running;
  bool countRun;
  int counts;

  //speed profile controls
  bool useProfile;
  unsigned long oscTmr;
  unsigned long runTmr;
  int runTimeout;
  float oscillator;
  float oscMax;
  float oscIncrement;
  float incrementInterval;
  bool resetOnRun;

  //stores the pointer to the callback function
  void (*endCB)();

  signed int (*speedFunc)(float);

  //declare the actuator object and set defaults.
  actuator(int pwm, int d, int enb, int l, int r){
    pulsePin = pwm;
    enbPin = enb;
    dirPin = d;
    
    lftLm = l;
    rgtLm = r;
    
    pps = 0;
    dir = 0;
    endCB = 0;
    running = false;
    runTimeout = 0;
    countRun = false;

    useProfile = false;
    oscTmr = 0;
    oscillator = 0;
    oscMax = 3.1415*8;
    oscIncrement = .01;
    incrementInterval = 2; //ms
    resetOnRun = false;

    pinMode(dirPin,OUTPUT);
    pinMode(enbPin,OUTPUT);
    digitalWrite(dirPin,LOW);
    digitalWrite(enbPin,LOW);

    pinMode(lftLm,INPUT_PULLUP);
    pinMode(rgtLm,INPUT_PULLUP);
    
  }

  int direction(){
    return dir;
  }

  void setSpeed(int sp){
    pps = sp;
    if(tonePin != pulsePin) noTone(tonePin);
    tonePin = pulsePin;
    tone(pulsePin,sp);
  }

  void setSpeedProfile(signed int (*prof)(float),bool reset = false){
    useProfile = true;
    speedFunc = prof;
    resetOnRun = reset;
  }
  
  void setDirection(int d){
    dir = d;
    digitalWrite(dirPin,dir);
  }
  
  void start(){
    digitalWrite(enbPin,HIGH);
  }
  
  void stop(){
    digitalWrite(enbPin,LOW);
  }
  
  void registerEndCB(void (*CB)()){
    endCB = CB;
  }
  
  void run(int d, int spd = 0){
    if(spd > 0) setSpeed(spd);
    if(resetOnRun) oscillator = 0;
    running = true;
    dir = d;
    digitalWrite(dirPin,dir);
    digitalWrite(enbPin,HIGH);

    if(runTimeout){
      runTmr = millis()+runTimeout;
    }
  }

  void runByCounts(int cnts, int d, int spd = 0){
    if(spd > 0) setSpeed(spd);
    if(resetOnRun) oscillator = 0;
    dir = d;
    countRun = true;
    counts = cnts;
    running = true;
    runTmr = millis()+runTimeout;
    float ppms = pps/1000.;
    tmr = millis() + counts/ppms;
    digitalWrite(dirPin,dir);
    digitalWrite(enbPin,HIGH);
  }
  
  void idle(){

    if(!digitalRead(lftLm) && dir && running){
      running = false;
      stop();
      if(endCB) endCB();
    } else if(!digitalRead(rgtLm) && !dir && running){
      running = false;
      stop();
      if(endCB) endCB();
    }

    if(runTimeout && running && millis() > runTmr){
      running = false;
      stop();
      if(endCB) endCB();
    }

    if(countRun && running && ((!useProfile && millis()>tmr) || counts <=0)){
      running = false;
      countRun = false;
      stop();
      if(endCB) endCB();
    }

    if(millis()>oscTmr && running && useProfile){
      if(speedFunc) setSpeed(abs(speedFunc(oscillator)));
      if(speedFunc(oscillator) < 0) digitalWrite(dirPin,!dir);
      else digitalWrite(dirPin,dir);
      oscillator+=oscIncrement;
      if(oscillator>=oscMax) oscillator=0;
      oscTmr = millis() + incrementInterval;
      counts -= incrementInterval * pps/1000. * ((speedFunc(oscillator) < 0)?-1:1);
    }
  }
};



