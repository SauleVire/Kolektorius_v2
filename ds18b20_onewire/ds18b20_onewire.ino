
#include <OneWire.h>
const byte WATER_TEMP_PIN=2;
OneWire ds(WATER_TEMP_PIN);
byte addr[8];
void setup() {
  Serial.begin(9600);

  Serial.print(F("Free Ram: "));
  Serial.println(freeRam());

  //Set up Water Temp sensor - there is only one 1 wire sensor
  if ( !ds.search(addr)) {
    Serial.println(F("---> ERROR: Did not find the DS18B20 Water Temperature Sensor!"));
    return;
  }
  else {
    Serial.print(F("DS18B20 ROM ="));
    for(byte i = 0; i < 8; i++) {
      Serial.write(' ');
      Serial.print(addr[i], HEX);
    }
    Serial.println();
  }

}
void loop() {

  uint16_t value;

    value = getWaterTemperature();
    if (value == 85) {//I've noticed 85 is the value I get when the sensor isn't working..but could be a correct reading
      Serial.println(F("WARNING: The water temperature sensor is returning 85.  This might mean the sensor is not working correctly!"));

    }
    Serial.print(F("---> Water Temp: "));
    Serial.println(value);
}
/******************************************************************************
 * return celsius reading of water temmperature
 *******************************************************************************/
uint16_t getWaterTemperature() {
  //  byte data[12];
  byte data[12];
  byte addr[8];
   if ( !ds.search(addr)) 
 {   ds.reset_search();  return -99;  }// no more sensors on chain, reset search

  ds.reset();
  ds.select(addr);
  //see the DS18B20 data sheet for what the commands mean. http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf
  ds.write(0x44,1); // read temperature and store it in the scratchpad
  ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read the water temp from the scratchpad
  for ( byte i = 0; i < 12; i++) {           
    data[i] = ds.read();
  }
  int16_t raw = (data[1] << 8) | data[0];
  //I then divide the result by bit shifting instead of / 16.
  uint16_t celsius = raw >> 4;
  return celsius;
}


int freeRam ()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
