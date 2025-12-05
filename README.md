# USB Keyboard Speaker for M5Stack CoreS3

M5Stack CoreS3を使用したUSBキーボード入力読み上げデバイスです。USBキーボードからの入力を検出し、アルファベットまたはローマ字で読み上げます。

## 機能

### アルファベットモード（デフォルト）
- キーボードで入力されたアルファベット（A-Z）を読み上げ
- 数字（0-9）を読み上げ
- 特殊キー（Space, Enter, Tab, 矢印キーなど）を読み上げ

### ローマ字モード
- ローマ字入力を日本語のひらがなとして読み上げ
- か行〜ぱ行までの基本ローマ字に対応
- 母音単独入力、子音+母音の組み合わせに対応
- 「ん」の特殊処理に対応

### その他の機能
- 音量調整（Ctrl + → で音量大、Ctrl + ← で音量小）
- LCD表示による視覚的フィードバック
- モード切替（Ctrl + M）

## 必要なハードウェア

- M5Stack CoreS3
- MAX3421E USB Host Shield（M5Stack用）
- SDカード（音声ファイル用）
- USBキーボード

## セットアップ

### 1. 環境構築

このプロジェクトはPlatformIOを使用してビルドします。

```bash
# PlatformIOがインストールされていることを確認
pio --version

# 依存関係のインストール
pio lib install
```

### 2. 音声ファイルの準備

SDカードに以下の音声ファイル（WAV形式）を配置してください：

#### アルファベットモード用
- `/A.wav`, `/B.wav`, ..., `/Z.wav`
- `/0.wav`, `/1.wav`, ..., `/9.wav`
- `/Space.wav`, `/Enter.wav`, `/Tab.wav`
- `/RA.wav` (→), `/LA.wav` (←), `/UA.wav` (↑), `/DA.wav` (↓)

#### ローマ字モード用
- `/あ.wav`, `/い.wav`, `/う.wav`, `/え.wav`, `/お.wav`
- `/か.wav`, `/き.wav`, `/く.wav`, `/け.wav`, `/こ.wav`
- `/さ.wav`, `/し.wav`, `/す.wav`, `/せ.wav`, `/そ.wav`
- ...（その他のひらがな）
- `/ん.wav`

### 3. ビルドと書き込み

```bash
# ビルド
pio run

# デバイスへの書き込み
pio run --target upload

# シリアルモニターでデバッグ出力を確認
pio device monitor
```

## 使い方

### 基本的な使い方

1. M5Stack CoreS3に電源を投入
2. USBキーボードを接続
3. キーボードで文字を入力すると、対応する音声が再生されます

### モード切替

- **Ctrl + M**: アルファベットモード ↔ ローマ字モードを切り替え
- 現在のモードはLCD画面に表示されます

### 音量調整

- **Ctrl + →**: 音量を大きく（100%）
- **Ctrl + ←**: 音量を小さく（20%）

### ローマ字モードの使い方

#### 基本入力
- **母音単独**: `a` → "あ", `i` → "い" など
- **子音+母音**: `k + a` → "か", `s + i` → "し" など

#### 特殊な入力
- **「ん」の処理**:
  - `n + a` → "な"
  - `n + k` → "ん" + kを新規入力として扱う
  - `n + n` → "ん"
- **子音連続入力**: `k + s` → sを新規入力として扱う

## プロジェクト構造

```
USBmodule_S3/
├── src/
│   ├── main.cpp                 # メインプログラム
│   ├── DisplayDataGenerator.h/cpp  # 表示データ生成
│   ├── RomajiConverter.h/cpp   # ローマ字変換ロジック
│   └── usbh_helper.h           # USB Host設定
├── lib/
│   └── M5-Max3421E-USBShield-master/  # USB Host Shield ライブラリ
├── doc/
│   └── romaji_mode_specification.md   # ローマ字モード仕様書
├── platformio.ini               # PlatformIO設定
└── README.md                    # このファイル
```

## 開発環境

- PlatformIO
- Arduino Framework
- M5Unified Library (v0.2.2以上)
- ESP32 Platform

## ライセンス

本プロジェクトは、Adafruit TinyUSBライブラリをベースとしており、MITライセンスの下で公開されています。

## トラブルシューティング

### キーボードが認識されない
- USBキーボードが正しく接続されているか確認
- MAX3421E USB Host Shieldが正しく接続されているか確認

### 音声が再生されない
- SDカードが正しくマウントされているか確認
- 音声ファイルが正しいパスに配置されているか確認
- 音声ファイルの形式がWAVであることを確認

### ローマ字モードで文字が表示されない
- ローマ字モード用の音声ファイルがSDカードに配置されているか確認
- 無効なローマ字組み合わせ（yi, ye, wi, we, wu等）は読み上げられません

## 更新履歴

### 2025-12-05
- ローマ字読み上げモードを追加
- Ctrl+Mによるモード切替機能を実装
- ローマ字入力の状態管理機能を実装

## 参考資料

- [M5Stack CoreS3 公式ドキュメント](https://docs.m5stack.com/)
- [PlatformIO ドキュメント](https://docs.platformio.org/)
- [Adafruit TinyUSB ライブラリ](https://github.com/adafruit/Adafruit_TinyUSB_Arduino)

