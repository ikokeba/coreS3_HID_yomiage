
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


