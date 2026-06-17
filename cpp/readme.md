# Numerical Stacking Puzzle (C++版)

このディレクトリには、C++（Visual Studio 2022）で実装された  
Numerical Stacking Puzzle のソースコードが含まれています。

---

## 概要

数値ブロックを積み上げていくパズルゲームです。  
上下左右に同じ数字ブロックが揃うと、それらがマージされます。

このC++版は、Windowsネイティブ環境での動作を前提にした実装です。

---

## 動作環境

- OS：Windows
- 開発環境：Visual Studio 2022
- 言語：C++

---

## ビルド方法

### 1. Visual Studioで開く
```
cpp/numerical-stacking-puzzle.sln
```

---

### 2. ビルド
- 「ビルド」→「ソリューションのビルド」

---

### 3. 実行
- 「デバッグ」→「デバッグの開始」
  または  
- 「デバッグなしで開始」

---

## ディレクトリ構成

```
cpp/
├── README.md
├── numerical-stacking-puzzle.sln
├── numerical-stacking-puzzle/
│   ├── *.cpp / *.h   ← メインのソースコード
│   ├── *.rc          ← リソース
│   ├── *.ico         ← アイコン
│   ├── *.vcxproj     ← プロジェクト設定
│   ├── *.filters     ← 表示設定
│   └── （Visual Studio設定ファイル）
```

※ `Debug` や `Release` フォルダはビルド時に自動生成され、  
Git管理対象には含まれていません。

---

## 設計のポイント

- クラス単位で機能を分割
- 描画処理・入力処理・ゲームロジックを分離
- 拡張しやすい構造を意識

---

## 関連

- プロジェクト全体README → `../README.md`
- Python版 → `../python/`
- Unity版 → `../unity(csharp)/`
---

## 注意事項

- Visual Studio 2022 を前提としています
- 他の環境ではビルドできない可能性があります

---

## 作者

- GitHub: https://github.com/miyabi-nari
