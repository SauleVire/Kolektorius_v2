void K_Temperatura(void){
  K_t.reset();
  K_t.skip();
  // start conversion and return
  if (!(Conv_start1)){
    K_t.write(0x44,0);
    Conv_start1 = true;
    return;
  }
  // check for conversion if it isn't complete return if it is then convert to decimal
  if (Conv_start1){
    Busy1 = K_t.read_bit();
    if (Busy1 == 0){
      return;
    }
    K_t.reset();
    K_t.skip();
    K_t.write(0xBE);  
    for ( int i = 0; i < 2; i++) {           // we need 2 bytes
      data1[i] = K_t.read();
    } 
    unsigned int raw = (data1[1] << 8) + data1[0];

    K = (raw & 0xFFFC) * 0.0625; 
    Conv_start1 = false;
    return;
  } 

}
void B_Temperatura(void){
  B_t.reset();
  B_t.skip();
  // start conversion and return
  if (!(Conv_start2)){
    B_t.write(0x44,0);
    Conv_start2 = true;
    return;
  }
  // check for conversion if it isn't complete return if it is then convert to decimal
  if (Conv_start2){
    Busy2 = B_t.read_bit();
    if (Busy2 == 0){
      return;
    }
    B_t.reset();
    B_t.skip();
    B_t.write(0xBE);  
    for ( int i = 0; i < 2; i++) {           // we need 2 bytes
      data2[i] = B_t.read();
    } 
    unsigned int raw = (data2[1] << 8) + data2[0];

    B = (raw & 0xFFFC) * 0.0625; 
    Conv_start2 = false;
    return;
  } 

}
void T_Temperatura(void){
  T_t.reset();
  T_t.skip();
  // start conversion and return
  if (!(Conv_start3)){
    T_t.write(0x44,0);
    Conv_start3 = true;
    return;
  }
  // check for conversion if it isn't complete return if it is then convert to decimal
  if (Conv_start3){
    Busy3 = T_t.read_bit();
    if (Busy3 == 0){
      return;
    }
    T_t.reset();
    T_t.skip();
    T_t.write(0xBE);  
    for ( int i = 0; i < 2; i++) {           // we need 2 bytes
      data3[i] = T_t.read();
    } 
    unsigned int raw = (data3[1] << 8) + data3[0];

    T = (raw & 0xFFFC) * 0.0625; 
    Conv_start3 = false;
    return;
  } 

}
