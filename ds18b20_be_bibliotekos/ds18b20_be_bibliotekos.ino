/*ReadDS18B20two
ver: 18 Jly 2010
Started end of term eve WG/TA/LE/EW/JGB

Reading two DS18B20s

See...

http://sheepdogguides.com/arduino/ar3ne1tt2.htm

... for explanation of this code.

Code adapted from code from nuelectronics.com demo*/

#define tture1 9//no ; here
#define tture2 2//no ; here
#define tture3 A3//no ; here
float K,B,T;
/*Forward declarations. Only the last two need concern the user
Remmed out, as they seem unnecessary
void OneWireReset(int Pin);//Called by readTture
void OneWireOutByte(int Pin, byte d);//Called by readTture
byte OneWireInByte(int Pin);//Called by readTture

void readTture(byte Pin);//Of use to users
void printTture();//Of use to users
*/

//Following globals used to communicate results back
//from readTture(Pin), and to send data to printTture...
int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

void setup() {
    //For each tture sensor: Do a pinMode and a digitalWrite
    pinMode(tture1, INPUT);
    pinMode(tture2, INPUT);

    digitalWrite(tture1, LOW);//Disable internal pull-up.
    digitalWrite(tture2, LOW);

    Serial.begin(9600);
    delay(300);//Wait for newly restarted system to stabilize
//    Serial.print("Temperature measurement, two sensors:\n\n");
}

void loop(){

K=  readTture(tture1);//N.B.: Values passed back in globals
  printTture();//N.B.: Takes values from globals. Also...
     //no newline part of pritTture;
//  Serial.print("   ");
//  delay(120);// Delay... must not be too short.
B=  readTture(tture2);//Now read and report 2nd tture.
  printTture();
//  delay(200);// Delay... must not be too short.
//  Serial.print("\n");//Start new line
T=  readTture(tture3);//Now read and report 2nd tture.
  printTture();
}


//Everything below here... just copy it into your program "as is".
//You are only likely to need to use readTture(pin) and printTture()
//   directly. Others are subordinate to those.
//These routine access the following global variables...
//   int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;
void OneWireReset(int Pin) // reset.  Should improve to act as a presence pulse
{
     digitalWrite(Pin, LOW);
     pinMode(Pin, OUTPUT); // bring low for 500 us
     delayMicroseconds(500);
     pinMode(Pin, INPUT);
     delayMicroseconds(500);
}//end OneWireReset

void OneWireOutByte(int Pin, byte d) // output byte d (least sig bit first).
{
   byte n;
   for(n=8; n!=0; n--)
   {
      if ((d & 0x01) == 1)  // test least sig bit
      {
         digitalWrite(Pin, LOW);
         pinMode(Pin, OUTPUT);
         delayMicroseconds(5);
         pinMode(Pin, INPUT);
         delayMicroseconds(60);
      }
      else
      {
         digitalWrite(Pin, LOW);
         pinMode(Pin, OUTPUT);
         delayMicroseconds(60);
         pinMode(Pin, INPUT);
      }
      d=d>>1; // now the next bit is in the least sig bit position.
   }
}//end OneWireOutByte

byte OneWireInByte(int Pin) // read byte, least sig byte first
{
    byte d, n, b;
    for (n=0; n<8; n++)
    {
        digitalWrite(Pin, LOW);
        pinMode(Pin, OUTPUT);
        delayMicroseconds(5);
        pinMode(Pin, INPUT);
        delayMicroseconds(5);
        b = digitalRead(Pin);
        delayMicroseconds(50);
        d = (d >> 1) | (b<<7); // shift d to right and insert b in most sig bit position
    }
    return(d);
}//end OneWireInByte

float readTture(byte Pin){
//Pass WHICH pin you want to read in "Pin"
//Returns values in... (See global declarations)
  OneWireReset(Pin);
  OneWireOutByte(Pin, 0xcc);
  OneWireOutByte(Pin, 0x44); // perform temperature conversion, strong pullup for one sec

  OneWireReset(Pin);
  OneWireOutByte(Pin, 0xcc);
  OneWireOutByte(Pin, 0xbe);

  LowByte = OneWireInByte(Pin);
  HighByte = OneWireInByte(Pin);
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

  Whole = Tc_100 / 100;  // separate off the whole and fractional portions
  Fract = Tc_100 % 100;
};//end readTture

void printTture(){//Uses values from global variables.
//See global declarations.
//N.B.: No new line inside printTture
if (SignBit) // If it's negative
    {
     Serial.print("-");
    };
  Serial.print(Whole);
  Serial.print(".");
  if (Fract < 10)
    {
     Serial.print("0");
    };
  Serial.print(Fract);
};//end printTture
