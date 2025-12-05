
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

// ローマ字モード用: ひらがなからDisplayDataへの変換（中央表示）
DisplayData convert_hiragana_to_DisplayData(String hiragana);

// ローマ字モード用: 子音を中央に表示するDisplayData生成
DisplayData create_consonant_display_data(char consonant);

// ローマ字モード用: ローマ字を右上に小さく表示するDisplayData生成
DisplayData create_romaji_display_data(String romaji);

// モード表示用のDisplayData生成
DisplayData create_mode_display_data(bool isRomajiMode);


