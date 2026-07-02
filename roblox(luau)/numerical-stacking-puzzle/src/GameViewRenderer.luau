--[[====================================================================
 Copyright 2026 Miyabi
 GameViewRenderer.luau
 ゲーム画面の描画オブジェクト管理クラス
 Roblox版では描画メソッドがコンパクトにまとまるため、
 C++のように分割せず、ある程度集約した構成にする。
 C++版相当クラス: CRAppBoxSet, CRAppNextBoxSet,
                  CRAppKeyIndSet, CRAppKeyInd
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
      +- CRAppBoxSet === GameViewRenderer
      |   +- CRAppBox
      +- CRAppNextBoxSet === GameViewRenderer
      |   +- CRAppBox
      +- CRAppKeyIndSet === GameViewRenderer
      |   +- CRAppKeyInd === GameViewRenderer
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
local TileView = require(script.Parent.TileView)
local HudView = require(script.Parent.HudView)

local GameViewRenderer = {}
GameViewRenderer.__index = GameViewRenderer

-- =====================================================================
-- コンストラクタ
-- @param playerGui 親GUIインスタンス
-- @param inputController 入力管理インスタンス(GUI操作に紐づけするため必要)
-- @return GameViewRendererインスタンス
function GameViewRenderer.new(playerGui, inputController)
	local self = setmetatable({}, GameViewRenderer)

	self.playerGui = playerGui
	self.input = inputController

	self.gameFrame = nil	-- ゲーム画面用GUI(この時点ではnil)

	self.tiles = {}			-- 盤面タイル表示(この時点では空)
	self.fallingTile = nil	-- 落下中タイル表示(この時点ではnil)
	self.nextTiles = {}		-- NEXTキュータイル表示(この時点では空)
	self.keyIndicators = {}	-- キー入力状態表示(この時点では空)
	self.hud = nil			-- ヘッドアップ表示(この時点ではnil)

	return self
end

-- =====================================================================
-- 描画オブジェクトのクリア
-- @param none
-- @return none
function GameViewRenderer:Clear()
	-- 子・孫の描画オブジェクトがあれば破棄する
	if self.gameFrame then
		for _, child in ipairs(self.gameFrame:GetChildren()) do
			child:Destroy()
		end
	end

	self.gameFrame = nil
	self.tiles = {}
	self.fallingTile = nil
	self.nextTiles = {}
	self.keyIndicators = {}
	self.hud = nil
end

-- =====================================================================
-- ゲーム画面の生成
-- @param parent 親フレームインスタンス(表示基準点となる)
-- @param board 盤面データ管理クラスのインスタンス
-- @param nextBox NEXTキュー管理クラスのインスタンス
-- @param gameController ゲーム全体の進行管理クラスのインスタンス
-- @return none
-- note:
-- 描画オブジェクトの他、画面上の操作用GUIの配置も行うため、
-- 引数でgameControllerを受け取っている。
-- 各フレーム(マス目)にはInputControllerのInjectColumnDown関数を紐付する。
-- InjectColumnDown関数はクリック、タップ時に呼び出しされるので、
-- ゲームはクリック、タップ操作に対応している。
function GameViewRenderer:CreateGameUI(parent, board, nextBox, gameController)
	-- 描画オブジェクトのクリア
	self:Clear()

	-- ゲーム画面用フレームの生成
	local frame = Instance.new("Frame")
	frame.Name = "gameFrame(Frame)"
	frame.Size = UDim2.fromScale(1, 1)
	frame.BackgroundTransparency = 1
	frame.Parent = parent
	self.gameFrame = frame

	self.tiles = {}
	for x = 0, Constants.CELL_X_NUM - 1 do
		self.tiles[x] = {}
	end

	-- 盤面データに対応したフレーム(マス目)の生成
	for x = 0, Constants.CELL_X_NUM - 1 do
		for y = 0, Constants.CELL_Y_NUM - 1 do
			local bg = Instance.new("Frame")
			bg.Name = "BG:"..x.."/"..y 
			bg.Size = UDim2.fromScale(Constants.CELL_SIZE / Constants.BASE_WIDTH, Constants.CELL_SIZE / Constants.BASE_HEIGHT)
			bg.Position = TileView.GridToPosition(x, y)
			bg.BackgroundColor3 = Color3.fromRGB(230, 230, 230)
			bg.BorderColor3 = Color3.fromRGB(200, 200, 200)
			bg.ZIndex = 1
			bg.Parent = frame

			-- 各フレーム(マス目)にInjectColumnDown関数を紐付する。
			bg.InputBegan:Connect(function(input)
				if input.UserInputType == Enum.UserInputType.MouseButton1 or input.UserInputType == Enum.UserInputType.Touch then
					if self.input then
						self.input:InjectColumnDown(x)
					end
				end
			end)
		end
	end

	-- 盤面にタイル配置(ブロックが存在する空ではないマスのみ)
	for x = 0, Constants.CELL_X_NUM - 1 do
		for y = 0, Constants.CELL_Y_NUM - 1 do
			local v = board:GetVal(x, y)
			if v >= 0 then
				self.tiles[x][y] = TileView.new(frame, v, TileView.GridToPosition(x, y))
			end
		end
	end

	-- NEXTキューにタイル配置
	for i = 0, Constants.NEXT_BOX_NUM - 1 do
		local tile = TileView.new(frame, nextBox:GetVal(i), TileView.NextBoxPosition(i))
		self.nextTiles[i] = tile
	end

	-- キー入力状態表示の生成
	self:CreateKeyIndicators(frame)

	-- ヘッドアップ表示の生成
	self.hud = HudView.new(frame)
	self.hud:SetScore(board:GetScore())
	self.hud:SetElapsedTime(0)
end

-- =====================================================================
-- キー入力状態表示の生成
-- @param parent 親フレームインスタンス(表示基準点となる)
-- @return none
-- note:
function GameViewRenderer:CreateKeyIndicators(parent)
	for row = 0, 1 do
		for x = 0, Constants.CELL_X_NUM - 1 do
			local labels = Constants.KEY_LABELS[x]

			-- フレーム矩形のサイズ・位置・境界線の設定
			local frame = Instance.new("Frame")
			frame.Name = "KeyLabel:"..x
			frame.Size = UDim2.fromScale(80 / Constants.BASE_WIDTH, 80 / Constants.BASE_HEIGHT)
			frame.Position = UDim2.fromScale((x * 100 + 10 + 400) / Constants.BASE_WIDTH, (row * 800 + 10) / Constants.BASE_HEIGHT)
			frame.BackgroundColor3 = Color3.fromRGB(160,192,128)
			frame.BorderColor3 = Color3.fromRGB(0,128,0)
			frame.ZIndex = 20
			frame.Parent = parent

			-- 文字列のサイズ・位置・フォント情報の設定(フレーム上段文字)
			local a = Instance.new("TextLabel")
			a.Name = "KeyLabel1:"..x
			a.BackgroundTransparency = 1
			a.Position = UDim2.fromScale((x * 100 + 10 + 400) / Constants.BASE_WIDTH, (row * 800 + 10) / Constants.BASE_HEIGHT)
			a.Size = UDim2.fromScale(40 / Constants.BASE_WIDTH, 40 / Constants.BASE_HEIGHT)
			a.Text = labels[1]
			a.Font = Enum.Font.GothamBold
			a.TextScaled = true
			-- "a.TextSize = xx" は不要。TextScaledでサイズは設定する。
			a.TextColor3 = Color3.fromRGB(0,0,0)
			a.ZIndex = 21
			a.Parent = parent

			-- 文字列のサイズ・位置・フォント情報の設定(フレーム下段文字)
			local b = Instance.new("TextLabel")
			b.Name = "KeyLabel2:"..x
			b.BackgroundTransparency = 1
			b.Position = UDim2.fromScale((x * 100 + 50 + 400) / Constants.BASE_WIDTH, (row * 800 + 50) / Constants.BASE_HEIGHT)
			b.Size = UDim2.fromScale(40 / Constants.BASE_WIDTH, 40 / Constants.BASE_HEIGHT)
			b.Text = labels[2]
			b.Font = Enum.Font.GothamBold
			b.TextScaled = true
			-- "b.TextSize = xx" は不要。TextScaledでサイズは設定する。
			b.TextColor3 = Color3.fromRGB(0,0,0)
			b.ZIndex = 21
			b.Parent = parent

			self.keyIndicators[#self.keyIndicators + 1] = {
				index = x,
				frame = frame,
			}
		end
	end
end

-- =====================================================================
-- キー入力状態表示の更新
-- @param input InputManagerインスタンス
-- @return none
-- note:
-- キー入力がオン状態のインジケータの表示色を変える
function GameViewRenderer:UpdateKeyIndicators(input)
	if not input then
		return
	end

	local keyOn = input:GetKeyOn()

	for _, ind in ipairs(self.keyIndicators) do
		if keyOn.qwerty[ind.index] then
			ind.frame.BackgroundColor3 = Color3.fromRGB(240,192,128)
		else
			ind.frame.BackgroundColor3 = Color3.fromRGB(160,192,128)
		end
	end
end

-- =====================================================================
-- 落下ブロックの生成
-- @param input InputManagerインスタンス
-- @return none
function GameViewRenderer:CreateFallingTile(value)
	if self.fallingTile then
		self.fallingTile:Destroy()
	end

	self.fallingTile = TileView.new(
		self.gameFrame,
		value,
		UDim2.fromScale((1200+400) / Constants.BASE_WIDTH, Constants.BOARD_OFFSET_Y / Constants.BASE_HEIGHT)
	)
end

-- =====================================================================
-- 落下ブロックの位置を設定
-- @param px,py 仮想空間座標上のピクセル座標 左上隅を(0,0)とする
-- @return none
-- note:
-- 仮想空間座標の幅はBOARD_OFFSET_X, BOARD_OFFSET_Yで定義されている
function GameViewRenderer:SetFallingTilePositionPixel(px, py)
	if self.fallingTile then
		self.fallingTile:SetPixelPosition(
			Constants.BOARD_OFFSET_X + px,
			Constants.BOARD_OFFSET_Y + py
		)
	end
end

-- =====================================================================
-- 落下ブロックのサーブ状態描画開始
-- @param x,y 盤面の座標 左上隅を(0,0)とする
-- @return none
-- note:
-- 盤面上の座標を表示位置に換算するため、
-- self.gameFrameを親フレームとしたTileViewに対して使用されることを
-- 前提としている。
function GameViewRenderer:ServeFallingTile(x, y)
	if self.fallingTile then
		self.fallingTile:TweenToGrid(x, y)
	end
end

-- =====================================================================
-- 落下ブロックの破棄
-- @param none
-- @return none
function GameViewRenderer:DestroyFallingTile()
	if self.fallingTile then
		self.fallingTile:Destroy()
		self.fallingTile = nil
	end
end

-- =====================================================================
-- ブロックの積上状態描画開始
-- @param board 盤面データ管理クラスのインスタンス
-- @return none
function GameViewRenderer:ApplyStack(board)
	for x = 0, Constants.CELL_X_NUM - 1 do
		for y = 0, Constants.CELL_Y_NUM - 1 do
			local val = board:GetVal(x, y)
			if val < 0 then
				val = board:GetStackVal(x, y)
			end

			if val >= 0 and not self.tiles[x][y] then
				self.tiles[x][y] = TileView.new(self.gameFrame, val, TileView.GridToPosition(x, y))
			end
		end
	end
end

-- =====================================================================
-- ブロックのマージ状態描画開始
-- @param board 盤面データ管理クラスのインスタンス
-- @return none
function GameViewRenderer:ApplyMerge(board)
	for x = 0, Constants.CELL_X_NUM - 1 do
		for y = 0, Constants.CELL_Y_NUM - 1 do
			local tile = self.tiles[x][y]
			if tile then
				local mx = board:GetMergeX(x, y)
				local my = board:GetMergeY(x, y)

				if mx >= 0 and my >= 0 then
					if mx == x and my == y then
						local val = board:GetMergeVal(x, y)
						if val > tile.value then
							tile:SetValue(val)
						end
					else
						self.tiles[x][y] = nil
						tile:TweenToGrid(mx, my, function()
							tile:Destroy()
						end)
					end
				end
			end
		end
	end
end

-- =====================================================================
-- ブロックの落下着地状態描画開始
-- @param board 盤面データ管理クラスのインスタンス
-- @return none
function GameViewRenderer:ApplyLand(board)
	for x = 0, Constants.CELL_X_NUM - 1 do
		for y = Constants.CELL_Y_NUM - 1, 0, -1 do
			local tile = self.tiles[x][y]
			if tile then
				local ly = board:GetLandY(x, y)
				if ly >= 0 then
					self.tiles[x][y] = nil
					self.tiles[x][ly] = tile
					tile:TweenToGrid(x, ly)
				end
			end
		end
	end
end

-- =====================================================================
-- 盤面のブロック描画のリフレッシュ
-- @param board 盤面データ管理クラスのインスタンス
-- @return none
function GameViewRenderer:RefreshBoard(board)
	for x = 0, Constants.CELL_X_NUM - 1 do
		for y = 0, Constants.CELL_Y_NUM - 1 do
			local v = board:GetVal(x, y)
			local tile = self.tiles[x][y]

			if v >= 0 then
				if tile then
					tile:SetValue(v)
					tile:SetGridPosition(x, y)
				else
					self.tiles[x][y] = TileView.new(self.gameFrame, v, TileView.GridToPosition(x, y))
				end
			else
				if tile then
					tile:Destroy()
					self.tiles[x][y] = nil
				end
			end
		end
	end
end

-- =====================================================================
-- NEXTキューのブロック描画のリフレッシュ
-- @param nextBox NEXTキューデータ管理クラスのインスタンス
-- @return none
function GameViewRenderer:RefreshNextBoxes(nextBox)
	for i = 0, Constants.NEXT_BOX_NUM - 1 do
		if self.nextTiles[i] then
			self.nextTiles[i]:Destroy()
			self.nextTiles[i] = nil
		end

		local tile = TileView.new(
			self.gameFrame,
			nextBox:GetVal(i),
			UDim2.fromScale((Constants.NEXT_BOX_OFFSET_X + i * Constants.CELL_SIZE + 100) / Constants.BASE_WIDTH, (Constants.NEXT_BOX_OFFSET_Y) / Constants.BASE_HEIGHT)
		)

		tile:TweenToPosition(TileView.NextBoxPosition(i))
		self.nextTiles[i] = tile
	end
end

-- =====================================================================
-- 表示するスコアの設定
-- @param score スコア
-- @return none
function GameViewRenderer:SetHudScore(score)
	if self.hud then
		self.hud:SetScore(score)
	end
end

-- =====================================================================
-- 表示する経過時間の設定
-- @param score 経過時間
-- @return none
function GameViewRenderer:SetHudElapsed(frames)
	if self.hud then
		self.hud:SetElapsedTime(frames)
	end
end

-- =====================================================================
-- 表示するマージ記録の設定
-- @param times,values マージした経過時間、マージで生成した値
-- @return none
function GameViewRenderer:SetHudMergeRecords(times, values)
	if self.hud then
		self.hud:SetMergeRecords(times, values)
	end
end

-- =====================================================================
-- ヘッドアップ表示とキー入力状態表示の更新
-- @param none
-- @return none
function GameViewRenderer:Update()
	if self.hud then
		self.hud:Update()
	end

	if self.input then
		self:UpdateKeyIndicators(self.input)
	end
end

return GameViewRenderer
