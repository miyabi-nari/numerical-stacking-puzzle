--[[====================================================================
 Copyright 2026 Miyabi
 InputController.luau
 キーボード・マウス・タッチ・UIボタンを一元管理し、
 「押された瞬間（トリガ）」を提供する
 C++版相当クラス: CRMidKey
 CRMainFrm
  +- CTimerLib
  +- CRMidKey === InputController
  +- CRMidGui
  +- CRMidSnd
  +- CRAppli
      +- CRCellDat
      +- CRNextDat
      +- CRec
      +- CRTitleLblSet
      |   +- CRTitleLbl
      +- CRAppBoxSet
      |   +- CRAppBox
      +- CRAppNextBoxSet
      |   +- CRAppBox
      +- CRAppKeyIndSet
      |   +- CRAppKeyInd
      +- CRAppLblSet
      |   +- CRAppLbl
      +- CRAppBox(FallingBox)
======================================================================]]
-- from NSP_Client.client, require(NSP.GameController)
-- note: robloxのlua言語仕様
-- ソースファイルはrequireされたときにファイル先頭から実行される。
-- 途中のlocal, functionは変数、関数の登録という扱いになる。
-- ソース末尾のreturnは、require呼出元にモジュールテーブルを返している。
local UserInputService = game:GetService("UserInputService")

local InputController = {}
InputController.__index = InputController

-- =====================================================================
-- ローカル定数の定義
local QWERTY_NUM = 11	-- キーボード入力配列数(盤面横幅に対応 CELL_X_NUMと一致)

-- 盤面Xに対応するキーボード入力リスト
-- note:
-- Robloxは予約機能の割り当てられたキーがあるため、C++版とは異なるキー列にしている。
local keyToIndex = {
	[Enum.KeyCode.A] = 0,			-- 盤面X= 0対応キー: A
	[Enum.KeyCode.S] = 1,			-- 盤面X= 1対応キー: S
	[Enum.KeyCode.D] = 2,			-- 盤面X= 2対応キー: D
	[Enum.KeyCode.F] = 3,			-- 盤面X= 3対応キー: F
	[Enum.KeyCode.G] = 4,			-- 盤面X= 4対応キー: G
	[Enum.KeyCode.H] = 5,			-- 盤面X= 5対応キー: H
	[Enum.KeyCode.J] = 6,			-- 盤面X= 6対応キー: J
	[Enum.KeyCode.K] = 7,			-- 盤面X= 7対応キー: K
	[Enum.KeyCode.L] = 8,			-- 盤面X= 8対応キー: L
	[Enum.KeyCode.Semicolon] = 9,	-- 盤面X= 9対応キー: Semicolon
	[Enum.KeyCode.Quote] = 10,		-- 盤面X=10対応キー: Quote
}

-- =====================================================================
-- コンストラクタ
-- @param none
-- @return InputControllerインスタンス
function InputController.new()
	local self = setmetatable({}, InputController)

	-- キー入力を全てOFFにする
	self.keyOn = {
		mouseL = false,	-- マウス左クリック(未使用 将来拡張用)
		mouseR = false,	-- マウス右クリック(未使用 将来拡張用)
		qwerty = {},	-- キーボード盤面対応キー
		one = false,	-- キーボード上の[1]
		two = false,	-- キーボード上の[2]
		three = false,	-- キーボード上の[3]
		fn = {},		-- ファンクションキー(未使用 将来拡張用)
	}

	self.keyDown = {
		mouseL = false,	-- マウス左クリック(未使用 将来拡張用)
		mouseR = false,	-- マウス右クリック(未使用 将来拡張用)
		qwerty = {},	-- キーボード盤面対応キー
		one = false,	-- キーボード上の[1]
		two = false,	-- キーボード上の[2]
		three = false,	-- キーボード上の[3]
		fn = {},		-- ファンクションキー(未使用 将来拡張用)
	}

	-- columnDownFromPointerはマウス/タッチで入力された列（デバッグや将来拡張用）
	-- 現在はkeyDown.qwertyと併用し、直接は参照していない
	self.columnDownFromPointer = nil

	for i = 0, QWERTY_NUM - 1 do
		self.keyOn.qwerty[i] = false
		self.keyDown.qwerty[i] = false
	end

	-- ファンクションキーは将来のショートカット拡張用
	for i = 1, 12 do
		self.keyOn.fn[i] = false
		self.keyDown.fn[i] = false
	end

	return self
end

-- =====================================================================
-- 入力デバイスとの接続
-- @param none
-- @return none
-- note:
-- マウス入力、ファンクションキー入力は使用していない。
-- マウス入力（＆タップ）は画面上のGUIオブジェクト(ボタン)への操作を
-- ViewRendererが拾い上げて、InputControllerに渡している。(InjectColumnDown関数)
function InputController:Bind()
	-- 入力オン検出時のコールバック定義
	UserInputService.InputBegan:Connect(function(input, processed)
		-- UI要素が入力を消費した場合は無視
		-- ※UIクリックで入力を拾いたい場合は、この条件に注意
		if processed then
			return
		end

		-- キーボード盤面対応キー
		local index = keyToIndex[input.KeyCode]
		if index ~= nil then
			if not self.keyOn.qwerty[index] then
				self.keyDown.qwerty[index] = true
			end
			self.keyOn.qwerty[index] = true
			return
		end

		-- キーボード上の[1]
		if input.KeyCode == Enum.KeyCode.One then
			if not self.keyOn.one then
				self.keyDown.one = true
			end
			self.keyOn.one = true
			return
		end

		-- キーボード上の[2]
		if input.KeyCode == Enum.KeyCode.Two then
			if not self.keyOn.two then
				self.keyDown.two = true
			end
			self.keyOn.two = true
			return
		end

		-- キーボード上の[3]
		if input.KeyCode == Enum.KeyCode.Three then
			if not self.keyOn.three then
				self.keyDown.three = true
			end
			self.keyOn.three = true
			return
		end

		-- マウス左クリック(未使用 将来拡張用)
		-- マウス入力は直接使用せず、ViewRenderer経由で扱う
		if input.UserInputType == Enum.UserInputType.MouseButton1 then
			if not self.keyOn.mouseL then
				self.keyDown.mouseL = true
			end
			self.keyOn.mouseL = true
		end
	end)

	-- 入力オフ検出時のコールバック定義
	UserInputService.InputEnded:Connect(function(input)
		-- キーボード盤面対応キー
		local index = keyToIndex[input.KeyCode]
		if index ~= nil then
			self.keyOn.qwerty[index] = false
			return
		end

		-- キーボード上の[1]
		if input.KeyCode == Enum.KeyCode.One then
			self.keyOn.one = false
			return
		end

		-- キーボード上の[2]
		if input.KeyCode == Enum.KeyCode.Two then
			self.keyOn.two = false
			return
		end

		-- キーボード上の[3]
		if input.KeyCode == Enum.KeyCode.Three then
			self.keyOn.three = false
			return
		end

		-- マウス左クリック(未使用 将来拡張用)
		-- マウス入力は直接使用せず、ViewRenderer経由で扱う
		if input.UserInputType == Enum.UserInputType.MouseButton1 then
			self.keyOn.mouseL = false
		end
	end)
end

-- =====================================================================
-- ViewRendererからのGUIオブジェクトに紐づいた入力の通知
-- @param column 入力検出された盤面の横方向セル位置
-- @return none
-- note:
-- マウス入力（＆タップ）は画面上のGUIオブジェクト(ボタン)への操作を
-- ViewRendererが拾い上げ、この関数を呼ぶことでInputControllerに渡している。
-- マウス入力（＆タップ）は１回きりの入力としてkeyDownのみセットする。
-- (keyOnではなくkeyDownのみセットすることで1フレームのトリガ入力として扱える)
function InputController:InjectColumnDown(column)
	if column >= 0 and column < QWERTY_NUM then
		self.keyDown.qwerty[column] = true
	end
end

-- =====================================================================
-- キー入力オン情報の取得
-- @param none
-- @return キー入力オン情報
function InputController:GetKeyOn()
	return self.keyOn
end

-- =====================================================================
-- キー入力オフ→オン情報の取得
-- @param none
-- @return キー入力オフ→オン情報
function InputController:GetKeyDown()
	return self.keyDown
end

-- =====================================================================
-- キー入力オフ→オン情報のクリア
-- @param none
-- @return none
-- note:
-- 周期処理の終端での呼出を想定。
-- キー入力オフ→オン情報を次回周期処理に持ち越さないためのクリア処理。
-- 入力は非同期イベントで発生するため、EndFrameでリセットしないと
-- 押しっぱなし扱いになる。
function InputController:EndFrame()
	for i = 0, QWERTY_NUM - 1 do
		self.keyDown.qwerty[i] = false
	end

	self.keyDown.one = false
	self.keyDown.two = false
	self.keyDown.three = false
	self.keyDown.mouseL = false
	self.columnDownFromPointer = nil
end

return InputController
