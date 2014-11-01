 
static void my_result_cb (byte status, word off, word len) {
  Serial.print("<<< reply ");
  Serial.print(millis() - Ethernet_timer);
  Serial.println(" ms");
  Serial.println((const char*) Ethernet::buffer + off);
}
