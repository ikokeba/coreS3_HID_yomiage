#ifndef ROMAJI_CONVERTER_H
#define ROMAJI_CONVERTER_H

#include <Arduino.h>

// 入力モード
enum InputMode {
    MODE_ALPHABET,  // アルファベットモード
    MODE_ROMAJI     // ローマ字モード
};

// ローマ字入力状態
enum RomajiState {
    STATE_INITIAL,      // 初期状態
    STATE_CONSONANT,    // 子音待機状態
    STATE_N_WAIT        // n待機状態
};

// ローマ字変換クラス
class RomajiConverter {
public:
    RomajiConverter();
    
    // モード切替
    void toggleMode();
    InputMode getMode() const { return currentMode; }
    
    // キーコードから文字への変換（小文字）
    char keycodeToChar(uint8_t keycode);
    
    // 文字が母音かどうか
    bool isVowel(char c);
    
    // 文字が子音かどうか
    bool isConsonant(char c);
    
    // ローマ字モードでのキー入力処理
    // 戻り値: 読み上げるべきひらがな（空文字列の場合は読み上げなし）
    String processKeyInput(uint8_t keycode);
    
    // 状態をリセット
    void resetState();
    
    // 現在の状態を取得
    RomajiState getState() const { return state; }

private:
    InputMode currentMode;
    RomajiState state;
    char lastConsonant;  // 最後に入力された子音
    
    // ローマ字からひらがなへのマッピング
    String romajiToHiragana(char consonant, char vowel);
    
    // 母音単独のひらがなマッピング
    String vowelToHiragana(char vowel);
};

#endif // ROMAJI_CONVERTER_H

