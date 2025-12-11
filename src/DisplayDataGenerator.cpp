#include "DisplayDataGenerator.h"



// キーコードと文字の対応表
char keycode_to_char_AtoZ[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N',
    'O', 'P', 'Q', 'R', 'S', 'T', 'U',
    'V', 'W', 'X', 'Y', 'Z'
};

char keycode_to_char_number[] = {
    '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0'
};

// #define DEBUG_LCD

void play_wav(String wav_path){
    #ifdef DEBUG_LCD
    M5.Lcd.println("load "+wav_path);
    #endif
    File f = SD.open(wav_path);
    if (!f) {
        
        #ifdef DEBUG_LCD
        M5.Lcd.println("file open error");
        #endif

        return;
    }
    //  M5.Lcd.printf("file size:%d", f.size());
    //  M5.Lcd.println("");


    //https://community.m5stack.com/topic/6958/using-m5-speaker-playwav-in-void-loop
    size_t wav_fileSize = f.size();
    uint8_t *wav_Buffer = (uint8_t*)malloc(wav_fileSize);
    f.read(wav_Buffer, wav_fileSize);
    f.close();
    M5.Speaker.playWav(wav_Buffer);

}


void spk_SD_setup(){
    { /// I2S Custom configurations are available if you desire.
        auto spk_cfg = M5.Speaker.config();

        M5.Speaker.config(spk_cfg);
    }
    M5.Speaker.begin();

    //音量の初期値設定
    set_volume(20);

    //SDマウント
    SD.begin(GPIO_NUM_4, SPI, 25000000);


}

void set_volume(uint8_t v){
    M5.Speaker.setVolume(v);
    M5.Lcd.setCursor(0,10);
    M5.Lcd.setTextSize(1);
    M5.Lcd.println("volume: "+String(v));
}

// キーコードを文字に変換する関数
DisplayData convert_keycode_to_DisplayData(int keycode) {
    DisplayData ret_val;
    //アルファベットの場合
    if (keycode >= 0x04 && keycode <= 0x1d) {
        ret_val.lcd_str = keycode_to_char_AtoZ[keycode - 0x04];
        ret_val.wav_path = "/"+ret_val.lcd_str+".wav";
    }
    //数値の場合
    if (keycode >= 0x1E && keycode <= 0x27) {
        ret_val.lcd_str = keycode_to_char_number[keycode - 0x1E];
        ret_val.wav_path = "/"+ret_val.lcd_str+".wav";
    }

    //特殊な記号の場合
    //Space, Enter, Ctrl, 矢印, tab, alt, @
    switch (keycode)
    {
      case 0x2c:
        //space
        ret_val.lcd_str = "Space";
        ret_val.font_size = 6;
        ret_val.x = 20;
        ret_val.y = 70;
        ret_val.wav_path = "/"+ret_val.lcd_str+".wav";
        break;
      
      case 0x2b:
        //tab
        ret_val.lcd_str = "Tab";
        ret_val.font_size = 6;
        ret_val.x = 20;
        ret_val.y = 70;
        ret_val.wav_path = "/"+ret_val.lcd_str+".wav";
        break;

      case 0x28:
        // Enter
        ret_val.lcd_str = "Enter";
        ret_val.font_size = 6;
        ret_val.x = 20;
        ret_val.y = 70;
        ret_val.wav_path = "/"+ret_val.lcd_str+".wav";
        break;
        
      case 0x4f:
        //→
        ret_val.lcd_str = "→";
        ret_val.font_size = 17;
        ret_val.wav_path = "/RA.wav";
        break;
        
      case 0x50:
        //←
        ret_val.lcd_str = "←";
        ret_val.font_size = 17;
        ret_val.wav_path = "/LA.wav";
        break;
        
      case 0x51:
        //↓
        ret_val.lcd_str = "↓";
        ret_val.font_size = 17;
        ret_val.wav_path = "/DA.wav";
        break;
        
      case 0x52:
        //↑
        ret_val.lcd_str = "↑";
        ret_val.font_size = 17;
        ret_val.wav_path = "/UA.wav";
        break;
        
      case 0x2f:
        //@
        ret_val.lcd_str = "@";
        ret_val.font_size = 6;
        ret_val.x = 20;
        ret_val.y = 70;
        ret_val.wav_path = "/at.wav";
        break;
        
        
      default:
        break;
    
    }


    return ret_val;
}

// ローマ字モード用: ひらがなからDisplayDataへの変換（中央表示）
DisplayData convert_hiragana_to_DisplayData(String hiragana) {
    DisplayData ret_val;
    ret_val.lcd_str = hiragana;
    ret_val.font_size = 9;  // アルファベットモードの約半分のサイズ
    ret_val.x = 90;  // 中央寄り
    ret_val.y = 50;  // 画面中央より少し下（縦方向にはみ出ないように調整）
    ret_val.wav_path = "/" + hiragana + ".wav";
    return ret_val;
}

// ローマ字モード用: 子音を中央に表示するDisplayData生成
DisplayData create_consonant_display_data(char consonant) {
    DisplayData ret_val;
    ret_val.lcd_str = String(consonant);
    ret_val.font_size = 7;  // 小さめのサイズ
    ret_val.x = 90;  // 中央
    ret_val.y = 50;  // 画面中央より少し下
    ret_val.wav_path = "";  // 子音単独では音声再生なし
    return ret_val;
}

// ローマ字モード用: ローマ字を右上に小さく表示するDisplayData生成
DisplayData create_romaji_display_data(String romaji) {
    DisplayData ret_val;
    ret_val.lcd_str = romaji;
    ret_val.font_size = 2;  // 非常に小さいサイズ
    ret_val.x = 200;  // 右上寄り
    ret_val.y = 10;   // 上端
    ret_val.wav_path = "";  // 表示のみ、音声再生なし
    return ret_val;
}

// モード表示用のDisplayData生成
DisplayData create_mode_display_data(bool isRomajiMode) {
    DisplayData ret_val;
    if (isRomajiMode) {
        ret_val.lcd_str = "ローマ字モード";
    } else {
        ret_val.lcd_str = "アルファベットモード";
    }
    ret_val.font_size = 2;
    ret_val.x = 10;
    ret_val.y = 10;
    ret_val.wav_path = "";  // モード切替時は音声再生なし
    return ret_val;
}