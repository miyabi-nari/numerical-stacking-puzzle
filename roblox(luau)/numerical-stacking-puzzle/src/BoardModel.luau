--[[====================================================================
 Copyright 2026 Miyabi
 BoardModel.luau
 盤面データとゲームルールを管理するModelクラス

 ・セル値の保持
 ・スタック判定／実行
 ・マージ判定／実行
 ・落下着地補正判定／実行
 ・スコア計算

 C++版 CRCellDat のロジックをベースに移植
 C++版相当クラス: CRCellDat
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui
  +- CRMidSnd
  +- CRAppli
      +- CRCellDat === BoardModel
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
local Constants = require(script.Parent.Constants)

local BoardModel = {}
BoardModel.__index = BoardModel

-- =====================================================================
-- ローカル定数への再定義
local W = Constants.CELL_X_NUM	-- 盤面の横方向セル数
local H = Constants.CELL_Y_NUM	-- 盤面の縦方向セル数
local EMPTY = Constants.EMPTY	-- 盤面の空セルを表す値

local MAX_SAFE_EXP = 30			-- 得点計算の制限値

-- 4方向DFS探索用ベクトル(マージ探索に使用する)
local DIRS = {
	{-1, 0},
	{ 1, 0},
	{ 0,-1},
	{ 0, 1},
}

-- =====================================================================
-- コンストラクタ
-- @param none
-- @return BoardModelインスタンス
function BoardModel.new()
	local self = setmetatable({}, BoardModel)
	self.cells = {}
	self.score = 0	-- スコア

	-- 盤面を全て空セルにする
	for x = 0, W - 1 do
		self.cells[x] = {}
		for y = 0, H - 1 do
			self.cells[x][y] = {
				val = EMPTY,		-- セル格納値
				stackVal = EMPTY,	-- スタック処理値
				mergeX = -1,		-- マージ位置X
				mergeY = -1,        -- マージ位置Y
				mergeVal = EMPTY,   -- マージ処理値
				landY = -1,         -- 落下着地位置Y
				landVal = EMPTY,    -- 落下着地情報(将来拡張用 C++互換)
			}
		end
	end

	return self
end

-- =====================================================================
-- 引数範囲チェック用関数
-- @param x,y 盤面上のセル位置X,Y
-- @return 引数のセル位置が盤面の範囲内であればtrueを返す
function BoardModel:IsValid(x, y)
	return x >= 0 and x < W and y >= 0 and y < H
end

-- =====================================================================
-- セルに格納値を設定
-- @param x,y 盤面上のセル位置X,Y
-- @param value セルの格納値
-- @return none
function BoardModel:SetVal(x, y, value)
	if self:IsValid(x, y) then
		self.cells[x][y].val = value
	end
end

-- =====================================================================
-- セルの格納値を取得
-- @param x,y 盤面上のセル位置X,Y
-- @return セルの格納値
function BoardModel:GetVal(x, y)
	if not self:IsValid(x, y) then
		return EMPTY
	end
	return self.cells[x][y].val
end

-- =====================================================================
-- スタック処理値を取得
-- @param x,y 盤面上のセル位置X,Y
-- @return スタック処理値
function BoardModel:GetStackVal(x, y)
	if not self:IsValid(x, y) then
		return EMPTY
	end
	return self.cells[x][y].stackVal
end

-- =====================================================================
-- マージするセルの値を取得
-- @param x,y 盤面上のセル位置X,Y
-- @return マージするセルの値
function BoardModel:GetMergeVal(x, y)
	if not self:IsValid(x, y) then
		return EMPTY
	end
	return self.cells[x][y].mergeVal
end

-- =====================================================================
-- マージ位置(X方向)を取得
-- @param x,y 盤面上のセル位置X,Y
-- @return マージ位置X
function BoardModel:GetMergeX(x, y)
	if not self:IsValid(x, y) then
		return -1
	end
	return self.cells[x][y].mergeX
end

-- =====================================================================
-- マージ位置(Y方向)を取得
-- @param x,y 盤面上のセル位置X,Y
-- @return マージ位置Y
function BoardModel:GetMergeY(x, y)
	if not self:IsValid(x, y) then
		return -1
	end
	return self.cells[x][y].mergeY
end

-- =====================================================================
-- 落下着地位置(Y方向)を取得
-- @param x,y 盤面上のセル位置X,Y
-- @return 落下着地位置Y
function BoardModel:GetLandY(x, y)
	if not self:IsValid(x, y) then
		return -1
	end
	return self.cells[x][y].landY
end

-- =====================================================================
-- スタック処理値の情報をすべてクリア
-- @param none
-- @return none
function BoardModel:ClearStackInfo()
	for x = 0, W - 1 do
		for y = 0, H - 1 do
			self.cells[x][y].stackVal = EMPTY
		end
	end
end

-- =====================================================================
-- マージするセルの値,マージ位置の情報をすべてクリア
-- @param none
-- @return none
function BoardModel:ClearMergeInfo()
	for x = 0, W - 1 do
		for y = 0, H - 1 do
			local c = self.cells[x][y]
			c.mergeX = -1
			c.mergeY = -1
			c.mergeVal = EMPTY
		end
	end
end

-- =====================================================================
-- 落下着地位置の情報をすべてクリア
-- @param none
-- @return none
function BoardModel:ClearLandInfo()
	for x = 0, W - 1 do
		for y = 0, H - 1 do
			self.cells[x][y].landY = -1
		end
	end
end

-- =====================================================================
-- セル格納値の積み上げ可否を検査
-- @param x,y 盤面上のセル位置X,Y
-- @param value セルの格納値
-- @return 積み上げ可否。可の場合はtrueを返す
-- note:
-- 検査前にスタック処理値の情報をすべてクリアする。
-- 積み上げ可の場合はスタック処理値の情報をセットしてtrueを返す
-- (積み上げ可否のルール)
-- 引数で渡されたセル位置が最下段の場合＝積み上げ可
-- 引数で渡されたセル位置の直下が空セルではない場合
--   -> 最上段以外＝積み上げ可
--   -> 最上段でも直下のセルとマージ可能なら＝積み上げ可
function BoardModel:CheckEnStack(x, y, value)
	-- スタック処理値の情報をすべてクリア
	self:ClearStackInfo()

	if y >= H - 1 then
		self.cells[x][y].stackVal = value
		return true
	end

	local below = self.cells[x][y + 1].val

	if below >= 0 then
		if y >= 1 then
			self.cells[x][y].stackVal = value
			return true
		else
			if value == below then
				self.cells[x][y].stackVal = value
				return true
			end
		end
	end

	return false
end

-- =====================================================================
-- セル格納値の積み上げを実行
-- @param none
-- @return none
-- note:
-- 上位層から、BoardModel:CheckEnStackの呼び出し->trueの場合に、
-- 上位層が続けて呼び出す想定。
function BoardModel:ExecStack()
	for x = 0, W - 1 do
		for y = 0, H - 1 do
			local c = self.cells[x][y]
			if c.stackVal >= 0 then
				c.val = c.stackVal
				c.stackVal = EMPTY
			end
		end
	end
end

-- =====================================================================
-- セル格納値のマージ可否を検査
-- @param x,y 盤面上のセル位置X,Y
-- @return マージ可否。可の場合はtrueを返す
-- @return 最大マージ値(マージする値の中で最大の格納値)
-- @return マージ規模(マージするセルの数)
-- note:
-- 検査前にマージするセルの値,マージ位置の情報をすべてクリアする。
-- マージ可の場合はマージの情報をセットしてtrueを返す
-- (マージ可否のルール)
-- 検査対象セルの上下左右に隣接するセルに同じ値がある場合＝マージ可
-- マージ時のセルの移動先は、引数で渡したセル位置により近い方とする。
function BoardModel:CheckMerge(startX, startY)
	-- マージするセルの値,マージ位置の情報をすべてクリア
	self:ClearMergeInfo()

	local found = false

	-- 落下基準列から左右に分割して探索（優先順位制御）
	-- 右半分の探索
	for x = startX, W - 1 do
		for y = 0, H - 1 do
			if self:CheckMergeSub(x, y, x, y) > 1 then
				found = true
			end
		end
	end

	-- 左半分の探索
	for x = startX - 1, 0, -1 do
		for y = 0, H - 1 do
			if self:CheckMergeSub(x, y, x, y) > 1 then
				found = true
			end
		end
	end

	local maxMergeVal = -1
	local maxMergeDifVal = -1

	if found then
		for x = 0, W - 1 do
			for y = 0, H - 1 do
				local c = self.cells[x][y]
				if c.val < c.mergeVal then
					maxMergeVal = math.max(maxMergeVal, c.mergeVal)
					maxMergeDifVal = math.max(maxMergeDifVal, c.mergeVal - c.val)
				end
			end
		end
	end

	return found, maxMergeVal, maxMergeDifVal
end

-- =====================================================================
-- セル格納値のマージ可否を検査するサブ関数
-- @param x,y 盤面上のセル位置X,Y(探索先)
-- @param baseX,baseY 盤面上のセル位置X,Y(ベース位置)
-- @return マージ規模。マージ不成立時は0を返す。
-- note:
-- baseセルと同じ値の連結数を数え、
-- 基準セルには最終的な MergeVal を設定する。
function BoardModel:CheckMergeSub(x, y, baseX, baseY)
	local cell = self.cells[x][y]
	local baseCell = self.cells[baseX][baseY]

	local count = 0

	if cell.val ~= EMPTY and cell.mergeX == -1 and cell.mergeY == -1 then
		if cell.val == baseCell.val then
			count = 1
			cell.mergeX = baseX
			cell.mergeY = baseY

			-- 4方向探索
			for _, d in ipairs(DIRS) do
				local nx = x + d[1]
				local ny = y + d[2]
				if self:IsValid(nx, ny) then
					count += self:CheckMergeSub(nx, ny, baseX, baseY)
				end
			end

			-- 基準セルだけ最終的な MergeVal を設定する。
			if x == baseX and y == baseY then
				cell.mergeVal = cell.val + count - 1
			end
		end
	end

	return count
end

-- =====================================================================
-- セル格納値のマージを実行
-- @param none
-- @return none
-- note:
-- 上位層から、BoardModel:CheckMergeの呼び出し->trueの場合に、
-- 上位層が続けて呼び出す想定。
function BoardModel:ExecMerge()
	for x = 0, W - 1 do
		for y = 0, H - 1 do
			local c = self.cells[x][y]
			c.val = c.mergeVal
			c.mergeX = -1
			c.mergeY = -1
			c.mergeVal = EMPTY
		end
	end
end

-- =====================================================================
-- セル格納値の落下着地可否を検査
-- @param none
-- @return 落下着地可否。可の場合はtrueを返す
-- note:
-- 検査前に落下着地位置の情報をすべてクリアする。
-- 落下着地可の場合は落下着地位置の情報をセットしてtrueを返す
-- (落下着地可否のルール)
-- 検査対象セルの直下が空セルの場合＝落下着地可
-- 直下の下にさらに空セルが連続する場合は、より低い方とする。
function BoardModel:CheckEnLand()
	-- 落下着地位置の情報をすべてクリア
	self:ClearLandInfo()

	local found = false

	for x = 0, W - 1 do
		for y = H - 1, 0, -1 do
			if self:CheckEnLandSub(x, y) then
				found = true
			end
		end
	end

	return found
end

-- =====================================================================
-- セル格納値の落下着地可否を検査するサブ関数
-- @param x,y 盤面上のセル位置X,Y(探索先)
-- @return 落下着地可否。可の場合はtrueを返す
-- note:
-- 検査対象セルの直下が空セルの場合＝落下着地可
-- 直下の下にさらに空セルが連続する場合は、より低い方とする。
function BoardModel:CheckEnLandSub(x, y)
	local cell = self.cells[x][y]

	if cell.val == EMPTY then
		return false
	end

	if y >= H - 1 then
		return false
	end

	for checkY = y + 1, H - 1 do
		local below = self.cells[x][checkY]

		if below.val == EMPTY then
			cell.landY = checkY
			return true
		else
			if below.landY >= 0 then
				cell.landY = below.landY - 1
				return true
			end
			break
		end
	end

	return false
end

-- =====================================================================
-- セル格納値の落下着地を実行
-- @param none
-- @return none
-- note:
-- 上位層から、BoardModel:CheckEnLandの呼び出し->trueの場合に、
-- 上位層が続けて呼び出す想定。
function BoardModel:ExecLand()
	for x = 0, W - 1 do
		for y = H - 1, 0, -1 do
			local c = self.cells[x][y]
			local ly = c.landY

			if ly ~= -1 then
				self.cells[x][ly].val = c.val
				c.val = EMPTY
			end

			c.landY = -1
		end
	end
end

-- =====================================================================
-- 得点計算
-- @param none
-- @return none
-- note:
-- 得点は空セルを除く全セルの格納値の合計。
-- 格納値はPow(2,格納値)に換算して得点集計する。
function BoardModel:CalcScore()
	local score = 0

	for x = 0, W - 1 do
		for y = 0, H - 1 do
			local v = self.cells[x][y].val
			if v >= 0 and v <= MAX_SAFE_EXP then
				score += 2 ^ v
			end
		end
	end

	self.score = score
	return true
end

-- =====================================================================
-- 得点を取得
-- @param none
-- @return 得点
function BoardModel:GetScore()
	return self.score
end

-- =====================================================================
-- 得点を設定
-- @param none
-- @return 設定
function BoardModel:SetScore(score)
	self.score = score or 0
end

-- =====================================================================
-- 盤面データのシリアライザ
-- @param none
-- @return シリアライズされたデータ
-- note:
-- データセーブ用のメソッド
function BoardModel:Serialize()
	local data = {
		score = self.score,
		cells = {},
	}

	for x = 0, W - 1 do
		data.cells[tostring(x)] = {}
		for y = 0, H - 1 do
			local c = self.cells[x][y]
			data.cells[tostring(x)][tostring(y)] = {
				val = c.val,
				stackVal = c.stackVal,
				mergeX = c.mergeX,
				mergeY = c.mergeY,
				mergeVal = c.mergeVal,
				landY = c.landY,
				landVal = c.landVal,
			}
		end
	end

	return data
end

-- =====================================================================
-- 盤面データのデシリアライザ
-- @param data シリアライズされたデータ
-- @return デシリアライザが成功した場合はtrueを返す
-- note:
-- データロード用のメソッド
function BoardModel:Deserialize(data)
	if not data or not data.cells then
		return false
	end

	for x = 0, W - 1 do
		for y = 0, H - 1 do
			local col = data.cells[tostring(x)]
			local src = col and col[tostring(y)]
			local c = self.cells[x][y]

			if src then
				c.val = src.val or EMPTY
				c.stackVal = src.stackVal or EMPTY
				c.mergeX = src.mergeX or -1
				c.mergeY = src.mergeY or -1
				c.mergeVal = src.mergeVal or EMPTY
				c.landY = src.landY or -1
				c.landVal = src.landVal or EMPTY
			else
				c.val = EMPTY
				c.stackVal = EMPTY
				c.mergeX = -1
				c.mergeY = -1
				c.mergeVal = EMPTY
				c.landY = -1
				c.landVal = EMPTY
			end
		end
	end

	self.score = data.score or 0
	return true
end

return BoardModel
