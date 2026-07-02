--[[====================================================================
 Copyright 2026 Miyabi
 NextBoxModel.luau
 NEXTキュー管理クラス
 次に出現するブロックを固定長キューで管理する
 C++版相当クラス: CRNextDat
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui
  +- CRMidSnd
  +- CRAppli
      +- CRCellDat
      +- CRNextDat === NextBoxModel
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
local Constants = require(script.Parent.Constants)

local NextBoxModel = {}
NextBoxModel.__index = NextBoxModel

-- =====================================================================
-- ローカル定数への再定義
local N = Constants.NEXT_BOX_NUM				-- NEXTキューのセル数
local VALUE_NUM = Constants.NEXT_BOX_VALUE_NUM	-- NEXTキューのセルが格納する値の範囲
local EMPTY = Constants.EMPTY					-- NEXTキューの空セルを表す値

-- =====================================================================
-- コンストラクタ
-- @param none
-- @return NextBoxModelインスタンス
function NextBoxModel.new()
	local self = setmetatable({}, NextBoxModel)

	self.boxValueTimer = VALUE_NUM	-- NEXTキュー格納値算出用の内部カウンタ
	self.nextBoxCount = 0			-- NEXTキュー格納値の算出回数
	self.values = {}

	-- NEXTキューを全て空セルにする
	for i = 0, N - 1 do
		self.values[i] = EMPTY
	end

	-- NEXTキューに初期値を格納する
	for i = 0, N - 1 do
		self.values[i] = self.nextBoxCount	-- NEXTキュー格納値
		self.nextBoxCount += 1				-- NEXTキュー格納値の算出回数 +1
	end

	return self
end

-- =====================================================================
-- NEXTキューの格納値を算出する
-- @param none
-- @return NEXTキューの格納値
-- note:
-- 初回の[VALUE_NUM]回目までは、2^0,2^1...2^nの格納値を順に算出する
-- [VALUE_NUM]回目以降は、内部タイマに基づく周期的疑似乱数で格納値を算出する
function NextBoxModel:GetNextBoxNum()
	local ret

	if self.nextBoxCount < VALUE_NUM then
		ret = self.nextBoxCount
	else
		ret = self.boxValueTimer % VALUE_NUM
	end

	self.nextBoxCount += 1

	return ret
end

-- =====================================================================
-- NEXTキュー更新
-- @param none
-- @return none
function NextBoxModel:Update()
	-- boxValueTimerは周期的な値生成に使用される内部クロック
	self.boxValueTimer = Constants.Dec(self.boxValueTimer)

	if self.boxValueTimer == 0 then
		self.boxValueTimer = VALUE_NUM
	end
end

-- =====================================================================
-- NEXTキューの格納値を設定する
-- @param i NEXTキューの位置
-- @param v NEXTキューの格納値
-- @return none
function NextBoxModel:SetVal(i, v)
	self.values[i] = v
end

-- =====================================================================
-- NEXTキューの格納値を取得する
-- @param i NEXTキューの位置
-- @return NEXTキューの格納値
function NextBoxModel:GetVal(i)
	return self.values[i] or EMPTY
end

-- =====================================================================
-- NEXTキューの格納値の算出回数を取得する
-- @param none
-- @return NEXTキューの格納値の算出回数
function NextBoxModel:GetNextBoxCnt()
	return self.nextBoxCount
end

-- =====================================================================
-- NEXTキューの格納値の算出回数を設定する
-- @param v NEXTキューの格納値の算出回数
-- @return none
function NextBoxModel:SetNextBoxCnt(v)
	self.nextBoxCount = v or 0
end

-- =====================================================================
-- NEXTキュー格納値算出用の内部カウンタを取得する
-- @param none
-- @return NEXTキュー格納値算出用の内部カウンタ
function NextBoxModel:GetBoxValTm()
	return self.boxValueTimer
end

-- =====================================================================
-- NEXTキュー格納値算出用の内部カウンタを設定する
-- @param v NEXTキュー格納値算出用の内部カウンタ
-- @return none
function NextBoxModel:SetBoxValTm(v)
	self.boxValueTimer = v or 6
end

-- =====================================================================
-- NEXTキュー格納値の取り出し指示
-- @param none
-- @return NEXTキューから取り出した格納値
-- note:
-- QUEUEを先頭から1つ取り出し、全体をシフトして末尾に新規値を追加する。
function NextBoxModel:ExecPush()
	local ret = self.values[0]

	for i = 0, N - 2 do
		self.values[i] = self.values[i + 1]
	end

	self.values[N - 1] = self:GetNextBoxNum()

	return ret
end

-- =====================================================================
-- NEXTキューデータのシリアライザ
-- @param none
-- @return シリアライズされたデータ
-- note:
-- データセーブ用のメソッド
function NextBoxModel:Serialize()
	local values = {}
	for i = 0, N - 1 do
		values[tostring(i)] = self.values[i]
	end

	return {
		values = values,
		nextBoxCount = self.nextBoxCount,
		boxValueTimer = self.boxValueTimer,
	}
end

-- =====================================================================
-- NEXTキューデータのデシリアライザ
-- @param data シリアライズされたデータ
-- @return デシリアライザが成功した場合はtrueを返す
-- note:
-- データロード用のメソッド
function NextBoxModel:Deserialize(data)
	if not data then
		return false
	end

	for i = 0, N - 1 do
		if data.values then
			self.values[i] = data.values[tostring(i)] or EMPTY
		else
			self.values[i] = EMPTY
		end
	end

	self.nextBoxCount = data.nextBoxCount or 0
	self.boxValueTimer = data.boxValueTimer or 6

	return true
end

return NextBoxModel
