void menu_compute() {
  //MAIN MENU
  if (menu_index == 0) {
    menu_max = 3;
    u8g2.clearBuffer();
    uint8_t i = 0;
    u8g2.setFont(u8g2_font_9x18_tf);
    //u8g.setFont(u8g_font_fub17);
    u8g2.drawStr(0, 17, "Main Menu");
    u8g2.drawLine(0, 19, 127, 19);

    u8g2.drawBox(0, menu_pos * 10 + 22, 6, 6); // draw cursor bar
    for ( i; i < 4; i++ ) {       // draw all menu items
      // current selected menu item
      u8g2.drawStr(10, 31 + i * 10 , menu_strings[i]);
    }

    u8g2.sendBuffer();

  }

  //FAVORITE CHANNEL (5880)
  if (menu_index == 1 && menu_page == 1) {

    menu_page = 2;
    menu_pos = 31; //5880 F8 Channel
  }

  //ALL CHANNELS
  if (menu_index == 1 && menu_page == 2) {
    menu_max = 39;
    fly_loop(6, menu_pos);
    menu_update = true;
    return;
  }

  //BAND SELECTION
  if (menu_index == 1 && menu_page == 3) {
    menu_max = 4;
    menu_band = menu_pos;

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_9x18_tf);
    u8g2.drawStr(0, 17, "Band Selection");
    u8g2.drawLine(0, 19, 127, 19);
    u8g2.drawStr(10, 41, "Band");
    u8g2.drawStr(49, 41, band_strings[menu_pos]);

    u8g2.sendBuffer();
  }

  //SPECTRUM SCANNER
  if (menu_index == 1 && menu_page == 4) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_9x18_tf);
    u8g2.setCursor(0, 10);
    u8g2.print(F("SPECTRUM"));

    while (menu_index == 1) {

      for (int i = CHANNEL_MIN; i < CHANNEL_MAX; i++) {

        uint8_t channelIndex = channelList[i]; //retrive the value based on the freq order
        rx5808B.setFrequency(channelFreqTable[channelIndex]);
        delay(30);
        uint16_t rssi_norm = constrain(rx5808B.readRSSI(), rssiB_min, rssiB_max);

        rssi_norm = map(rssi_norm, rssiB_min, rssiB_max, 1, 40);

        u8g2.setDrawColor(0);
        u8g2.drawVLine(5 + 3 * i, 20, 40);

        u8g2.setDrawColor(1);
        u8g2.drawVLine(5 + 3 * i, 20 + 40 - rssi_norm, rssi_norm);
        u8g2.sendBuffer();
      }

      if (button_pressed)
        timerPress();

      delay(10);
    }
    menu_update = true;
    return;

  }

  //BAND CHANNELS
  if (menu_index == 2) {
    menu_max = 7;
    fly_loop(menu_band, menu_pos);
    menu_update = true;
    return;
  }

  menu_update = false;
  return;
} //end menu_compute

void oled_splash() {

  u8g2.clearBuffer();

  u8g2.setFont(u8g_font_8x13);

  u8g2.drawStr( 0, 20, "Video Receiver");
  u8g2.drawStr( 0, 35, "Version 4.20");

  u8g2.setFont(u8g2_font_6x10_tr);
  sprintf (j_buf, "RSSI MIN %d", rssiA_min); //Rssi min
  u8g2.drawStr(0, 50, j_buf);

  sprintf (j_buf, "RSSI MAX %d", rssiA_max); //Rssi max
  u8g2.drawStr(0, 60, j_buf);

  sprintf (j_buf, "%d", rssiB_min); //Rssi min
  u8g2.drawStr(84, 50, j_buf);

  sprintf (j_buf, "%d", rssiB_max); //Rssi max
  u8g2.drawStr(84, 60, j_buf);

  u8g2.sendBuffer();

  delay(2000);

}
