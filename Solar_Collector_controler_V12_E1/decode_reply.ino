int get_reply_data(word off)
{
  memset(line_buf,NULL,sizeof(line_buf));
  if (off != 0)
  {
    uint16_t pos = off;
    int line_num = 0;
    int line_pos = 0;
    
    // Skip over header until data part is found
    while (Ethernet::buffer[pos]) {
      if (Ethernet::buffer[pos-1]=='\n' && Ethernet::buffer[pos]=='\r') break;
      pos++; 
    }
    pos+=2;
    while (Ethernet::buffer[pos])
    {
      if (line_pos<49) {line_buf[line_pos] = Ethernet::buffer[pos]; line_pos++;} else break;
      pos++; 
    }
    line_buf[line_pos] = '\0';
    return line_pos;
  }
  return 0;
}

static void my_callback (byte status, word off, word len) {

  // get_reply_data gets the data part of the reply and puts it in the line_buf char array
  int lsize =   get_reply_data(off);
  
  Serial.print("Feed value: ");

  int value = atoi(line_buf);
  Serial.println(value);
  
//  if (value>0)
//  {
//    digitalWrite(redLED,LOW);            
//    digitalWrite(greenLED,LOW); 
//  }
//  else
//  {
//    digitalWrite(redLED,HIGH);            
//    digitalWrite(greenLED,HIGH);  
//  }
  
}

