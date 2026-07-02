--[[====================================================================
 Copyright 2026 Miyabi
 TileView.luau
 箱オブジェクト(タイル)管理クラス
 盤面表示・落下・演出の描画
 C++版相当クラス: CRAppBox
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
      |   +- CRAppBox === TileView
      +- CRAppNextBoxSet
      |   +- CRAppBox === TileView
      +- CRAppKeyIndSet
      |   +- CRAppKeyInd
      +- CRAppLblSet
      |   +- CRAppLbl
      +- CRAppBox(FallingBox) === TileView
======================================================================]]
-- from NSP_Client.client, require(NSP.GameController)
-- note: robloxのlua言語仕様
-- ソースファイルはrequireされたときにファイル先頭から実行される。
-- 途中のlocal, functionは変数、関数の登録という扱いになる。
-- ソース末尾のreturnは、require呼出元にモジュールテーブルを返している。
local TweenService = game:GetService("TweenService")
local Constants = require(script.Parent.Constants)

local TileView = {}
TileView.__index = TileView

-- =====================================================================
-- ローカル定数への再定義
local CELL_SIZE = Constants.CELL_SIZE			-- タイル１個のサイズ
local BOARD_OFFSET_X = Constants.BOARD_OFFSET_X	-- 盤面の横オフセット位置
local BOARD_OFFSET_Y = Constants.BOARD_OFFSET_Y	-- 盤面の縦オフセット位置
local TWEEN_TIME = Constants.TILE_TWEEN_TIME	-- スライド時間

-- =====================================================================
-- コンストラクタ
-- @param parent 親フレームインスタンス(表示基準点となる)
-- @param value タイルの数値(0以上の整数。表示値は2^valueとなる)
-- @param position 初期表示位置(親フレームからの相対位置。Type:UDim2の前提)
-- @return TileViewインスタンス
function TileView.new(parent, value, position)
	local self = setmetatable({}, TileView)

	self.value = value
	self.state = "Idle"	-- 停止中

	-- フレーム矩形のサイズ・位置・境界線の設定
	local frame = Instance.new("Frame")
	frame.Size = UDim2.fromScale(CELL_SIZE / Constants.BASE_WIDTH,
	                             CELL_SIZE / Constants.BASE_HEIGHT)
	frame.Position = position or UDim2.fromOffset(0, 0)
	frame.BorderSizePixel = 1
	frame.BorderColor3 = Color3.fromRGB(64,64,64)
	frame.ZIndex = 10
	frame.Parent = parent

	-- 文字列のサイズ・位置・フォント情報の設定
	local label = Instance.new("TextLabel")
	label.Size = UDim2.fromScale(1, 1)
	label.BackgroundTransparency = 1
	label.TextXAlignment = Enum.TextXAlignment.Center
	label.TextYAlignment = Enum.TextYAlignment.Center
	label.Font = Enum.Font.GothamBold
	label.ZIndex = 11
	label.Parent = frame

	-- フレーム矩形と文字列の設定を反映
	self.frame = frame
	self.label = label

	-- 表示文字列（＆矩形の色）を設定
	self:SetValue(value)

	return self
end

-- =====================================================================
-- ユーティリティ系・仮想空間座標上のピクセル座標->スケール座標変換
-- @param px,py 仮想空間座標上のピクセル座標 左上隅を(0,0)とする
-- @return Type:UDim2のスケール座標
local function posFromPixels(px, py)
	return UDim2.fromScale(px / Constants.BASE_WIDTH, py / Constants.BASE_HEIGHT)
end

-- =====================================================================
-- ユーティリティ系・盤面の座標->盤面表示位置への変換
-- @param x,y 盤面の座標 左上隅を(0,0)とする
-- @return Type:UDim2のスケール座標
function TileView.GridToPosition(x, y)
	return UDim2.fromScale(
		(BOARD_OFFSET_X + x * CELL_SIZE) / Constants.BASE_WIDTH,
		(BOARD_OFFSET_Y + y * CELL_SIZE) / Constants.BASE_HEIGHT
	)
end

-- =====================================================================
-- ユーティリティ系・NextBoxキュー上の位置->スケール座標変換
-- @param index NextBoxキュー上の位置 (0～4の整数)
-- @return Type:UDim2のスケール座標
function TileView.NextBoxPosition(index)
	return UDim2.fromScale(
		(Constants.NEXT_BOX_OFFSET_X + index * CELL_SIZE) / Constants.BASE_WIDTH,
		(Constants.NEXT_BOX_OFFSET_Y) / Constants.BASE_HEIGHT
	)
end

-- =====================================================================
-- 位置を設定
-- @param position 初期表示位置(親フレームからの相対位置。Type:UDim2の前提)
-- @return none
function TileView:SetPosition(position)
	self.frame.Position = position
end

-- =====================================================================
-- 位置を設定(盤面の座標指定)
-- @param x,y 盤面の座標 左上隅を(0,0)とする
-- @return none
-- note:
-- 盤面上の座標を表示位置に換算するため、
-- self.gameFrameを親フレームとしたTileViewに対して使用されることを
-- 前提としている。
function TileView:SetGridPosition(x, y)
	self.frame.Position = TileView.GridToPosition(x, y)
end

-- =====================================================================
-- 位置を設定
-- @param px,py 仮想空間座標上のピクセル座標 左上隅を(0,0)とする
-- @return none
-- note:
-- 仮想空間座標の幅はBOARD_OFFSET_X, BOARD_OFFSET_Yで定義されている
function TileView:SetPixelPosition(px, py)
	self.frame.Position = posFromPixels(px, py)
end

-- =====================================================================
-- スライド移動の開始設定
-- @param position スライド移動の移動先
-- @param onComplete スライド完了時のコールバック(省略時(nil)はコールバック無)
-- @return none
-- note:
-- TweenInfoを使用する。
-- TweenInfoはRoblox の Tween（補間アニメーション）の設定オブジェクト。
function TileView:TweenToPosition(position, onComplete)
	self.state = "Tween"	-- アニメーション中

	local tween = TweenService:Create(
		self.frame,
		TweenInfo.new(TWEEN_TIME, Enum.EasingStyle.Quad, Enum.EasingDirection.Out),
		{ Position = position }
	)

	tween.Completed:Once(function()
		self.state = "Idle"	-- 停止中
		if onComplete then
			onComplete()
		end
	end)

	tween:Play()
end

-- =====================================================================
-- スライド移動の開始設定(盤面の座標指定)
-- @param x,y 盤面の座標 左上隅を(0,0)とする
-- @param onComplete スライド完了時のコールバック(省略時(nil)はコールバック無)
-- @return none
function TileView:TweenToGrid(x, y, onComplete)
	self:TweenToPosition(TileView.GridToPosition(x, y), onComplete)
end

-- =====================================================================
-- 表示文字列・文字色・フォントサイズ・背景色を設定
-- @param value タイルの数値(0以上の整数。表示値は2^valueとなる)
-- @return none
-- note:
-- 文字列の表示値は2^valueとなる。矩形の色はタイルの数値に依存する固定値。
function TileView:SetValue(value)
	self.value = value

	local style = Constants.TILE_STYLES[value]

	if style then
		-- フレーム矩形の色設定
		self.label.Text = style.text
		self.frame.BackgroundColor3 = style.background
		self.label.TextColor3 = style.textColor
		-- "self.label.TextSize = xx" は不要。TextScaledでサイズは設定する。
		self.label.TextScaled = true
	else
		-- 文字列の色と文字設定
		self.label.Text = Constants.FormatPowValue(value)
		self.frame.BackgroundColor3 = Color3.fromRGB(32, 128, 128)
		self.label.TextColor3 = Color3.fromRGB(255, 255, 255)
		self.label.TextSize = 20
	end
end

-- =====================================================================
-- オブジェクトの破棄
-- @param none
-- @return none
-- note:
-- self.frameをParentに持つself.labelは、self.frame:Destroy()内で
-- 再帰的に破棄される(Roblox仕様)
-- 注：変数参照は残る。
function TileView:Destroy()
	if self.frame then
		self.frame:Destroy()
		self.frame = nil
	end
end

return TileView
