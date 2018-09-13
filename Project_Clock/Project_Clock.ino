#include "Wire.h"
#define  I2C_ADDRESS 0x68  // the I2C address of Tiny RTC
#include <Keypad.h>

unsigned long previousMillis = 0;        // will store last time delay
const long interval = 1000;           // interval at which to blink (milliseconds)

const byte ROWS = 4;           // Four rows
const byte COLS = 3;          // Three columns
char keys[ROWS][COLS] = {     // Define the Keymap
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = { 11, 10, 9, 8 };       // Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte colPins[COLS] = { 7, 6, 5};         // Connect keypad COL0, COL1 and COL2 to these Arduino pins.
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

  int dataMin=4;
  int latchMin=2;
  int clckMin=3;

  int dataHr=12;
  int latchHr=A0;
  int clckHr=13;

  int clockPin = A2;  // pin 11 of shift register
  int dataPin = A1;   // pin 14 of shift register
  int latchPin = A3;// pin 12 of shift register
  
  byte second, minute, hour; 
       
  void getTime()       // Function to gets the date and time from the ds1307 and prints result
    {
       Wire.beginTransmission(I2C_ADDRESS);      // Reset the register pointer
       Wire.write(decToBcd(0));
       Wire.endTransmission();
       
       Wire.requestFrom(I2C_ADDRESS, 3);
       second     = bcdToDec(Wire.read() & 0x7f);
       minute     = bcdToDec(Wire.read());
       hour       = bcdToDec(Wire.read() & 0x3f);  // Need to change this if 12 hour am/pm
       Serial.print(hour);
       Serial.print(":");
       Serial.print(minute);
       Serial.print(":");
       Serial.print(second);
       Serial.println();
       showTime(hour,minute);
       wregHr();
       wregMin();
    }
    
 void setup() 
  {
    pinMode(dataMin,OUTPUT);
    pinMode(latchMin,OUTPUT);
    pinMode(clckMin,OUTPUT);
    
    pinMode(dataHr,OUTPUT);
    pinMode(latchHr,OUTPUT);
    pinMode(clckHr,OUTPUT);
    
    pinMode(clockPin,OUTPUT);
    pinMode(dataPin,OUTPUT);
    pinMode(latchPin,OUTPUT);
    
    Wire.begin();
    Serial.begin(19200);
  }
    
void loop() 
{
  unsigned long currentMillis = millis();
  char key = kpd.getKey();   
  Serial.print(key);
  if(key != NO_KEY)
   {
     if(key=='*')
     {
       Serial.println(key);
       key=kpd.waitForKey();
        if(key=='#')
         {
          Serial.println(key);
          setTime();      //Set current time;
         }
      }
   }
      
   if (currentMillis - previousMillis >= interval)   // save the last time you blinked the LED
   {    
   previousMillis = currentMillis;
   getTime();               //get the time data from tiny RTC
   }
   wregHr();
   wregMin();
}

byte decToBcd(byte val)
{
        return ( (val/10*16) + (val%10) );     // Convert normal decimal numbers to binary coded decimal
}

byte bcdToDec(byte val)
{
        return ( (val/16*10) + (val%16) );      // Convert binary coded decimal to normal decimal numbers
}


void setTime()        // Function to set the currnt time, change the second&minute&hour to the right time
{
   char key;
   Serial.print("Enter Time HHMM");
   int hr= getKeyData()*10 + getKeyData();
   int mnt= getKeyData()*10 + getKeyData();
   Serial.print("Set time:- ");
   Serial.print(hr);
   Serial.print(":");
   Serial.println(mnt);
   Serial.println("If yes press #");
   key=kpd.waitForKey();
    if(key=='#')
     {
        second = 0; 
        minute = mnt;
        hour  =  hr;
        Wire.beginTransmission(I2C_ADDRESS);
        Wire.write(decToBcd(0));
        Wire.write(decToBcd(second));    // 0 to bit 7 starts the clock
        Wire.write(decToBcd(minute));
        Wire.write(decToBcd(hour));      // If you want 12 hour am/pm you need to set
                                        // bit 6 (also need to change readDateDs1307)
        Wire.endTransmission();
      }
     else
      {
         setTime();
      }
       showTime(hour,minute);
       wregHr();
       wregMin();
}

int getKeyData()
{
   char key = kpd.waitForKey();
    if(key=='1')
     {
       return(1);
     }
    else if (key=='2')
     {
       return(2);
     }
    else if (key=='3')
     {
       return(3);
     }
    else if (key=='4')
     {
       return(4);
     }
    else if (key=='5')
     {
       return(5);
     }
    else if (key=='6')
     {
       return(6);
     }
    else if (key=='7')
     {
       return(7);
     }
    else if (key=='8')
     {
       return(8);
     }
    else if (key=='9')
     {
       return(9);
     }
    else if (key=='0')
     {
       return(0);
     }
    else
     {
       getTime();
     }
}

boolean regHr[24];
boolean regMin[60];


void wregMin()
{
  digitalWrite(latchMin,LOW);
  for(int i=59;i>=0;i--)
  {
    digitalWrite(clckMin,LOW);
    digitalWrite(dataMin,regMin[i]);
    digitalWrite(clckMin,HIGH);
  }
  digitalWrite(latchMin,HIGH);
}

void wregHr()
{
  digitalWrite(latchHr,LOW);
  for(int i=23;i>=0;i--)
  {
    digitalWrite(clckHr,LOW);
    digitalWrite(dataHr,regHr[i]);
    digitalWrite(clckHr,HIGH);
  }
  digitalWrite(latchHr,HIGH);
}

void showTime(int hr, int minu)
{ 
  char tim;
  empReg();
  int pin;
  if(hr>=12)
  {
    hr=hr-12;
    tim='P';
  }
  else
  {
    tim='A';
  }
  pin=hr*5;
  hr=hr*2;
  for(int i=1;i<=minu;i++)
    {
      regMin[i]=HIGH;
    }
    regMin[pin]=HIGH;
    wregMin();
 
    regHr[hr]=HIGH;
    regHr[hr+1]=HIGH;
    wregHr();
    Disp(hr,minu,tim);
}

void Disp(int hr,int minu,char tim)
{
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);   // Display A on 7-segment Display
  digitalWrite(latchPin, HIGH);
  hr=hr/2;
  
  Segment(tim);
  
  Segment(minu%10);
  minu=minu/10;
  Segment(minu);

  if(hr==0)
  {
    Segment(2);
    Segment(1);
  }
  else
  {
    Segment(hr%10);
    hr=hr/10;
    Segment(hr);
  }
}

void empReg()
{
    for(int i=0;i<60;i++)
    {
      regMin[i]=LOW;
    }
    for(int i=0;i<24;i++)
    {
      regHr[i]=LOW;
    }
}

void Segment(char num)
{
  if(num=='P')
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 31);   // Display P on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num=='A')
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 95);   // Display A on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
}

void Segment(int num)
{
   if(num==1)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 72);   // Display 1 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==2)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 61);   // Display 2 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==3)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 109);   // Display 3 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==4)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 75);   // Display 4 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==5)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 103);   // Display 5 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==6)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 115);   // Display 6 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==7)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 76);   // Display 7 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==8)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 127);   // Display 8 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==9)
     {
       digitalWrite(latchPin, LOW);
       shiftOut(dataPin, clockPin, MSBFIRST, 79);   // Display 9 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
    else if (num==0)
     {
       digitalWrite(latchPin, LOW);                                
       shiftOut(dataPin, clockPin, MSBFIRST, 126);   // Display 0 on 7-segment Display
       digitalWrite(latchPin, HIGH);
     }
}

