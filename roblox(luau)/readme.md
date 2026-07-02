# Numerical Stacking Puzzle (Roblox版)

このディレクトリには、Roblox（Luau）で実装された  
Numerical Stacking Puzzle のソースコードが含まれています。

---

## 概要

数値ブロックを積み上げていくパズルゲームです。  
上下左右に同じ数字ブロックが揃うと、それらがマージされます。

このRoblox版は、C++版およびUnity版をベースに再実装されたもので、  
Roblox環境上で動作するようにUI・入力・描画を最適化しています。

---

## 動作環境

- Roblox Studio バージョン：0.728.0.7280895 (64bit)
- 言語：Luau（Roblox Lua）
- 開発ツール：
  - Rojo（必須に近い推奨 ※使用しない場合、手動でソースコードの階層構築とコピーが必要）

---

## 起動方法

### 1. Rojoサーバーを起動

プロジェクトルートで以下を実行します：

```bash
rojo serve
```

以下のような表示が出ます：

```
Rojo server listening:
  Address: localhost
  Port:    34872
```

---

### 2. Roblox Studioで接続

1. Roblox Studioを起動
2. 「Plugins」→ Rojo → **Connect**
3. `localhost:34872` に接続

---

### 3. 実行

- Playボタンを押すことでゲーム開始

---

---

## ディレクトリ構成

```txt
roblox(luau)/
├── README.md
├── default.project.json  ← rojo用jsonファイル
├── rojo.exe.txt          ← ここにrojo.exeを配置します。
├── numerical-stacking-puzzle
│   ├── src/              ← luauソースコード(アプリケーションスクリプト)
│   ├── client/           ← luauソースコード(クライアントスクリプト)
│   ├── server/           ← luauソースコード(サーバースクリプト)
│   └── audio/            ← 効果音用サンプルwavファイル
```

---

## 設計のポイント

MVCベースで責務を分離しています：

- **GameController**  
  ゲーム全体の進行管理（フェーズ制御）

- **Model系**
  - BoardModel（盤面ロジック）
  - NextBoxModel（NEXTキュー）

- **View系**
  - ViewRenderer（基盤UI）
  - TitleViewRenderer（タイトル）
  - GameViewRenderer（ゲーム画面）
    - TileView（ブロック）
    - HudView（スコア・時間）

- **InputController**
  非同期入力をフレーム同期へ変換

- **SaveCodec**
  セーブデータのシリアライズ

C++版のソースコードとの対応は、各ソースファイルのヘッダーコメントを参照してください。

---

## 操作方法

C++版と同じ操作の他、マウス・タッチ操作に対応しています。
（Android版は主にタッチ操作を想定しています）

### キーボード

| 操作    | 内容         |
| ----- | ---------- |
| A〜;キー | 列選択        |
| 1キー   | NEW / BACK |
| 2キー   | LOAD       |
| 3キー   | SAVE       |


### マウス / タッチ

* 盤面クリック / タップ → 列選択
* UIボタン操作可能

---

## セーブについて

- C++版のようなロード時のファイル選択機能はありません。
- Remote通信を通じたセーブ・ロード機能です。
- サーバー動作とローカル動作を切り替える時は、サーバースクリプトNSP_SaveServer.server.luau内の[USE_DATASTORE]の記述を変更する必要があります。(ファイル内コメントを参照ください)

---

---

## 🔊 効果音（Asset）の追加方法

本プロジェクトでは、RobloxのSoundインスタンスを使用して効果音を再生しています。  
新しい効果音を追加する場合は、以下の手順でAsset（音声）を登録してください。

---

### 1. 音声ファイルの準備

- 対応形式：`.wav`（推奨）または `.mp3`
- 短い効果音（SE）を想定（numerical-stacking-puzzle/audio/ ディレクトリにサンプルファイルを格納しています）

---

### 2. Robloxにアップロード

1. Robloxサイトにログイン  
2. Creator Dashboard → **Creations** → **Audio**  
3. 「Upload」から音声ファイルをアップロード  
4. アップロード後に表示される **Asset ID** をコピー  

例：

```

rbxassetid://1234567890

```

---

### 3. SoundManagerに登録

`SoundManager.luau` の以下の部分を編集します：

```lua
self.stack = makeSound("Stack", "rbxassetid://xxxxxxxxxxxxxx")
self.merge1 = makeSound("Merge1", "rbxassetid://xxxxxxxxxxxxxx")
self.merge2 = makeSound("Merge2", "rbxassetid://xxxxxxxxxxxxxx")
self.merge3 = makeSound("Merge3", "rbxassetid://xxxxxxxxxxxxxx")
```

---

### 4. 注意点

* プライベートAssetは他ユーザーに再生されない場合があります
* 音量は `Sound.Volume` で調整可能

---

### 💡 補足

Robloxでは音声の公開設定や利用制限があるため、  
テスト時は自分のアカウントでアップロードしたAssetを使用してください。

---

## 関連

* プロジェクト全体README → `../README.md`
* C++版 → `../cpp/`
* Python版 → `../python/`
* Unity版 → `../unity(csharp)/`

---

## 注意事項

- Roblox Studio + Rojo環境が必要です
- Rojoを使用しない場合、手動でソースコードの階層構築とコピーが必要です。
- Rojoの初回接続時はプラグインのHTTP許可が必要です
- モバイル環境では一部UI挙動が異なる可能性があります

---

## 作者

- GitHub: https://github.com/miyabi-nari
