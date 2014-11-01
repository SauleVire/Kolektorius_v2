//float dallas(int x) // jei davikliai ant atskiru isvadu
float Dallas (byte addr[8])
{
//OneWire ds(x);// jei davikliai ant atskiru isvadu
  //returns the temperature from one DS18S20 in DEG Celsius
  byte data[12];
//  byte addr[8];// jei davikliai ant atskiru isvadu
//  float result;
 // if ( !ds.search(addr)) 
 // {   ds.reset_search();  return -99;  }// no more sensors on chain, reset search
  if ( OneWire::crc8( addr, 7) != addr[7]) 
  {   return -88; }//CRC is not valid!
  if ( addr[0] != 0x28) 
  {   return -77; }// Device is not recognized
  

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion
  delay(850); // Wait some time...
  
  ds.reset();   // byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad command

  for (int i = 0; i < 9; i++)  // Receive 9 bytes
  { 
    data[i] = ds.read();
  }
  
 // ds.reset_search();
  
  // memory consuming but readable code:
/*  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;  // 1/16 = 0.0625
  return TemperatureSum;
  
    
  return ( (data[1] << 8) + data[0] )*0.0625 ;
 */
   unsigned int raw = (data[1] << 8) | data[0];
       byte cfg = (data[4] & 0x60);
       return raw / 16.0;
}
