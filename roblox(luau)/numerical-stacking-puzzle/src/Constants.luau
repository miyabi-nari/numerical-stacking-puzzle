--[[====================================================================
 Copyright 2026 Miyabi
 Constants.luau
 共通パラメータ定義
 C++版相当クラス: なし(define.h, define.cpp)
======================================================================]]
local Constants = {}

-- =====================================================================
-- 定数定義
Constants.APP_NAME = "Numerical Stacking Puzzle"		-- アプリケーション名
Constants.APP_VERSION = "Version 0.1"					-- バージョン表記
Constants.APP_COPYRIGHT = "Copyright(C) 2026 Miyabi."	-- CopyRight表記

Constants.SAVE_VERSION = 10			-- セーブデータバージョン番号

Constants.CELL_X_NUM = 11			-- 盤面の横方向セル数
Constants.CELL_Y_NUM = 7			-- 盤面の縦方向セル数
Constants.NEXT_BOX_NUM = 4			-- NEXTキューのセル数
Constants.NEXT_BOX_VALUE_NUM = 6	-- NEXTキューのセルが格納する値の範囲

Constants.EMPTY = -1				-- 盤面・NEXTキューの空セルを表す値

-- 画面の大きさ定義(仮想空間座標)
Constants.BASE_WIDTH = 1600+400		-- 画面横幅
Constants.BASE_HEIGHT = 900			-- 画面縦幅

-- 画面上のオブジェクト座標(仮想空間座標)
-- note: 左上隅を(0,0)、右下隅を(BASE_WIDTH-1,BASE_HEIGHT-1)とする。
Constants.CELL_SIZE = 100			-- タイル１個のサイズ
Constants.BOARD_OFFSET_X = 0+400	-- 盤面の横オフセット位置
Constants.BOARD_OFFSET_Y = 100		-- 盤面の縦オフセット位置

Constants.NEXT_BOX_OFFSET_X = 1200+400	-- NEXTキューの横オフセット位置
Constants.NEXT_BOX_OFFSET_Y = 100		-- NEXTキューの縦オフセット位置

-- タイマー定数(FPS単位)
Constants.TM_SERVE = 15			-- サーブ状態保持時間
Constants.TM_FALL_SLOW_INI = 60	-- 落下(遅)状態保持時間(1セル落下時間)
Constants.TM_FALL_FAST = 5		-- 落下(速)状態保持時間(1セル落下時間)
Constants.TM_STACK = 10			-- 積上状態保持時間
Constants.TM_MERGE = 30			-- 結合動作状態保持時間
Constants.TM_LAND = 30			-- 落下着地動作状態保持時間

-- タイルのアニメーション時間
Constants.TILE_TWEEN_FRAMES = 12
Constants.TILE_TWEEN_TIME = Constants.TILE_TWEEN_FRAMES / 60

-- =====================================================================
-- 定数テーブル
-- キー入力状態表示の文字列
-- (上段: 英字キー / 下段: 対応日本語キー表示)
Constants.KEY_LABELS = {
	[0] = {"Ａ　", "　ち"},
	[1] = {"Ｓ　", "　と"},
	[2] = {"Ｄ　", "　し"},
	[3] = {"Ｆ　", "　は"},
	[4] = {"Ｇ　", "　き"},
	[5] = {"Ｈ　", "　く"},
	[6] = {"Ｊ　", "　ま"},
	[7] = {"Ｋ　", "　の"},
	[8] = {"Ｌ　", "　り"},
	[9] = {"＋　", "；れ"},
	[10] = {"＊　", "：け"},
}

-- ブロック表示
Constants.TILE_STYLES = {
	[0] = {text="1", background=Color3.fromRGB(128,128,128), textColor=Color3.fromRGB(0,0,0), textSize=64},
	[1] = {text="2", background=Color3.fromRGB(64,64,192), textColor=Color3.fromRGB(0,0,0), textSize=64},
	[2] = {text="4", background=Color3.fromRGB(192,64,64), textColor=Color3.fromRGB(0,0,0), textSize=64},
	[3] = {text="8", background=Color3.fromRGB(192,64,192), textColor=Color3.fromRGB(0,0,0), textSize=64},
	[4] = {text="16", background=Color3.fromRGB(64,192,64), textColor=Color3.fromRGB(0,0,0), textSize=48},
	[5] = {text="32", background=Color3.fromRGB(64,192,192), textColor=Color3.fromRGB(0,0,0), textSize=48},
	[6] = {text="64", background=Color3.fromRGB(192,192,64), textColor=Color3.fromRGB(0,0,0), textSize=48},
	[7] = {text="128", background=Color3.fromRGB(192,192,192), textColor=Color3.fromRGB(0,0,0), textSize=40},
	[8] = {text="256", background=Color3.fromRGB(192,192,255), textColor=Color3.fromRGB(0,0,0), textSize=40},
	[9] = {text="512", background=Color3.fromRGB(255,192,192), textColor=Color3.fromRGB(0,0,0), textSize=40},
	[10] = {text="1,024", background=Color3.fromRGB(255,192,255), textColor=Color3.fromRGB(0,0,0), textSize=32},
	[11] = {text="2,048", background=Color3.fromRGB(192,255,192), textColor=Color3.fromRGB(0,0,0), textSize=32},
	[12] = {text="4,096", background=Color3.fromRGB(192,255,255), textColor=Color3.fromRGB(0,0,0), textSize=32},
	[13] = {text="8,192", background=Color3.fromRGB(255,255,192), textColor=Color3.fromRGB(0,0,0), textSize=32},
	[14] = {text="16,384", background=Color3.fromRGB(255,255,255), textColor=Color3.fromRGB(0,0,0), textSize=28},
	[15] = {text="32,768", background=Color3.fromRGB(32,32,32), textColor=Color3.fromRGB(255,255,255), textSize=28},
	[16] = {text="65,536", background=Color3.fromRGB(32,32,128), textColor=Color3.fromRGB(255,255,255), textSize=28},
	[17] = {text="131,072", background=Color3.fromRGB(128,32,32), textColor=Color3.fromRGB(255,255,255), textSize=22},
	[18] = {text="262,144", background=Color3.fromRGB(128,32,128), textColor=Color3.fromRGB(255,255,255), textSize=22},
	[19] = {text="524,288", background=Color3.fromRGB(32,128,32), textColor=Color3.fromRGB(255,255,255), textSize=22},
	[20] = {text="1,048,576", background=Color3.fromRGB(32,128,128), textColor=Color3.fromRGB(255,255,255), textSize=20},
}

-- =====================================================================
-- 共通関数

-- デクリメント操作
-- @param n デクリメント対象数値
-- @param m 減算量
-- @return デクリメント結果
-- note:
-- 0でクランプ
function Constants.Dec(n, m)
	m = m or 1
	if n <= m then
		return 0
	end
	return n - m
end

-- 数値 -> 文字列変換のフォーマット操作
-- @param n フォーマット対象数値
-- @return フォーマット文字列
function Constants.FormatNumber(n)
	n = math.floor(n or 0)
	local s = tostring(n)
	while true do
		local ns, c = s:gsub("^(-?%d+)(%d%d%d)", "%1,%2")
		s = ns
		if c == 0 then
			break
		end
	end
	return s
end

-- ブロックに表示する文字列を返す
-- @param n ブロックの値
-- @return 文字列
-- note:
-- テーブルで定義された文字列を返すが、テーブルの上限を超えた場合は、
-- 単純にブロックの値で2のべき乗をした数値を返す。
function Constants.FormatPowValue(pow)
	local style = Constants.TILE_STYLES[pow]
	if style then
		return style.text
	end
	return Constants.FormatNumber(2 ^ pow)
end

-- 時間の形式に成形された文字列を返す
-- @param frames 時間情報
-- @return 文字列
function Constants.FormatFrames(frames)
	if frames == nil or frames < 0 then
		return "--:--:--.--"
	end

	local f = math.floor(frames)
	local hour = math.floor(f / (60 * 60 * 60))
	f %= (60 * 60 * 60)

	local min = math.floor(f / (60 * 60))
	f %= (60 * 60)

	local sec = math.floor(f / 60)
	f %= 60

	local frac = math.floor(f * 100 / 60)

	return string.format("%d:%02d:%02d.%02d", hour, min, sec, frac)
end

return Constants
