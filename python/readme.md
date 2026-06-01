# Numerical Stacking Puzzle (Python版)

このディレクトリには、Pythonで実装された、
Numerical Stacking Puzzle のソースコードが含まれています。

---

## 概要

数値ブロックを積み上げていくパズルゲームです。  
上下左右に同じ数字ブロックが揃うと、それらがマージされます。

このPython版は、GUIライブラリ **PySide6 (Qt for Python)** を用いた実装です。  
主に Linux（Ubuntu系）環境での実行を想定していますが、  
**Linux（Debian / Raspberry Pi）および Windows の Python 実行環境でも動作を確認しています。**

---

## 動作環境

- Linux（Ubuntu推奨）
- Linux（Debian / Raspberry Pi）※動作確認済み
- Windows（Python実行）※動作確認済み
- Python 3.x
- GUI：PySide6（Qt）

---

## 実行方法（Linux / Ubuntu）

### ① システム準備

まずUbuntuの基本環境を整えます。

```bash
sudo apt update
sudo apt install -y python3 python3-venv python3-pip
```

これで以下が利用できるようになります

- Python本体
- venv（仮想環境機能）
- pip（パッケージ管理）

---

### ② リポジトリをクローン

```bash
git clone https://github.com/miyabi-nari/numerical-stacking-puzzle.git
cd numerical-stacking-puzzle/python
```

---

### ③ 仮想環境（venv）を作成

最近のPythonでは、プロジェクトごとに仮想環境を使うことが推奨されています。

```bash
python3 -m venv venv
```

これにより、`venv/` ディレクトリが作成され、  
このプロジェクト専用の Python 実行環境を用意できます。

---

### ④ 仮想環境を有効化

```bash
source venv/bin/activate
```

有効化されると、シェルの先頭に `(venv)` が表示されます。

---

### ⑤ PySide6 のインストール

```bash
pip install PySide6
```

このパッケージは仮想環境内にのみインストールされます。

---


### ⑥ 追加ライブラリのインストール（環境によって必要）

Qt / GUI 実行時に追加ライブラリが必要になる場合があります。

```bash
sudo apt install -y libxcb-cursor0
sudo apt install -y libpulse-mainloop-glib0
sudo apt install -y fonts-noto-cjk
```


用途の例：

- `libxcb-cursor0`  
  Qt の xcb プラットフォームプラグイン関連
- `libpulse-mainloop-glib0`  
  音関連ライブラリ
- `fonts-noto-cjk`  
  日本語表示用フォント

環境によっては、これらが無くても動作する場合がありますが、  
不足していると起動エラーや文字表示の問題が発生することがあります。

---

### ⑦ アプリ実行

`main.py` がこのディレクトリ直下にある場合：

```bash
python main.py
```

もし `numerical-stacking-puzzle/` 配下にある構成にしている場合は、実際の配置に合わせて  
次のように実行してください。

```bash
python numerical-stacking-puzzle/main.py
```

---

## ディレクトリ構成

```
python/
├── README.md
├── numerical-stacking-puzzle/
│   ├── *.py          ← メインのソースコード
│   ├── *.wav         ← サウンドファイル
│   └── venv/         ← 仮想環境（Git管理外）
```

---

## 設計のポイント

- **PySide6** を使用した GUI アプリケーション
- ロジックと UI の分離を意識
- Pythonらしい、比較的シンプルで拡張しやすい構成

---

## 制限事項

- 本アプリケーションは **GUI アプリケーション** です
- そのため、**Google Colaboratory** や **Jupyter Notebook** のような  
  GUI ウィンドウを前提としない実行環境では、通常のデスクトップアプリとしては動作しません
- 学習用にソースコードを参照することは可能ですが、上記環境での起動・操作は想定していません

---

## 関連

- プロジェクト全体README → `../README.md`
- C++版 → `../cpp/`

---

## 注意事項

- 仮想環境（venv）の使用を推奨します
- Linux環境では、Qt関連の追加ライブラリが必要になる場合があります
- 実行コマンドは `main.py` の配置場所に応じて適宜読み替えてください

---

## 作者

- GitHub: https://github.com/miyabi-nari
