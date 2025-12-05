#include "RomajiConverter.h"

// ローマ字からひらがなへのマッピングテーブル
// 子音(行) x 母音(段) の2次元配列
const char* romaji_map[20][5] = {
    // a    i    u    e    o
    {"か", "き", "く", "け", "こ"},  // k
    {"さ", "し", "す", "せ", "そ"},  // s
    {"た", "ち", "つ", "て", "と"},  // t
    {"な", "に", "ぬ", "ね", "の"},  // n
    {"は", "ひ", "ふ", "へ", "ほ"},  // h
    {"ま", "み", "む", "め", "も"},  // m
    {"や", "",   "ゆ", "",   "よ"},  // y
    {"ら", "り", "る", "れ", "ろ"},  // r
    {"わ", "",   "",   "",   "を"},  // w
    {"が", "ぎ", "ぐ", "げ", "ご"},  // g
    {"ざ", "じ", "ず", "ぜ", "ぞ"},  // z
    {"だ", "ぢ", "づ", "で", "ど"},  // d
    {"ば", "び", "ぶ", "べ", "ぼ"},  // b
    {"ぱ", "ぴ", "ぷ", "ぺ", "ぽ"},  // p
    {"",   "",   "",   "",   ""},    // c (未使用)
    {"",   "",   "",   "",   ""},    // f (未使用)
    {"",   "",   "",   "",   ""},    // v (未使用)
    {"",   "",   "",   "",   ""},    // j (未使用)
    {"",   "",   "",   "",   ""},    // l (未使用)
    {"",   "",   "",   "",   ""}     // x (未使用)
};

// 母音からひらがなへのマッピング
const char* vowel_map[5] = {"あ", "い", "う", "え", "お"};

RomajiConverter::RomajiConverter() {
    currentMode = MODE_ALPHABET;
    state = STATE_INITIAL;
    lastConsonant = '\0';
}

void RomajiConverter::toggleMode() {
    if (currentMode == MODE_ALPHABET) {
        currentMode = MODE_ROMAJI;
    } else {
        currentMode = MODE_ALPHABET;
    }
    resetState();
}

char RomajiConverter::keycodeToChar(uint8_t keycode) {
    // キーコードを小文字の文字に変換
    if (keycode >= 0x04 && keycode <= 0x1d) {
        // A-Zのキーコードを小文字に変換
        return 'a' + (keycode - 0x04);
    }
    return '\0';
}

bool RomajiConverter::isVowel(char c) {
    return (c == 'a' || c == 'i' || c == 'u' || c == 'e' || c == 'o');
}

bool RomajiConverter::isConsonant(char c) {
    // 子音の定義: k, s, t, n, h, m, y, r, w, g, z, d, b, p, c, f, v, j, l, x
    return (c == 'k' || c == 's' || c == 't' || c == 'n' || c == 'h' ||
            c == 'm' || c == 'y' || c == 'r' || c == 'w' || c == 'g' ||
            c == 'z' || c == 'd' || c == 'b' || c == 'p' || c == 'c' ||
            c == 'f' || c == 'v' || c == 'j' || c == 'l' || c == 'x');
}

String RomajiConverter::romajiToHiragana(char consonant, char vowel) {
    // 子音と母音からインデックスを計算
    int consonantIndex = -1;
    int vowelIndex = -1;
    
    // 子音のインデックスを取得
    switch (consonant) {
        case 'k': consonantIndex = 0; break;
        case 's': consonantIndex = 1; break;
        case 't': consonantIndex = 2; break;
        case 'n': consonantIndex = 3; break;
        case 'h': consonantIndex = 4; break;
        case 'm': consonantIndex = 5; break;
        case 'y': consonantIndex = 6; break;
        case 'r': consonantIndex = 7; break;
        case 'w': consonantIndex = 8; break;
        case 'g': consonantIndex = 9; break;
        case 'z': consonantIndex = 10; break;
        case 'd': consonantIndex = 11; break;
        case 'b': consonantIndex = 12; break;
        case 'p': consonantIndex = 13; break;
        default: return "";  // 無効な子音
    }
    
    // 母音のインデックスを取得
    switch (vowel) {
        case 'a': vowelIndex = 0; break;
        case 'i': vowelIndex = 1; break;
        case 'u': vowelIndex = 2; break;
        case 'e': vowelIndex = 3; break;
        case 'o': vowelIndex = 4; break;
        default: return "";  // 無効な母音
    }
    
    // マッピングテーブルから取得
    if (consonantIndex >= 0 && vowelIndex >= 0) {
        const char* hiragana = romaji_map[consonantIndex][vowelIndex];
        if (hiragana[0] != '\0') {
            return String(hiragana);
        }
    }
    
    return "";  // 無効な組み合わせ
}

String RomajiConverter::vowelToHiragana(char vowel) {
    switch (vowel) {
        case 'a': return String(vowel_map[0]);
        case 'i': return String(vowel_map[1]);
        case 'u': return String(vowel_map[2]);
        case 'e': return String(vowel_map[3]);
        case 'o': return String(vowel_map[4]);
        default: return "";
    }
}

String RomajiConverter::processKeyInput(uint8_t keycode) {
    if (currentMode != MODE_ROMAJI) {
        return "";  // ローマ字モードでない場合は処理しない
    }
    
    char c = keycodeToChar(keycode);
    if (c == '\0') {
        return "";  // 無効なキーコード
    }
    
    String result = "";
    
    switch (state) {
        case STATE_INITIAL:
            if (isVowel(c)) {
                // 母音が入力された場合、そのまま読み上げ
                result = vowelToHiragana(c);
                // 状態は初期状態のまま
            } else if (c == 'n') {
                // 'n'が入力された場合、n待機状態に遷移
                state = STATE_N_WAIT;
                // 読み上げは行わない
            } else if (isConsonant(c)) {
                // 子音が入力された場合、子音待機状態に遷移
                state = STATE_CONSONANT;
                lastConsonant = c;
                // 読み上げは行わない
            }
            break;
            
        case STATE_CONSONANT:
            if (isVowel(c)) {
                // 母音が入力された場合、ローマ字を読み上げ
                result = romajiToHiragana(lastConsonant, c);
                state = STATE_INITIAL;
                lastConsonant = '\0';
            } else if (c == 'n') {
                // 'n'が入力された場合、n待機状態に遷移
                state = STATE_N_WAIT;
                // 最後の子音は保持しない（n待機状態に移行）
            } else if (isConsonant(c)) {
                // 別の子音が入力された場合、最後の子音を新規入力として扱う
                lastConsonant = c;
                // 状態は子音待機状態のまま
                // 読み上げは行わない
            }
            break;
            
        case STATE_N_WAIT:
            if (isVowel(c)) {
                // 母音が入力された場合、ローマ字を読み上げ（na, ni, nu, ne, no）
                result = romajiToHiragana('n', c);
                state = STATE_INITIAL;
            } else if (isConsonant(c)) {
                // 子音が入力された場合、"ん"を読み上げ、子音を新規入力として扱う
                result = "ん";
                state = STATE_CONSONANT;
                lastConsonant = c;
            } else if (c == 'n') {
                // 'n'が再度入力された場合、"ん"を読み上げ
                result = "ん";
                // 状態はn待機状態のまま
            }
            break;
    }
    
    return result;
}

void RomajiConverter::resetState() {
    state = STATE_INITIAL;
    lastConsonant = '\0';
}

String RomajiConverter::getCurrentRomaji() const {
    if (state == STATE_CONSONANT && lastConsonant != '\0') {
        // 子音待機状態: 子音のみを返す
        String romaji = "";
        romaji += lastConsonant;
        return romaji;
    } else if (state == STATE_N_WAIT) {
        // n待機状態: "n"を返す
        return "n";
    }
    return "";  // 初期状態またはその他
}

