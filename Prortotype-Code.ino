#include <SoftwareSerial.h>
#include <DFPlayer_Mini_Mp3.h>

//#define S0
#define S1 2
#define S2 3
#define S3 4
#define OUT 5

#define BUTL 13
#define BUTM 6
#define BUTR 7

#define MOTOR 9

//#define DEBUG

enum locations {konfig  = 1, place1, place2, place3,placen}; 

int audio[9][4] = { {9 ,10,11,12},//sprav0
                    {13,14,15,16},//ter1
                    {17,18,19,20},//xer2
                    {1 ,2 ,3 ,4 },//lang3
                    {5 ,6 ,7 ,8 },//punk4
                    {21,22,23,24},//set5
                    {25,26,27,28},//back6
                    {29,30,31,32},//vol7
                    {33,34,35,36}};//pow8
                                      
int lang = 1;
int volume = 7;
int power = 7;

class Motor
{
private:
  int pin;
  int pwr;
  int t1 = 0;
  int t2 = 0;
  int flag = 1;
public:
  Motor(int _pin, int _pwr): pin(_pin), pwr(_pwr)
  {
    pinMode(pin, OUTPUT);
  }

  void charge(bool f, int p)
  {
    p = map(p, 1, 10 ,0, 255);
    digitalWrite(pin, f * p);                           /////////////////////////0
  }
  
  void chargeTime(int p)
  {
    if(t1 == 0 || pwr != p)
    {
      flag = 1;
      pwr = p;
      t1 = millis();
    }
    t2 = millis();  
    if(t2 - t1 < 500)
    {
      p = map(p, 1, 10, 0, 255);
    #ifdef DEBUG
      Serial.print("charge: ");
      Serial.println(p * flag);
    #endif    
      analogWrite(pin, flag * p);  
    }
    else if(flag == 1)
    {
      t1 = millis();
      flag = !flag;
    }
  }
};

class ColorSensor
{
private:
  int s1;
  int s2;
  int s3;
  int out;

  int red;
  int green;
  int blue;
public:
  ColorSensor(int _s1, int _s2, int _s3, int _out):s1(_s1), s2(_s2), s3(_s3), out(_out)
  {
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
    pinMode(_s3, OUTPUT);
    pinMode(_out, INPUT);
  } 

  bool isRed()
  {
    return (red > 0 && red < 200 && green > 100 && green < 300 && blue > 100 && blue < 300); 
  }

  bool isGreen()
  {
    return (red > 24 && red < 100 && green > 24 && green < 100 && blue > 24 && blue < 100); 
  }
  
  bool isBlue()
  {
    return (red > 170 && red < 320 && green > 100 && green < 250 && blue > 40 && blue < 140); 
  }
  
  void setColor()
  {
    digitalWrite(S2, LOW); 
    digitalWrite(S3, LOW); 

    red = pulseIn(OUT, LOW);
  #ifdef DEBUG
    Serial.print("red: "); 
    Serial.println(red); 
    Serial.println("------------------------"); 
  #endif
  //RED 

  //GREEN 
    digitalWrite(S2, HIGH); 
    digitalWrite(S3, HIGH); 

    green = pulseIn(OUT, LOW); 
  #ifdef DEBUG
    Serial.print("green: "); 
    Serial.println(green); 
    Serial.println("------------------------"); 
  #endif
  //GREEN 

  //BLUE
   
    digitalWrite(S2, LOW); 
    digitalWrite(S3, HIGH); 

    blue = pulseIn(OUT, LOW); 
  #ifdef DEBUG
    Serial.print("blue: "); 
    Serial.println(blue); 
    Serial.println("------------------------"); 
  #endif
    //BLUE
  }
};

class Menu
{
private:
  int count;
  int t1;
  int t2;
  const int number;
public:
  Menu(int n): count(1),
               number(n){}
  Menu(int n, int c): count(c),
                      number(n){}
                      
  bool operator==(const int c)
  {
    return 
      this -> count == c;  
  }

  void next()
  {
    if(++count > number)
      count = 1;
  }

  void prev()
  {
    if(--count < 1)
      count = number;
  }

  void nextDel()
  {
    if(!t1)
      t1 = millis();
    t2 = millis();  
    if(t2 - t1 > 200)
    {
      next();
      t1 = millis();
    }    
  }

  void prevDel()
  {
    if(!t1)
      t1 = millis();
    t2 = millis();
    if(t2 - t1 > 200)
    {
      prev();
      t1 = millis();  
    } 
  }

  int getCount()
  {
    return count;
  }
  void setCount(int c)
  {
    count = c;
  }
  void setCount()
  {
    count = 1;
  }

  void setTime()
  {
    t1 = 0;
    t2 = 0;
  }
};

class Button
{
private:
  int pin;
  int clck;
  int t1;
  int t2;
  bool lbs = LOW;
  int flag = -1;
public:
  Button(int p): pin(p)
  {
    pinMode(p, INPUT);
  }
  
  bool tap()
  {
    return digitalRead(pin);   
  }

 void clik1()
  {
    bool b = tap();
    if(b != lbs)
    {
      t1 = millis();
    }
    t2 = millis();
    if(t2 - t1 > 500 && b)
    {
      clck++;
    }
    if(t2 - t1 > 50 && !b)
    {
      clck = 0;
    }
    if(t2 - t1 > 50)
    {
      if(b != flag)
      {
        flag = b;

        if(flag)
        {
          clck++; 
        }
      }
    }
    lbs = b;
  }  
  
  void clik2()
  {  
    bool b = tap();
    if(b != lbs)
    {
      t1 = millis();
    }
    t2 = millis();
    if(t2 - t1 > 500 && b)
    {
      clck++;
    }
    if(t2 - t1 > 200 && !b)
    {
      clck = 0;
    }
    if(t2 - t1 > 50)
    {
      if(b != flag)
      {
        flag = b;

        if(flag)
        {
          clck++; 
        }
      }
    }
    lbs = b;
  }  
  int getClck()
  {  
    return clck;
  }
  void setTime()
  {
    t1 = 0;
    t2 = 0;
  }
};

Button butL(BUTL);
Button butM(BUTM);
Button butR(BUTR);

Menu menu(placen - 1);
Menu setting(3);
Menu setVol(10, 7);
Menu setPow(10, 7);
Menu setLan(4);

Motor motor(MOTOR, power);
ColorSensor colSen(S1, S2, S3, OUT);

int vol()
{
  return map(volume, 1 , 10, 3, 20); 
}

void setup() 
{  
  Serial.begin (9600);
  mp3_set_serial (Serial);    
  mp3_set_volume (vol());
  delay (100);
  mp3_play (99);
  delay (100);
}

bool flagL = true;
bool flagM = true;
bool flagR = true;

void start()
{
  //mp3_play(99);
  setL(lang);
  mp3_play(audio[4][lang - 1]);
}

void setMot(int n)
{
  setPow.setCount(n);
  while(1)
  {
  butL.clik1();
  butM.clik1();
  butR.clik1();

  if(butL.getClck() > 10)
  {
    setPow.prevDel();
  }
  
  if(butR.getClck() > 10)
  {
    setPow.nextDel();
  }

  if(butL.getClck() && flagL){
    setPow.prev();
    flagL = !flagL;          }
    
  if(butR.getClck() && flagR){
    setPow.next();
    flagR = !flagR;          }

  if(butM.getClck() && flagM){
     flagM = !flagM;
     return; 
  }
  
  if(!butL.getClck() && !flagL){
    flagL = !flagL;
    setPow.setTime();          }
  
  if(!butR.getClck() && !flagR){
    flagR = !flagR;
    setPow.setTime();          }
  
  if(!butM.getClck() && !flagM){
    flagM = !flagM;
    setPow.setTime();          }
  
  motor.chargeTime(setPow.getCount());

#ifdef DEBUG    
  Serial.print("Set: ");
  Serial.println(setPow.getCount());
#endif  
  }
}

void setL(int n)
{
  setLan.setCount(n);
  while(1)
  {
  butL.clik1();
  butM.clik1();
  butR.clik1();

  if(butL.getClck() > 10)
  {
    setLan.prevDel();
    lang = setLan.getCount();
  }
  
  if(butR.getClck() > 10)
  {
    setLan.nextDel();
    lang = setLan.getCount();
  }

  if(butL.getClck() && flagL){
    setLan.prev();
    lang = setLan.getCount();
    flagL = !flagL;          }
    
  if(butR.getClck() && flagR){
    setLan.next();
    lang = setLan.getCount();
    flagR = !flagR;          }

  if(butM.getClck() && flagM){
     flagM = !flagM;
     return; 
  }
  
  if(!butL.getClck() && !flagL){
    flagL = !flagL;
    setLan.setTime();          }
  if(!butR.getClck() && !flagR){
    flagR = !flagR;
    setLan.setTime();          }
  if(!butM.getClck() && !flagM){
    flagM = !flagM;
    setLan.setTime();          }

  if(n != lang)
  {
    n = lang;
    mp3_play(audio[3][n - 1]);
  }
#ifdef DEBUG    
  Serial.print("Set: ");
  Serial.println(setLan.getCount());
#endif
  }
}

void setV(int n)
{
  setVol.setCount(n);
  while(1)
  {
  butL.clik1();
  butM.clik1();
  butR.clik1();

  if(butL.getClck() > 10)
  {
    setVol.prevDel();
    volume = setVol.getCount();
  }
  
  if(butR.getClck() > 10)
  {
    setVol.nextDel();
    volume = setVol.getCount();
  }

  if(butL.getClck() && flagL){
    setVol.prev();
    volume = setVol.getCount();
    flagL = !flagL;          }
    
  if(butR.getClck() && flagR){
    setVol.next();
    volume = setVol.getCount();
    flagR = !flagR;          }

  if(butM.getClck() && flagM){
     flagM = !flagM;
     return; 
  }
  
  if(!butL.getClck() && !flagL){
    flagL = !flagL;
    setVol.setTime();          }
  if(!butR.getClck() && !flagR){
    flagR = !flagR;
    setVol.setTime();          }
  if(!butM.getClck() && !flagM){
    flagM = !flagM;
    setVol.setTime();          }

  if(n != volume)
  {
    mp3_set_volume(vol());
    delay(25);
    mp3_play(100);
    delay(25);
    n = volume;
  }
#ifdef DEBUG    
  Serial.print("Set: ");
  Serial.println(setVol.getCount());
#endif  
  }
}

void settings()
{
  setting.setCount();
#ifdef DEBUG
  Serial.print("settings: ");
  Serial.println(setting.getCount());
#endif
  mp3_play(audio[7][lang - 1]);
  int temp = 1;    
  while(1)
  {
  butL.clik1();
  butM.clik1();
  butR.clik1();

  if(butL.getClck() > 10)
    setting.prevDel();
  
  if(butR.getClck() > 10)
    setting.nextDel();

  if(butL.getClck() && flagL){
    setting.prev();
    flagL = !flagL;          }
  if(butR.getClck() && flagR){
    setting.next();
    flagR = !flagR;          }
  if(butM.getClck() && flagM)
  {
    flagM = !flagM;
    switch(setting.getCount())
    {
      case 1:{ setV(volume); break; }
      case 2:{ setMot(power); break; }
      case 3: return; 
    }
    volume = setVol.getCount();
    power = setPow.getCount();
  #ifdef DEBUG  
    Serial.print("volume: ");
    Serial.println(volume);
    Serial.print("power: ");
    Serial.println(power);
  #endif
  }

  if(!butL.getClck() && !flagL){
    flagL = !flagL;
    setting.setTime();         }
  if(!butR.getClck() && !flagR){
    flagR = !flagR;
    setting.setTime();         }
  if(!butM.getClck() && !flagM){
    flagM = !flagM;
    setting.setTime();         }

  if(temp != setting.getCount())
  {
    temp = setting.getCount();
    switch(temp)
    {
      case 1: { mp3_play(audio[7][lang - 1]); break; }
      case 2: { mp3_play(audio[8][lang - 1]); break; }
      case 3: { mp3_play(audio[6][lang - 1]); break; }
    }
  }
      
  #ifdef DEBUG
    Serial.print("settings: ");
    Serial.println(setting.getCount());
  #endif  
  }
}

void loop() 
{
  start();
  int temp = 1;
  
  while(1)
  {
  butL.clik1();
  butM.clik1();
  butR.clik1();

  if(butL.getClck() > 10)
    menu.prevDel();
  
  if(butR.getClck() > 10)
    menu.nextDel();
  
  if(butL.getClck() && flagL){
    menu.prev();
    flagL = !flagL;          }
  if(butR.getClck() && flagR){
    menu.next();
    flagR = !flagR;          }
  if(butM.getClck() && flagM)
  {
    flagM = !flagM;
  
    switch(menu.getCount())
    {
      case 1:{settings(); break;}
      default:{go( menu.getCount() ); break;}
    }
  }

  if(!butL.getClck() && !flagL){
    flagL = !flagL;
    menu.setTime();            }
  if(!butR.getClck() && !flagR){
    flagR = !flagR;
    menu.setTime();            }
  if(!butM.getClck() && !flagM){
    flagM = !flagM;
    menu.setTime();            }

  if(temp != menu.getCount())
  {
    temp = menu.getCount();
    if(temp == 1)
      mp3_play(audio[5][lang - 1]);
    else
      mp3_play(audio[temp-2][lang - 1]);
  }
    
#ifdef DEBUG
  Serial.print("Menu : ");    
  Serial.println(menu.getCount());
#endif  
  }
}

bool setMm(int n)
{
  butL.clik1();
  butM.clik1();
  butR.clik1();

  if(butL.getClck() > 10)
  {
    setPow.prevDel();
  }
  
  if(butR.getClck() > 10)
  {
    setPow.nextDel();
  }

  if(butL.getClck() && flagL){
    setPow.prev();
    flagL = !flagL;          }
    
  if(butR.getClck() && flagR){
    setPow.next();
    flagR = !flagR;          }

  if(butM.getClck() && flagM){
     flagM = !flagM;
     return false; 
  }
  
  if(!butL.getClck() && !flagL){
    flagL = !flagL;
    setPow.setTime();          }
  
  if(!butR.getClck() && !flagR){
    flagR = !flagR;
    setPow.setTime();          }
  
  if(!butM.getClck() && !flagM){
    flagM = !flagM;
    setPow.setTime();          }
    
  power = setPow.getCount();
  
#ifdef DEBUG    
  Serial.print("SetMm: ");
  Serial.println(setPow.getCount());
#endif
  return true;  
}

void go(int n)
{
  switch(n)
  {
    case place1:
    {
      while(setMm(power))
      {
        colSen.setColor();
        motor.charge(!colSen.isRed(), power);  
      }
      break;
    }
    case place2:
    {
      while(setMm(power))
      {
        colSen.setColor();
        motor.charge(!colSen.isGreen(), power);  
      }
      break; 
    }
    case place3:
    {
      while(setMm(power))
      {
        colSen.setColor();
        motor.charge(!colSen.isBlue(), power);  
      } 
      break; 
    }
  }
  motor.charge(0, power);
}

