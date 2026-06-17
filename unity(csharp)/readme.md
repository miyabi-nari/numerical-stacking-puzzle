# Numerical Stacking Puzzle (Unity版)

このディレクトリには、Unity（C#）で実装された  
Numerical Stacking Puzzle のソースコードが含まれています。

---

## 概要

数値ブロックを積み上げていくパズルゲームです。  
上下左右に同じ数字ブロックが揃うと、それらがマージされます。

このUnity版は、C++版をベースに再実装されたもので、  
クロスプラットフォーム動作（Windows / Android / Linux / Web）に対応しています。

---

## 動作環境

- Unity バージョン：推奨 2021以降
- 言語：C#  
- 対応プラットフォーム：
  - Windows
  - Android
  - Linux (x86_64)
  - WebGL（ブラウザ）

---

## 起動方法

### 1. Unityで開く

Unity Hubを使い、以下のディレクトリを開きます：

```

unity/

```

---

### 2. シーン起動

- `Assets/Scenes` 内のメインシーンを開く  
- 再生（Play）ボタンで実行

---

---

## ビルド方法

### 共通

```

File → Build Settings

```

---

### ■ Windows

```

Platform: PC, Mac & Linux Standalone
Target: Windows

```

---

### ■ Android

```

Platform: PC, Mac & Linux Standalone
Target: Windows

```

---

### ■ Linux

```

Platform: PC, Mac & Linux Standalone
Target: Linux (x86\_64)

```

---

### ■ Web（ブラウザ）

```

Platform: WebGL

```

※ Web版は Compression Format を `Disabled` もしくは `Gzip` 推奨

---

---

## ディレクトリ構成

```

unity(csharp)/
├── README.md
├── numerical-stacking-puzzle
│   ├── Assets/
│   │   ├── Scripts/        ← C#ソースコード
│   │   ├── Scenes/         ← メインシーン
│   │   ├── Prefabs/        ← タイル・UIなど
│   │   ├── Audio/
│   │   ├── Input/
│   │   ├── Materials/
│   │   ├── Resources/
│   │   ├── TextMesh Pro/
│   │   └── _Recovery/
│   ├── ProjectSettings/
│   ├── Packages/
```

※ `Library`, `Temp`, `Build` などはGit管理対象外です。

---

## 設計のポイント

- MVCに近い構成で責務を分離
    - GameController：進行管理
    - GameBoard：盤面データ・ロジック
    - Renderer群：描画
    - InputManager：入力

- C++版ロジックをベースに再構築
- フェーズ管理（Serve / Fall / Stack / Merge / Land）による制御

---

## 入力操作

C++版と同じ操作の他、マウス・タッチ操作に対応しています。
（Android版は主にタッチ操作を想定しています）

---

## セーブについて

- C++版のようなロード時のファイル選択機能はありません。
- ローカル環境ではファイル保存に対応していますが、WebGL版ではファイル保存に制限があります

---

## 関連

- プロジェクト全体README → `../README.md`
- C++版 → `../cpp/`
- Python版 → `../python/`

---

## 注意事項

- Unityエディタ環境が必要です
- WebGL版はブラウザ制約（音・ファイルアクセス）があります

---

## 作者

- GitHub: https://github.com/miyabi-nari
