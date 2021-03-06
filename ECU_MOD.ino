#define injector  6
#define FUELPUMP  11
#define TPS A0
#define EOT A2

volatile boolean fire = false;
boolean fuelon = false;
volatile int rpm;
volatile float newmicros;
volatile float oldmicros;
volatile long newmillis;
volatile long oldmillis;
volatile long lag;
int injectime;
int val;
volatile long injecdelay ;
float tps_r = false;
static int deciSeconds = false;

void setup()
{
  pinMode(injector, OUTPUT);
  pinMode(FUELPUMP, OUTPUT);
  pinMode(EOT, INPUT);
  pinMode(3, INPUT);
  pinMode(TPS, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), pulseISR, FALLING);  

  Serial.begin(115200);

      digitalWrite(injector , HIGH);
      delay(200);
      digitalWrite(injector,LOW);
      digitalWrite(FUELPUMP , HIGH);
      delay(3000);
      digitalWrite(FUELPUMP , LOW);
      digitalWrite(13,HIGH);
      delay(800);
      digitalWrite(13,LOW);
}

void loop()
{
timing();
sensor();
fuelON();
data();

  if ((micros() - newmicros) > 500000L) {
    newmicros = micros();
    rpm = 0;
    lag = 0;
  }
  static unsigned long timer = millis();
  if (millis() >= timer) {
    deciSeconds++;
    timer += 600;
    if ((deciSeconds == 220) && (rpm > 1)) {  //Reset to 100.
      deciSeconds = 201;
    } else if (rpm == 0) {//reset to 0.
      deciSeconds = 0;
    }
  }

   // 0-10000rpm
  if (fire) {
    rpm = (60000000L / (newmicros - oldmicros)); 
    lag = map(rpm, 0, 11000, 0, 12);
    oldmicros = newmicros;
    delayMicroseconds(lag);
    digitalWrite(injector, HIGH);
    delayMicroseconds(injecdelay);
    digitalWrite(injector, LOW);
    fire = false;
  }
  if (rpm <=  1) {
    digitalWrite(FUELPUMP, LOW);
  }
  if (rpm > 1) {
    digitalWrite(FUELPUMP, HIGH);
  }
}


void data(){
  newmillis = millis();

  if ((newmillis - oldmillis) > 250) {
    Serial.print("*r");
    Serial.print(rpm);
    Serial.print("*t");
    Serial.print(tps_r);
    Serial.print("*d");
    Serial.print(deciSeconds);
    Serial.print("*i");
    Serial.print(injectime);
    Serial.print("*u");
    Serial.print(injecdelay);
    Serial.print("*l");
    Serial.print(lag);
    oldmillis = newmillis;
  }
}

void fuelON(){
  if (rpm <=  1) {
    digitalWrite(FUELPUMP, HIGH);
    if(!fuelon){
      digitalWrite(FUELPUMP , LOW);
      delay(1000);
      digitalWrite(FUELPUMP , HIGH);
      fuelon=true;
      }
  }
  if (rpm > 1) {
    digitalWrite(FUELPUMP, LOW);
  }
}
 

void sensor(){
  val = analogRead(TPS);
  tps_r = map(val, 0, 1023, 0 , 255 );
}

void timing(){//mengatur bensin dan waktu nyala injektor
switch (lag) {
    case 0:
      if (deciSeconds <= 1){
        lag = 80;
        injecdelay = 4000;
      }
     if (tps_r <= 31){
      if ((deciSeconds > 1)&&(deciSeconds <= 100)) {
        lag = 90000;
        injecdelay = 1950;
      }
      if (deciSeconds >100) {
        lag = 150000;
        injecdelay = 1950;
      }
     }
     if ((deciSeconds > 1) && (tps_r > 31)){
        lag = 30000;
        injecdelay = 3000;
       }
      break;
    case 1:
      if (deciSeconds <= 1){
        lag = 80;
        injecdelay = 4000;
      }
    if (tps_r <= 31){
      if ((deciSeconds > 1)&&(deciSeconds <= 100)) {
        lag = 90000;
        injecdelay = 1950;
      }
      if (deciSeconds > 100) {
        lag = 150000;
        injecdelay = 1950;
      }
     }                           //case 0&1=lamsan
     if ((deciSeconds > 1) && (tps_r > 31)){
        lag = 30000;
        injecdelay = 3000;
      }
      break;
    case 2:
      if (deciSeconds <= 50) {
        if (tps_r <= 50) {
          lag = 80000;
          injecdelay = 3200;
        }
        if (tps_r > 50) {
          lag = 10000;
          injecdelay = 3500;
        }
      }                            //rpm=1600-2100
      if (deciSeconds > 50) {
        if (tps_r <= 50) {
          lag = 90000;
          injecdelay = 3500;
        }
        if (tps_r > 50) {
          lag = 30000;
          injecdelay = 4000;
        }
      }
      break;
    case 3:
      if (deciSeconds <= 50) {
        if (tps_r <= 60) {
          lag = 60000;
          injecdelay = 3500;
        } 
        if (tps_r > 60) {
          lag = 25000;
          injecdelay = 4000;
        }                         //rpm = 2100-3300
      }
      if (deciSeconds > 50) {
        if (tps_r <= 60) {
          lag = 50000;
          injecdelay = 3500;
        }
        if (tps_r > 60) {
          lag = 25000;
          injecdelay = 5000;
        }
      }
      break;
    case 4:
       if (tps_r <= 70) {
          lag = 40000;
          injecdelay = 3900;
        }
        if (tps_r > 70) {           //3300-4100
          lag = 20000;
          injecdelay = 5500;
        }
      break;
    case 5:
      if (tps_r <= 90) {
          lag = 35000;
          injecdelay = 4100;
        }                        //4100-5700
        if (tps_r > 90) {
          lag = 10000;
          injecdelay = 6100;
        }
      break;
    case 6:
      if (tps_r <= 100) {
          lag = 25000;
          injecdelay = 6500;
        }                        //5700-6800
        if (tps_r > 100) {
          lag = 1000;
          injecdelay = 7000;
        }
      break;
    case 7:
      if (tps_r <= 120) {
          lag = 4000;
          injecdelay = 7300;
        }                        //6800-7400
        if (tps_r > 120) {
          lag = 200;
          injecdelay = 7600;
        }
      break;
    case 8:
      if (tps_r <= 140) {
          lag = 400;
          injecdelay = 8000;
        }                        //7400-8500
       if (tps_r > 140) {
          lag = 100;
          injecdelay = 8500;
        }
      break;
    case 9:
      lag = 100;
      injecdelay = 5100;  //>8500rpm
      break;
    case 10:
      lag = 100;
      injecdelay = 5100;  //std 3100
      break;
    case 11:
      lag = 100;
      injecdelay = 5100;
      break;
    case 12:
      lag = 100;
      injecdelay = 4200;
      break;
    }
  if (rpm >= 11000) {
    lag = 90;
    injecdelay = 3500;
  }
}
void pulseISR() {
  newmicros = micros();
  if ((newmicros - oldmicros) > 500000L )
  {
    oldmicros = newmicros;
  }
  else
  {
    fire = true;
  }
}
