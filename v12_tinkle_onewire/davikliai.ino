

 void Temperaturu_matavimas_1(){
  //____________________________ Start Sensor 1 _________________________________
#ifdef SetWaitForConversionFALSE
  K_sensor.setWaitForConversion(false);  // makes it async
#endif
  K_sensor.requestTemperatures(); // Send the command to get temperatures
  K=K_sensor.getTempCByIndex(0);
//_____________________________ Stop Sensor 1 ___________________________________
  //______________________ Start Sensor 3 ________________________________________
  #ifdef SetWaitForConversionFALSE
  T_sensor.setWaitForConversion(false);  // makes it async
#endif
  T_sensor.requestTemperatures(); // Send the command to get temperatures
T=T_sensor.getTempCByIndex(0);
//___________________ Stop Sensor 3 ______________________________________________
//__________________________________________ Start Sensor 2 _____________________
#ifdef SetWaitForConversionFALSE
  B_sensor.setWaitForConversion(false);  // makes it async
#endif
  B_sensor.requestTemperatures(); // Send the command to get temperatures
  B=B_sensor.getTempCByIndex(0);
//_____________________________________ Stop Sensor 2 ____________________________
}



