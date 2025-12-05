/*********************************************************************
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 Copyright (c) 2019 Ha Thach for Adafruit Industries
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/


/* This example demonstrates use of both device and host, where
 * - Device run on native usb controller (roothub port0)
 * - Host depending on MCUs run on either:
 *   - rp2040: bit-banging 2 GPIOs with the help of Pico-PIO-USB library (roothub port1)
 *   - samd21/51, nrf52840, esp32: using MAX3421e controller (host shield)
 *
 * Requirements:
 * - For rp2040:
 *   - [Pico-PIO-USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB) library
 *   - 2 consecutive GPIOs: D+ is defined by PIN_USB_HOST_DP, D- = D+ +1
 *   - Provide VBus (5v) and GND for peripheral
 *   - CPU Speed must be either 120 or 240 Mhz. Selected via "Menu -> CPU Speed"
 * - For samd21/51, nrf52840, esp32:
 *   - Additional MAX2341e USB Host shield or featherwing is required
 *   - SPI instance, CS pin, INT pin are correctly configured in usbh_helper.h
 */

// USBHost is defined in usbh_helper.h
#include <M5Unified.h>
#include "usbh_helper.h"
#include "DisplayDataGenerator.h"
#include "RomajiConverter.h"

#define DEBUG_MODE_SERIAL //現状必須。
// #define DEBUG_LCD

#define DATA_LEN 8

uint8_t global_reports[DATA_LEN]; //global変数
bool is_in_push = false;

// ローマ字変換インスタンス
RomajiConverter romajiConverter;

// Ctrl+M検出用のフラグ（キーが離されたことを検出するため）
bool ctrl_m_pressed = false;


void main_task(void *parameter){
  while(1){

    M5.update();
    if(global_reports[2] != 0x00 && is_in_push == false){
      is_in_push = true;
      
      M5.Lcd.clearDisplay();

      #ifdef DEBUG_LCD
      M5.Lcd.setCursor(0,10);
      M5.Lcd.setTextSize(1);
      for (uint16_t i = 0; i < DATA_LEN; i++) {
        M5.Lcd.printf("0x%02X ", global_reports[i]);
        M5.Lcd.println("");
      }
      #endif
      
      // Ctrl+Mの検出（左Ctrl: bit 0, 右Ctrl: bit 4, Mキー: 0x10）
      bool ctrl_pressed = (global_reports[0] & 0x01) || (global_reports[0] & 0x10);
      bool m_pressed = (global_reports[2] == 0x10);
      
      if(ctrl_pressed && m_pressed && !ctrl_m_pressed){
        // Ctrl+Mが押された（初回検出）
        ctrl_m_pressed = true;
        romajiConverter.toggleMode();
        
        // モード表示
        DisplayData modeData = create_mode_display_data(romajiConverter.getMode() == MODE_ROMAJI);
        M5.Lcd.setCursor(modeData.x, modeData.y);
        M5.Lcd.setTextSize(modeData.font_size);
        M5.Lcd.printf("%s", modeData.lcd_str.c_str());
        M5.Lcd.waitDisplay();
        
        #ifdef DEBUG_MODE_SERIAL
        Serial.printf("Mode switched to: %s\n", 
                      romajiConverter.getMode() == MODE_ROMAJI ? "ROMAJI" : "ALPHABET");
        #endif
      }
      else if(global_reports[0] == 0x01 && global_reports[2] == 0x4F){
        //音声大を設定
        set_volume(100);
      }
      else if(global_reports[0] == 0x01 && global_reports[2] == 0x50){
        //音声小を設定
        set_volume(20);
      }
      else if(!(ctrl_pressed && m_pressed)){
        // Ctrl+M以外のキー入力処理
        if(romajiConverter.getMode() == MODE_ROMAJI){
          // ローマ字モード
          char inputChar = romajiConverter.keycodeToChar(global_reports[2]);
          RomajiState oldState = romajiConverter.getState();
          char oldConsonant = romajiConverter.getLastConsonant();
          
          String hiragana = romajiConverter.processKeyInput(global_reports[2]);
          RomajiState newState = romajiConverter.getState();
          
          // 画面をクリア（前回の表示を消す）
          M5.Lcd.clearDisplay();
          
          if(hiragana.length() > 0){
            // 読み上げるべきひらがながある場合（母音入力後など）
            // ローマ字を右上に小さく表示
            String currentRomaji = "";
            if(oldState == STATE_CONSONANT && oldConsonant != '\0' && romajiConverter.isVowel(inputChar)){
              // 子音+母音の組み合わせ
              currentRomaji = String(oldConsonant) + String(inputChar);
            } else if(oldState == STATE_N_WAIT && romajiConverter.isVowel(inputChar)){
              // n+母音の組み合わせ
              currentRomaji = "n" + String(inputChar);
            } else if(oldState == STATE_INITIAL && romajiConverter.isVowel(inputChar)){
              // 母音単独入力
              currentRomaji = String(inputChar);
            } else if(hiragana == "ん"){
              // 「ん」が読み上げられる場合
              if(oldState == STATE_N_WAIT && inputChar == 'n'){
                currentRomaji = "nn";
              } else if(oldState == STATE_N_WAIT && romajiConverter.isConsonant(inputChar)){
                currentRomaji = "n" + String(inputChar);
              }
            }
            
            if(currentRomaji.length() > 0){
              DisplayData romajiData = create_romaji_display_data(currentRomaji);
              M5.Lcd.setCursor(romajiData.x, romajiData.y);
              M5.Lcd.setTextSize(romajiData.font_size);
              M5.Lcd.printf("%s", romajiData.lcd_str.c_str());
            }
            
            // ひらがなを中央に表示
            DisplayData hiraganaData = convert_hiragana_to_DisplayData(hiragana);
            play_wav(hiraganaData.wav_path);
            M5.Lcd.setCursor(hiraganaData.x, hiraganaData.y);
            M5.Lcd.setTextSize(hiraganaData.font_size);
            M5.Lcd.printf("%s", hiraganaData.lcd_str.c_str());
            M5.Lcd.waitDisplay();
          } else if(newState == STATE_CONSONANT && romajiConverter.getLastConsonant() != '\0'){
            // 子音入力時: 子音を中央に表示
            DisplayData consonantData = create_consonant_display_data(romajiConverter.getLastConsonant());
            M5.Lcd.setCursor(consonantData.x, consonantData.y);
            M5.Lcd.setTextSize(consonantData.font_size);
            M5.Lcd.printf("%s", consonantData.lcd_str.c_str());
            M5.Lcd.waitDisplay();
          } else if(newState == STATE_N_WAIT){
            // n待機状態: "n"を中央に表示
            DisplayData nData = create_consonant_display_data('n');
            M5.Lcd.setCursor(nData.x, nData.y);
            M5.Lcd.setTextSize(nData.font_size);
            M5.Lcd.printf("%s", nData.lcd_str.c_str());
            M5.Lcd.waitDisplay();
          }
        } else {
          // アルファベットモード（既存の処理）
          DisplayData dispdata = convert_keycode_to_DisplayData(global_reports[2]);
          play_wav(dispdata.wav_path);
          M5.Lcd.setCursor(dispdata.x,dispdata.y);
          M5.Lcd.setTextSize(dispdata.font_size);
          M5.Lcd.printf("%s", dispdata.lcd_str);
          M5.Lcd.waitDisplay();
        }
      }
      
    }
    
    if(global_reports[2] == 0x00){
      is_in_push = false;
      // Ctrl+Mが離されたことを検出
      if(ctrl_m_pressed){
        ctrl_m_pressed = false;
      }
    }

    delay(10);
  }
}


//--------------------------------------------------------------------+
// Using Host shield MAX3421E controller
//--------------------------------------------------------------------+
void setup() {
  auto cfg = M5.config();

  M5.begin(cfg);

  M5.Lcd.println("hello.");
  M5.Lcd.setTextFont(&fonts::efontJA_16);

  // init host stack on controller (rhport) 1
  USBHost.begin(1);

  //  while ( !Serial ) delay(10);   // wait for native usb
  #ifdef DEBUG_MODE_SERIAL
  Serial.begin(115200);
  Serial.println("TinyUSB Dual: HID Device Report Example");
  #endif

  // スピーカーセットアップ
  M5.Lcd.println("speaker setup...");



  M5.Lcd.printf("finish setup");

  xTaskCreatePinnedToCore(main_task, "MainTask", 10000, NULL, 1, NULL, 0);


  spk_SD_setup();

}

void loop() {
  // M5.Lcd.printf("start loop ");
  USBHost.task();
  #ifdef DEBUG_MODE_SERIAL
  // Serial.flush();
  #endif
}



void key_input_parser(uint8_t const *report, uint16_t len){


  // キー入力があればglobal_reportsを更新する
  int tmp = 0;
  for (uint16_t i = 0; i < len; i++) {
    tmp += report[i];
  }
  // キーが何かしら押されていたら、グローバル変数に保存
  // if(tmp != 0){
  //   memcpy(global_reports, report, len);
  // }
  // 常にグローバル変数に保存
  if(true){
    memcpy(global_reports, report, len);
  }


  #ifdef DEBUG_MODE_SERIAL
  Serial.printf("call: key_input_parser");
  Serial.println();
  #endif

}







void tuh_hid_report_sent_cb(uint8_t dev_addr, uint8_t idx,
                            uint8_t const* report, uint16_t len) {
}

extern "C" {

// Invoked when device with hid interface is mounted
// Report descriptor is also available for use.
// tuh_hid_parse_report_descriptor() can be used to parse common/simple enough
// descriptor. Note: if report descriptor length > CFG_TUH_ENUMERATION_BUFSIZE,
// it will be skipped therefore report_desc = NULL, desc_len = 0
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *desc_report, uint16_t desc_len) {
  (void) desc_report;
  (void) desc_len;
  uint16_t vid, pid;
  tuh_vid_pid_get(dev_addr, &vid, &pid);

  #ifdef DEBUG_MODE_SERIAL
  Serial.printf("HID device address = %d, instance = %d is mounted\r\n", dev_addr, instance);
  Serial.printf("VID = %04x, PID = %04x\r\n", vid, pid);
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Serial.printf("Error: cannot request to receive report\r\n");
  }
  #endif
}

// Invoked when device with hid interface is un-mounted
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {

  #ifdef DEBUG_MODE_SERIAL
  Serial.printf("HID device address = %d, instance = %d is unmounted\r\n", dev_addr, instance);
  #endif
}

// Invoked when received report from device via interrupt endpoint
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {

  #ifdef DEBUG_MODE_SERIAL
  Serial.printf("HIDreport : ");
  for (uint16_t i = 0; i < len; i++) {
    Serial.printf("0x%02X ", report[i]);
  }
  Serial.println();
  #endif
  key_input_parser(report, len);

  #ifdef DEBUG_MODE_SERIAL
  // continue to request to receive report
  if (!tuh_hid_receive_report(dev_addr, instance)) {
    Serial.printf("Error: cannot request to receive report\r\n");
  }
  #endif
}

} // extern C