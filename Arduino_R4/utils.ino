unsigned long getNTPTime() {
  WiFi.hostByName("pool.ntp.org", timeServerIP);

  memset(packetBuffer, 0, 48);
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode

  Udp.beginPacket(timeServerIP, 123);
  Udp.write(packetBuffer, 48);
  if (Udp.endPacket() == 0) return 0;  // Send failed

  // Wait up to 1500ms for a response
  int retry = 0;
  while (Udp.parsePacket() == 0 && retry < 15) {
    delay(100);
    retry++;
  }

  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, 48);
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    return secsSince1900 - 2208988800UL;
  }
  return 0;
}