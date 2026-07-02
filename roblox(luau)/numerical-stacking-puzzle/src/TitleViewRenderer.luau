--[[====================================================================
 Copyright 2026 Miyabi
 TitleViewRenderer.luau
 タイトル画面の描画オブジェクト管理クラス
 Roblox版では描画メソッドがコンパクトにまとまるため、
 C++のように分割せず、ある程度集約した構成にする。
 C++版相当クラス: CRTitleLblSet,CRTitleLbl
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui
  +- CRMidSnd
  +- CRAppli
      +- CRCellDat
      +- CRNextDat
      +- CRec
      +- CRTitleLblSet === ViewRenderer
      |   +- CRTitleLbl === ViewRenderer
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

local TitleViewRenderer = {}
TitleViewRenderer.__index = TitleViewRenderer

-- =====================================================================
-- コンストラクタ
-- @return TitleViewRendererインスタンス
function TitleViewRenderer.new()
	local self = setmetatable({}, TitleViewRenderer)

	self.titleFrame = nil	-- タイトル画面用GUI(この時点ではnil)

	return self
end

-- =====================================================================
-- 描画オブジェクトのクリア
-- @param none
-- @return none
function TitleViewRenderer:Clear()
	-- 子・孫の描画オブジェクトがあれば破棄する
	if self.titleFrame then
		for _, child in ipairs(self.titleFrame:GetChildren()) do
			child:Destroy()
		end
	end

	self.titleFrame = nil
end

-- =====================================================================
-- タイトル画面の生成
-- @param parent 親フレームインスタンス(表示基準点となる)
-- @param soundManager サウンド再生管理クラスのインスタンス
-- @return none
-- note:
function TitleViewRenderer:ShowTitle(parent, soundManager)
	-- 描画オブジェクトのクリア
	self:Clear()

	-- タイトル画面用フレームの生成
	local frame = Instance.new("Frame")
	frame.Name = "titleFrame(Frame)"
	frame.Size = UDim2.fromScale(1, 1)
	frame.BackgroundTransparency = 1
	frame.Parent = parent
	self.titleFrame = frame

	-- ラベル生成関数の定義
	local function label(name, text, x, y, w, h, size, color)
		local l = Instance.new("TextLabel")
		l.Name = name
		l.BackgroundTransparency = 1
		l.Position = UDim2.fromScale(x / Constants.BASE_WIDTH, y / Constants.BASE_HEIGHT)
		l.Size = UDim2.fromScale(w / Constants.BASE_WIDTH, h / Constants.BASE_HEIGHT)
		l.Text = text
		l.TextColor3 = color or Color3.fromRGB(0,0,0)
		l.Font = Enum.Font.GothamBold
		l.TextScaled = true
		-- "l.TextSize = xx" は不要。TextScaledでサイズは設定する。
		l.TextXAlignment = Enum.TextXAlignment.Left
		l.TextYAlignment = Enum.TextYAlignment.Center
		l.Parent = frame
		return l
	end

	-- タイトル、バージョン、CopyRightの文字表示
	label("titleLabelAppName", Constants.APP_NAME, 100+400, 100, 1400, 100, 100)
	label("titleLabelAppVersion",Constants.APP_VERSION, 100+400, 200, 1400, 50, 25)
	label("titleLabelAppCopyRight", Constants.APP_COPYRIGHT, 100+400, 250, 1400, 50, 25)

	-- NewGame, RecentGame, サウンド設定の文字表示
	local newGame = label("titleLabelNewGame", "New Game ... Please press the [A] key.", 100+400, 350, 1400, 50, 50, Color3.fromRGB(128,128,128))
	local recent = label("titleLabelRecentGame", "Recent Game ... Please press the [S] key.", 100+400, 450, 1400, 50, 50, Color3.fromRGB(128,128,128))
	local soundLine = label("titleLabelSound", "Sound", 100+400, 700, 1400, 50, 50, Color3.fromRGB(128,128,128))

	-- サウンド設定の文字表示更新
	local function updateSoundText()
		if soundManager:IsEnabled() then
			soundLine.Text = "Sound Off ... Please press the [D] key. (now, ON)"
		else
			soundLine.Text = "Sound On ... Please press the [F] key. (now, OFF)"
		end
	end
	-- ここで一度関数を呼び出して表示更新しておく
	updateSoundText()

	-- updateSoundText関数は後でGameControllerからを呼出可能になるよう保持しておく
	self.titleSoundTextUpdater = updateSoundText

	-- タイトル画面全体をカバーするボタンを生成(マウスクリック・タップ操作の拾い上げ用)
	local invisibleButton = Instance.new("TextButton")
	invisibleButton.BackgroundTransparency = 1
	invisibleButton.Text = ""
	invisibleButton.Size = UDim2.fromScale(1, 1)
	invisibleButton.Parent = frame
	invisibleButton.ZIndex = 1

	-- Zオーダー設定(画面上で重複しないので同じ優先度)
	newGame.ZIndex = 2
	recent.ZIndex = 2
	soundLine.ZIndex = 2
end

return TitleViewRenderer

