--[[====================================================================
 Copyright 2026 Miyabi
 HudView.luau
 スコア・時間・マージ履歴などのテキスト表示を管理する
 Roblox版では描画メソッドがコンパクトにまとまるため、
 C++のように分割せず、ある程度集約した構成にする。
 C++版相当クラス: CRAppLblSet, CRAppLbl
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
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
      +- CRAppLblSet === HudView
      |   +- CRAppLbl === HudView
      +- CRAppBox(FallingBox)
======================================================================]]
-- from NSP_Client.client, require(NSP.GameController)
-- note: robloxのlua言語仕様
-- ソースファイルはrequireされたときにファイル先頭から実行される。
-- 途中のlocal, functionは変数、関数の登録という扱いになる。
-- ソース末尾のreturnは、require呼出元にモジュールテーブルを返している。
local Constants = require(script.Parent.Constants)

local HudView = {}
HudView.__index = HudView

-- =====================================================================
-- コンストラクタ
-- @param parent 親フレームインスタンス(表示基準点となる)
-- @return HudViewインスタンス
function HudView.new(parent)
	local self = setmetatable({}, HudView)

	self.parent = parent
	self.targetScore = 0
	self.displayScore = -1

	-- ラベル生成関数の定義
	local function makeLabel(name, text, x, y, w, h, size, left)
		local label = Instance.new("TextLabel")
		label.BackgroundTransparency = 1
		label.Position = UDim2.fromScale(x / Constants.BASE_WIDTH, y / Constants.BASE_HEIGHT)
		label.Size = UDim2.fromScale(w / Constants.BASE_WIDTH, h / Constants.BASE_HEIGHT)
		label.Text = text
		label.TextColor3 = Color3.fromRGB(128,128,128)
		label.Font = Enum.Font.GothamBold
		label.TextScaled = true
		-- "label.TextSize = xx" は不要。TextScaledでサイズは設定する。
		label.TextXAlignment = left and Enum.TextXAlignment.Left or Enum.TextXAlignment.Center
		label.TextYAlignment = Enum.TextYAlignment.Center
		label.Parent = parent
		return label
	end

	-- Next Block、Score、Elapsed-Time、Merged-Timeの文字表示
	self.nextHead = makeLabel("LabelNextBlock", "Next Block", 1200+400, 40, 400, 60, 50, true)
	self.scoreHead = makeLabel("LabelScore", "Score", 1200+400, 220, 400, 60, 50, true)
	self.timeHead = makeLabel("LabelElapsedTime", "Elapsed-Time", 1200+400, 400, 400, 60, 50, true)
	self.mergeHead = makeLabel("LabelMergedTime", "Merged-Time", 1200+400, 580, 400, 60, 50, true)

	-- Score、Elapsed-Time、Merged-Timeの数値表示(初期値は 0 or --:--:--.--)
	self.scoreVal = makeLabel("LabelScoreValue", "0", 1220+400, 280, 380, 60, 50, true)
	self.timeVal = makeLabel("LabelTimeValue", "--:--:--.--", 1220+400, 460, 380, 60, 50, true)
	self.mergeVals = {}
	for i = 1, 5 do
		self.mergeVals[i] = makeLabel("LabelMergeTimeValue"..i, "-", 1220+400, 650 + (i - 1) * 40, 380, 40, 30, true)
	end

	return self
end

-- =====================================================================
-- ヘッドアップ表示の更新
-- @param none
-- @return none
-- note:
-- スコアのみアニメーション表示（他は即時反映）
-- スコアの表示は、即時反映ではなく、1周期に1%ずつ徐々に追従させて表示する。
function HudView:Update()
	if self.displayScore < 0 then
		self.displayScore = 0
	end

	if self.displayScore < self.targetScore then
		local diff = self.targetScore - self.displayScore
		self.displayScore += math.ceil(diff / 100)
	elseif self.displayScore > self.targetScore then
		local diff = self.displayScore - self.targetScore
		self.displayScore -= math.ceil(diff / 100)
	end

	self.scoreVal.Text = Constants.FormatNumber(self.displayScore)
end

-- =====================================================================
-- 表示するスコアの設定
-- @param score スコア
-- @return none
function HudView:SetScore(score)
	self.targetScore = score or 0
end

-- =====================================================================
-- 表示する経過時間の設定
-- @param score 経過時間
-- @return none
function HudView:SetElapsedTime(frames)
	self.timeVal.Text = Constants.FormatFrames(frames)
end

-- =====================================================================
-- 表示するマージ記録の設定
-- @param times,values マージした経過時間、マージで生成した値
-- @return none
function HudView:SetMergeRecords(times, values)
	for i = 1, 5 do
		local idx0 = i - 1
		self:SetMergeTime(idx0, times[idx0] or -1, values[idx0] or -1)
	end
end

-- =====================================================================
-- マージ記録を文字列に変換してラベルに設定する
-- @param index0 ラベルのインデックス
-- @param frames マージ発生時の経過時間
-- @param pow マージで生成した値
-- @return none
function HudView:SetMergeTime(index0, frames, pow)
	local idx = index0 + 1
	if idx < 1 or idx > 5 then
		return
	end

	if pow and pow > 0 then
		self.mergeVals[idx].Text = Constants.FormatPowValue(pow) .. "..." .. Constants.FormatFrames(frames)
	else
		self.mergeVals[idx].Text = "-"
	end
end

return HudView
