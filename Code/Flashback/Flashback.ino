// Flashback project version 0.6
// Created by Edlang90 
// Based on code from MikyM0use: https://github.com/MikyM0use/JAFaR
// and modified Richwave library from Spacefish: https://github.com/Spacefish/RichWave

#include <RichWave.h>
#include <EEPROM.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//CONSTANT INITIALIZATION
//#define DISABLE_FILTERING

#define CHANNEL_MIN 0
#define NUM_BANDS 5
#define CHANNEL_MAX 40

#define EEPROM_ADR_RSSI_MIN_A 0x10
#define EEPROM_ADR_RSSI_MAX_A 0x12

#define EEPROM_ADR_RSSI_MIN_B 0x14
#define EEPROM_ADR_RSSI_MAX_B 0x16

#define RX_A 1
#define RX_B 0

#define spiDataPin PB15
#define spiClockPin PB13

#define spiLatchA PB12//receiver A SPI Latch pin
#define rssiA PB0  //RSSI A input

#define spiLatchB PB11//receiver B SPI Latch pin
#define rssiB PB1  //RSSI B input

#define SW_CTRL PA8

#define up PB5
#define ent PB6
#define dn PB7

#define delay_time 150

#define SELECT_A {digitalWrite(SW_CTRL, LOW);}
#define SELECT_B {digitalWrite(SW_CTRL, HIGH);}

#define RX_HYST 0 //~10%


//RichWave Implementation
RichWave rx5808A(spiDataPin, spiLatchA, spiClockPin, rssiA);
RichWave rx5808B(spiDataPin, spiLatchB, spiClockPin, rssiB);

U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ PA4, /* dc=*/ PA3, /* reset=*/ PA2);

//VARIABLE INITIALIZATION
// Channels with their Mhz Values
static const uint16_t channelFreqTable[] = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917  // Raceband
};

// All Channels of the above List ordered by Mhz
static const uint8_t channelList[] = {
  19, 32, 18, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
};

char j_buf[40];
const char *menu_strings[] = { "5880 MHz", "Channels", "Band Sel", "Spectrum" };
const char *band_strings[] = { "A", "B", "E", "F", "R" };
const char *chan_strings[] = {
  "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
  "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8",
  "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8",
  "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
  "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
};

uint16_t scanVecA[CHANNEL_MAX];
uint16_t scanVecB[CHANNEL_MAX];

uint16_t menu_pos = 0;
uint8_t menu_index = 0;
uint8_t menu_page = 0;
uint8_t menu_max = 3; //sets max amount of items per menu (menu_max = "total menu items" - 1)
uint8_t menu_band = 0;

uint16_t rssiA_min, rssiA_max, rssiB_min, rssiB_max;

volatile bool menu_update = true;
bool flyMode = false;

volatile bool button_pressed = false;
volatile unsigned long heldTime = 0;
volatile unsigned long timer = 0;


void setup() {
  u8g2.begin();

  digitalWrite(up, HIGH);
  digitalWrite(dn, HIGH);

  pinMode(up, INPUT_PULLUP); //UP
  pinMode(ent, INPUT_PULLUP); //ENTER
  pinMode(dn, INPUT_PULLUP); //DOWN

  pinMode(SW_CTRL, OUTPUT);
  SELECT_B;

  rssiA_min = (EEPROM.read(EEPROM_ADR_RSSI_MIN_A));
  rssiA_max = (EEPROM.read(EEPROM_ADR_RSSI_MAX_A));
  rssiB_min = (EEPROM.read(EEPROM_ADR_RSSI_MIN_B));
  rssiB_max = (EEPROM.read(EEPROM_ADR_RSSI_MAX_B));

  delay(300);

  if (digitalRead(up) == 0 && digitalRead(dn) == 0) {
    calibration(RX_A);
    delay(100);
    calibration(RX_B);
  }

  attachInterrupt(digitalPinToInterrupt(ent), enterISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(up), upISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(dn), upISR, FALLING);

  oled_splash();
}

void loop() {

  if (menu_update) {
    menu_pos = readSwitch();
    menu_compute();
  }

  if (button_pressed)
    timerPress();

  delay(delay_time);
}//end loop


inline uint16_t readSwitch() {
  int but_up = digitalRead(up); //consider PIN Register mapping?
  int but_down = digitalRead(dn);

  if (but_up == 0) {
    if (menu_pos == menu_max)
      menu_pos = 0;
    else
      menu_pos++;
  }

  if (but_down == 0) {
    if (menu_pos == 0)
      menu_pos = menu_max;
    else
      menu_pos--;
  }

  return menu_pos;
}

void enterISR() {
  volatile int but_ent = digitalRead(ent);
  if (but_ent == LOW) {
    button_pressed = true;
    heldTime = millis();
  }
  else {
    button_pressed = false;
    if (timer < 500)
      menu_in();
    timer = 0;
  }
}

void upISR() {
  menu_update = true;
}

void timerPress() {
  timer = millis() - heldTime;
  if (timer >= 500) {
    button_pressed = false;
    menu_out();
  }
}

void menu_in() {
  if (menu_index == 1 && menu_page == 2)
    autoScan();
  else {
    if (menu_index < 1 && menu_page != 3)
      menu_index++;
    if (menu_index < 2 && menu_page == 3)
      menu_index++;

    menu_page = (menu_pos + 1) * menu_index;
    menu_pos = 0;
  }
  menu_update = true;
}

void menu_out() {
  if (menu_index == 1) {
    menu_page = 0;
    menu_index = 0;
  }

  if (menu_index == 2) {
    menu_page = 3;
    menu_index = 1;
  }

  flyMode = false;
  menu_pos = 0;
  menu_update = true;
}

void autoScan() {
  // scan(RX_B);
  menu_pos = getMaxPos();

  delay(30);

  //menu_update = true;
}





void fly_loop(uint8_t band, uint8_t menu_pos) {
  int16_t rssi_b = 0, rssi_a = 0, rssi_b_norm = 0, rssi_a_norm = 0, prev_rssi_b_norm = 0, prev_rssi_a_norm = 0, rssi_a_const = 0, rssi_b_const = 0, global_max_rssi;
  uint8_t current_rx;

  if (band == 6) {

    rx5808B.setFrequency(channelFreqTable[menu_pos]); //set the selected freq
    SELECT_B;
    current_rx = RX_B;

    // delay(30);
    rx5808A.setFrequency(channelFreqTable[menu_pos]);
  }

  else {

    rx5808B.setFrequency(channelFreqTable[(8 * band) + menu_pos]); //set the selected freq
    SELECT_B;
    current_rx = RX_B;

    // delay(30);
    rx5808A.setFrequency(channelFreqTable[(8 * band) + menu_pos]); //set the selected freq
  }

  global_max_rssi = max(rssiA_max, rssiB_max);

  flyMode = true;
  menu_update = true;

  //MAIN FLY LOOP
  while (flyMode) {

    //rssi_a = rx5808A.readRSSI();

    rssi_a_const = constrain(rx5808A.readRSSI(), rssiA_min, rssiA_max);
    rssi_a_norm = map(rssi_a_const, rssiA_min, rssiA_max, 1, global_max_rssi);


   // rssi_b = rx5808B.readRSSI();

    rssi_b_const = constrain(rssi_b = rx5808B.readRSSI(), rssiB_min, rssiB_max);
    rssi_b_norm = map(rssi_b_const, rssiB_min, rssiB_max, 1, global_max_rssi);

    //filter... thanks to A*MORALE!
    //alpha * (current-previous) / 2^10 + previous
    //alpha = dt/(dt+1/(2*PI *fc)) -> (0.0002 / (0.0002 + 1.0 / (2.0 * 3.1416 * 10))) = 01241041672 * 2^11 -> 25
    //dt = 200us
    //fc = 8HZ
    //floating point conversion 10 bit > shift 2^10 -> 1024


#ifndef DISABLE_FILTERING
#define ALPHA 25
    int16_t rssi_b_norm_filt = ((ALPHA * (rssi_b_norm - prev_rssi_b_norm)) / 1024) + prev_rssi_b_norm;
    int16_t rssi_a_norm_filt = ((ALPHA * (rssi_a_norm - prev_rssi_a_norm)) / 1024) + prev_rssi_a_norm;
#else
    int16_t rssi_b_norm_filt  = (rssi_b_norm + prev_rssi_b_norm * 3) / 4;
    int16_t rssi_a_norm_filt  = (rssi_a_norm + prev_rssi_a_norm * 3) / 4;
#endif

    prev_rssi_b_norm = rssi_b_norm_filt;
    prev_rssi_a_norm = rssi_a_norm_filt;

    if (menu_update == true) {

      menu_pos = readSwitch();

      if (band == 6) {
        rx5808B.setFrequency(channelFreqTable[menu_pos]); //set the selected freq
        SELECT_B;
        current_rx = RX_B;

        //delay(30);
        rx5808A.setFrequency(channelFreqTable[menu_pos]);
      }

      else {
        rx5808B.setFrequency(channelFreqTable[ (8 * band) + menu_pos]); //set the selected freq
        SELECT_B;
        current_rx = RX_B;

        // delay(30);
        rx5808A.setFrequency(channelFreqTable[ (8 * band) + menu_pos]); //set the selected freq
      }

      u8g2.clearBuffer();
      //u8g2.setDrawColor(1);
      u8g2.setFont(u8g_font_9x18);

      if (band == 6) {
        u8g2.setFont(u8g2_font_inb49_mr);
        u8g2.drawStr(1, 50, chan_strings[menu_pos]);
        u8g2.setFont(u8g_font_9x18);
        u8g2.drawFrame(80, 13, 41, 14);
        u8g2.drawBox(81, 14, menu_pos, 12);

        sprintf (j_buf, "%d", (int)channelFreqTable[menu_pos]);
        u8g2.drawStr(80, 40, j_buf);
      }

      else {
        u8g2.setFont(u8g2_font_inb49_mr);
        u8g2.drawStr(1, 50, chan_strings[(8 * band) + menu_pos]);
        u8g2.setFont(u8g_font_9x18);
        u8g2.drawStr(80, 25, "Band ");
        u8g2.drawStr(119, 25, band_strings[band]);

        sprintf (j_buf, "%d", (int)channelFreqTable[(8 * band) + menu_pos]);
        u8g2.drawStr(80, 40, j_buf);
      }

      u8g2.sendBuffer();

      u8g2.setFont(u8g2_font_u8glib_4_tr);

      u8g2.drawFrame(6, 54, 120, 3);
      u8g2.drawFrame(6, 59, 120, 3);


      delay(delay_time);
      menu_update = false;

    } //end menu update if


    if (current_rx == RX_A) {
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 57, 6, 6); //B box

      u8g2.setDrawColor(1);
      u8g2.drawStr(1, 62, "B");
      u8g2.drawBox(0, 52, 6, 6); //A box

      u8g2.setDrawColor(0);
      u8g2.drawStr(1, 57, "A");

      u8g2.drawHLine(7, 55, 118);
      u8g2.drawHLine(7, 60, 118);
    }

    else {
      u8g2.setDrawColor(0);
      u8g2.drawBox(0, 52, 6, 6); //A box

      u8g2.setDrawColor(1);
      u8g2.drawStr(1, 57, "A");
      u8g2.drawBox(0, 57, 6, 6); //B box

      u8g2.setDrawColor(0);
      u8g2.drawStr(1, 62, "B");

      u8g2.drawHLine(7, 55, 118);
      u8g2.drawHLine(7, 60, 118);
    }


    u8g2.setDrawColor(1);
    u8g2.drawHLine(7, 55, map(rssi_a_const, rssiA_min, rssiA_max, 1, 118));
    u8g2.drawHLine(7, 60, map(rssi_b_const, rssiB_min, rssiB_max, 1, 118));

    u8g2.sendBuffer();


    if (current_rx == RX_B && rssi_a_norm_filt > rssi_b_norm_filt + RX_HYST) {
      SELECT_A;
      current_rx = RX_A;
    }

    if (current_rx == RX_A && rssi_b_norm_filt > rssi_a_norm_filt + RX_HYST) {
      SELECT_B;
      current_rx = RX_B;
    }

    if (button_pressed)
      timerPress();

  } //end of flymode loop

}
