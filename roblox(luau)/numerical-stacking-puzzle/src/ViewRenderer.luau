--[[====================================================================
 Copyright 2026 Miyabi
 ViewRenderer.luau
 描画オブジェクト管理クラス
 Roblox版では[NEW/BACK][LOAD][SAVE]ボタンの操作を追加する。
 C++版相当クラス: CRMidGui
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui === ViewRenderer
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
local Constants = require(script.Parent.Constants)

local ViewRenderer = {}
ViewRenderer.__index = ViewRenderer

-- =====================================================================
-- コンストラクタ
-- @param playerGui 親GUIインスタンス
-- @return ViewRendererインスタンス
function ViewRenderer.new(playerGui)
	local self = setmetatable({}, ViewRenderer)

	self.playerGui = playerGui

	-- スクリーンGUI生成
	self.gui = Instance.new("ScreenGui")
	self.gui.Name = "gui(ScreenGui)"
	self.gui.IgnoreGuiInset = true
	self.gui.ResetOnSpawn = false
	self.gui.Parent = playerGui

	-- 画面全体を覆うフレームの生成(基底部)
	self.root = Instance.new("Frame")
	self.root.Name = "root(Frame)"
	self.root.Size = UDim2.fromScale(1, 1)
	self.root.AnchorPoint = Vector2.new(0.5, 0.5)
	self.root.Position = UDim2.fromScale(0.5, 0.5)
	self.root.BackgroundColor3 = Color3.fromRGB(245, 245, 245)
	self.root.BorderSizePixel = 0
	self.root.Parent = self.gui

	return self
end

-- =====================================================================
-- 描画オブジェクトのクリア
-- @param none
-- @return none
function ViewRenderer:Clear()
	-- 子・孫の描画オブジェクトがあれば破棄する
	for _, child in ipairs(self.root:GetChildren()) do
		child:Destroy()
	end
end

-- =====================================================================
-- 操作ボタン表示の生成
-- @param callbacks ゲーム全体の進行管理クラスのインスタンス
-- @return none
-- note:
-- 操作ボタン操作時に呼出するコールバックの接続も行うため、
-- 引数でcallbacksを受け取っている。
function ViewRenderer:ShowButton(callbacks)
	-- 描画オブジェクトのクリア
	self:Clear()

	-- [NEW/BACK][LOAD][SAVE]ボタン生成([NEW/BACK]ボタンは[NEW]表示)
	self:CreateButtons(callbacks.gameController)
	self:SetNewOrBackButtonNew()
end

-- =====================================================================
-- 操作ボタン生成
-- @param gameController ゲーム全体の進行管理クラスのインスタンス
-- @return none
-- note:
-- 操作ボタンは左側に縦並び配置(75,150+250*i 固定レイアウト)
function ViewRenderer:CreateButtons(gameController)
	self.buttons = {}

	local buttonNames = {"NEW/BACK", "LOAD", "SAVE"}

	for i, name in ipairs(buttonNames) do
		local btn = Instance.new("TextButton")
		btn.Size = UDim2.fromScale(250 / Constants.BASE_WIDTH, 100 / Constants.BASE_HEIGHT)
		btn.Position = UDim2.fromScale(75 / Constants.BASE_WIDTH, (150 + (i-1)*250) / Constants.BASE_HEIGHT)
		btn.Text = name
		btn.BackgroundColor3 = Color3.fromRGB(80, 80, 80)
		btn.TextColor3 = Color3.new(1,1,1)
		btn.Font = Enum.Font.GothamBold
		btn.TextScaled = true
		-- "btn.TextSize = xx" は不要。TextScaledでサイズは設定する。
		btn.ZIndex = 30
		btn.Parent = self.root

		self.buttons[name] = btn
	end

	-- コールバックイベント接続
	self.buttons["NEW/BACK"].Activated:Connect(function()
		gameController:OnNewOrBackButton()
	end)

	self.buttons["LOAD"].Activated:Connect(function()
		gameController:OnLoadButton()
	end)

	self.buttons["SAVE"].Activated:Connect(function()
		gameController:OnSaveButton()
	end)
end

-- =====================================================================
-- [NEW/BACK]ボタンの表示を[NEW]にする
-- @param none
-- @return none
function ViewRenderer:SetNewOrBackButtonNew()
	self.buttons["NEW/BACK"].Text = "NEW"
end

-- =====================================================================
-- [NEW/BACK]ボタンの表示を[BACK]にする
-- @param none
-- @return none
function ViewRenderer:SetNewOrBackButtonBack()
	self.buttons["NEW/BACK"].Text = "BACK"
end

-- =====================================================================
-- ボタンの操作の有効/無効を設定する
-- @param name 操作対象のボタン
-- @param enabled 有効の場合はtrueを指定
-- @return none
function ViewRenderer:SetButtonEnabled(name, enabled)
	local btn = self.buttons[name]
	if btn then
		btn.Active = enabled
		btn.AutoButtonColor = enabled
		btn.TextTransparency = enabled and 0 or 0.5
	end
end

return ViewRenderer
