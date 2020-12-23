//Handling for RX5808 modules using RichWave Library
#define MIN_TUNE_TIME 40

void scan(bool RX) {
  uint16_t freq;

  for (uint16_t chan = CHANNEL_MIN; chan < CHANNEL_MAX; chan++) {

    freq = channelFreqTable[chan];

    if (RX) {
      rx5808A.setFrequency(freq);

      delay(MIN_TUNE_TIME);
      scanVecA[chan] = rx5808A.readRSSI(); //raw value
    }
    else {
      rx5808B.setFrequency(freq);

      delay(MIN_TUNE_TIME);
      scanVecB[chan] = rx5808B.readRSSI(); //raw value
    }
  }
  return;
}

void calibration(bool RX) {
  int i = 0, j = 0;
  uint16_t  rssi_setup_min = 1024, minValue = 1024;
  uint16_t  rssi_setup_max = 0, maxValue = 0;
  uint16_t scanVec[CHANNEL_MAX];

  for (j = 0; j < 5; j++) {
    if (RX) {
      scan(RX_A);
      memcpy(scanVec, scanVecA, sizeof(uint16_t)*CHANNEL_MAX);
    }
    else {
      scan(RX_B);
      memcpy(scanVec, scanVecB, sizeof(uint16_t)*CHANNEL_MAX);
    }

    for (i = CHANNEL_MIN; i < CHANNEL_MAX; i++) {

      uint16_t rssi = scanVec[i];

      minValue = min(minValue, rssi);
      maxValue = max(maxValue, rssi);
    }

    rssi_setup_min = min(rssi_setup_min, minValue); //?minValue:rssi_setup_min;
    rssi_setup_max = max(rssi_setup_max, maxValue); //?maxValue:rssi_setup_max;
  }

  if (RX)
  {

    EEPROM.write(EEPROM_ADR_RSSI_MIN_A, (rssi_setup_min));
    EEPROM.write(EEPROM_ADR_RSSI_MAX_A, (rssi_setup_max));

    rssiA_min = (EEPROM.read(EEPROM_ADR_RSSI_MIN_A));
    rssiA_max = (EEPROM.read(EEPROM_ADR_RSSI_MAX_A));
  }

  else
  {
    EEPROM.write(EEPROM_ADR_RSSI_MIN_B, (rssi_setup_min));
    EEPROM.write(EEPROM_ADR_RSSI_MAX_B, (rssi_setup_max));

    rssiB_min = (EEPROM.read(EEPROM_ADR_RSSI_MIN_B));
    rssiB_max = (EEPROM.read(EEPROM_ADR_RSSI_MAX_B));
  }

  return;
}

uint16_t getMaxPos() {
  scan(RX_B);
  uint16_t maxVal = 0, maxPos = 0;
  for (uint8_t chan = CHANNEL_MIN; chan < CHANNEL_MAX; chan++) {
    if (maxVal < scanVecB[chan]) { //new max
      maxPos = chan;
      maxVal = scanVecB[chan];
    }
  }
  return maxPos;
}
