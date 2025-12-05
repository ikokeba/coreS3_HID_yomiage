
#include <M5Unified.h>
#include <SD.h>

struct DisplayData{
    String lcd_str = "";
    int font_size = 15;
    int x = 90;
    int y = -20;
    String wav_path = "/bell.wav";
  };

void set_volume(uint8_t v);

void play_wav(String wav_path);

void spk_SD_setup();

DisplayData convert_keycode_to_DisplayData(int keycode);

// ローマ字モード用: ひらがなからDisplayDataへの変換
DisplayData convert_hiragana_to_DisplayData(String hiragana);

// モード表示用のDisplayData生成
DisplayData create_mode_display_data(bool isRomajiMode);


