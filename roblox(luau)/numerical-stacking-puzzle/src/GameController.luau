--[[====================================================================
 Copyright 2026 Miyabi
 GameController.luau
 ゲーム全体の進行管理クラス
 タイトル～ゲームの遷移、および各フェーズ制御を担当
 C++版相当クラス: CRAppli
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui
  +- CRMidSnd
  +- CRAppli === GameController
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
local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Constants = require(script.Parent.Constants)
local BoardModel = require(script.Parent.BoardModel)
local NextBoxModel = require(script.Parent.NextBoxModel)
local TitleViewRenderer = require(script.Parent.TitleViewRenderer)
local GameViewRenderer = require(script.Parent.GameViewRenderer)
local SaveCodec = require(script.Parent.SaveCodec)

local GameController = {}
GameController.__index = GameController

-- =====================================================================
-- コンストラクタ
-- @param input InputManagerインスタンス
-- @param view ViewRendererインスタンス
-- @param sound SoundManagerインスタンス
-- @return GameControllerインスタンス
function GameController.new(input, view, sound)
	local self = setmetatable({}, GameController)

	-- バッファクリア
	self.mergeTimes = {
		[0] = -1,
		[1] = -1,
		[2] = -1,
		[3] = -1,
		[4] = -1,
	}

	self.mergeValues = {
		[0] = -1,
		[1] = -1,
		[2] = -1,
		[3] = -1,
		[4] = -1,
	}

	self.input = input	-- InputManagerインスタンス
	self.view = view	-- ViewRendererインスタンス
	self.sound = sound	-- SoundManagerインスタンス
	self.board = nil	-- 盤面データ＆ゲームルール管理(この時点では空)
	self.nextBox = nil	-- NEXTキュー管理(この時点では空)

	self.state = "Init"				-- 初期シーケンス=初期状態
	self.seqTimer = 0				-- シーケンスタイマー <- リセット
	self.elapsedTime = -1			-- 経過時間 <- 無効値
	self.titleBackRequested = false	-- タイトルバック指令 <- 無し
	self.startRequested = false		-- ゲームスタート指令 <- 無し
	self.loadStartRequested = false	-- ゲームロード＆スタート指令 <- 無し
	self.saveRequested = false		-- ゲームセーブ指令 <- 無し

	-- 落下箱の初期状態を設定
	self.fallX = 0
	self.fallY = 0
	self.fallVal = 0
	self.fallTimer = Constants.TM_FALL_SLOW_INI

	return self
end

-- =====================================================================
-- パズル系のモデルの生成
-- @param none
-- @return none
function GameController:CreatePuzzle()
	self.board = BoardModel.new()
	self.nextBox = NextBoxModel.new()
end

-- =====================================================================
-- パズル系のデータの初期設定
-- @param none
-- @return none
function GameController:InitPuzzleData()
	-- スコア、経過時間のリセット
	self.board:SetScore(0)
	self.elapsedTime = 0

	-- 落下箱の初期状態を設定
	self.fallX = 0
	self.fallY = 0
	self.fallVal = self.nextBox:ExecPush()
	self.fallTimer = Constants.TM_FALL_SLOW_INI
end

-- =====================================================================
-- 経過時間の更新処理
-- @param none
-- @return none
-- note:
-- 最初の+1カウントはブロックの落下開始からスタート。
-- 以降は毎フレームインクリメントする。
function GameController:UpdateElapsedTime()
	if self.elapsedTime == 0 then
		if self.state == "Serve" or self.state == "Title" or self.state == "Init" then
			return
		elseif self.state == "FallSlow" then
			if self.fallY ~= 0 then
				self.elapsedTime = 1
			end
		else
			self.elapsedTime = 1
		end
	else
		self.elapsedTime += 1
	end

	self.gameView:SetHudElapsed(self.elapsedTime)
end

-- =====================================================================
-- マージ時間の更新処理
-- @param maxMergeVal マージの発生した箱の値
-- @return none
-- note:
-- マージした値の大きい順に5位までのマージ達成時間を記録する。
-- マージ達成時間は最も早い時間のみ記憶する。既に同じマージ値の記録がある時は
-- 記録を更新しない。
function GameController:UpdateMergeTime(maxMergeVal)
	if maxMergeVal >= 0 then
		for i = 0, 4 do
			if self.mergeValues[i] == maxMergeVal then
				self.gameView:SetHudMergeRecords(self.mergeTimes, self.mergeValues)
				return
			end
		end

		local val = maxMergeVal
		local tm = self.elapsedTime

		for i = 0, 4 do
			if val >= self.mergeValues[i] then
				val, self.mergeValues[i] = self.mergeValues[i], val
				tm, self.mergeTimes[i] = self.mergeTimes[i], tm
			end
		end
	end

	self.gameView:SetHudMergeRecords(self.mergeTimes, self.mergeValues)
end

-- =====================================================================
-- ファイル保存オペレーション
-- @param none
-- @return none
-- note:
-- セーブ失敗時の救済やアラートバックは無し。
function GameController:SaveRecent()
	local remotes = ReplicatedStorage:FindFirstChild("NSP_Remotes")
	local saveRemote = remotes and remotes:FindFirstChild("SaveGame")

	if not saveRemote then
		return
	end

	local data = SaveCodec.Create(self)

	pcall(function()
		saveRemote:InvokeServer(data)
	end)
end

-- =====================================================================
-- ファイル読出オペレーション
-- @param none
-- @return 読み出しの成否
-- note:
-- ロードに失敗した場合はfalseを返し、上位レイヤーで処理。
function GameController:TryLoadRecent()
	local remotes = ReplicatedStorage:FindFirstChild("NSP_Remotes")
	local loadRemote = remotes and remotes:FindFirstChild("LoadGame")

	if not loadRemote then
		return false
	end

	local ok, data = pcall(function()
		return loadRemote:InvokeServer()
	end)

	if ok and data then
		-- SaveCodec.Apply内でデータ読出
		local applied = SaveCodec.Apply(self, data)
		if applied then
			return true
		end
	end

	return false
end

-- =====================================================================
-- シーケンス処理:初期状態
-- @param none
-- @return none
-- note:
-- タイトル状態へ遷移
function GameController:SeqInit()
	self:StartTitle()	-- タイトル開始処理
end

-- =====================================================================
-- タイトル開始処理 @タイトル状態 or タイトルバック状態
-- @param none
-- @return none
-- note:
-- ゲーム用モデルクリア、タイトル状態への遷移、ボタンUIの表示変更、
-- ゲーム画面のクリア、タイトル画面の表示開始を実行。
function GameController:StartTitle()
	self.board = nil
	self.nextBox = nil
	self.state = "Title"

	-- ボタンUIの表示変更(タイトル中はSAVEボタンは無効)
	self.view:ShowButton({
		gameController = self
	})
	self.view:SetNewOrBackButtonNew()
	self.view:SetButtonEnabled("LOAD", true)
	self.view:SetButtonEnabled("SAVE", false)
	-- ゲーム画面オブジェクトがあれば消去
	if self.gameView then
		self.gameView:Clear()
		self.gameView = nil
	end
	-- タイトル画面生成
	self.titleView = TitleViewRenderer.new()
	self.titleView:ShowTitle(self.view.root, self.sound)
end

-- =====================================================================
-- シーケンス処理:タイトル状態
-- @param none
-- @return none
-- note:
-- ゲームスタート/ゲームロード＆スタート/効果音切替のユーザ操作待ち
function GameController:SeqTitle()
	local keyDown = self.input:GetKeyDown()
	local start = false
	local loadStart = false

	-- ゲームスタート操作
	if keyDown.qwerty[0] or keyDown.one or self.startRequested then
		self.startRequested = false
		start = true
	end

	-- ゲームロード＆スタート操作
	if keyDown.qwerty[1] or keyDown.two or self.loadStartRequested then
		self.loadStartRequested = false
		start = true
		loadStart = true
	end

	-- 効果音切替(オフ)操作
	if keyDown.qwerty[2] then
		self.sound:SetEnabled(false)
		if self.titleView.titleSoundTextUpdater then
			self.titleView.titleSoundTextUpdater()
		end
		return
	end

	-- 効果音切替(オン)操作
	if keyDown.qwerty[3] then
		self.sound:SetEnabled(true)
		self.sound:PlayMerge3()
		if self.titleView.titleSoundTextUpdater then
			self.titleView.titleSoundTextUpdater()
		end
		return
	end

	-- ゲームスタートorゲームロード＆スタート -> ゲーム開始
	if start then
		self:CreatePuzzle()
		self:InitPuzzleData()
		if loadStart then
			-- ゲームロード
			if self:TryLoadRecent() then
			else
				-- ゲームロード失敗時は中断
				-- 前段のCreatePuzzle()でオブジェクトに接続しているので解放する
				self:StartTitle()	-- タイトル開始処理
				return
			end
		end
		-- ゲーム開始
		self:StartGame()
	end
end

-- =====================================================================
-- ゲーム開始処理 @タイトル状態
-- @param none
-- @return none
-- note:
-- モード遷移指令のクリア、サーブ状態への遷移、ボタンUIの表示変更、
-- タイトル画面のクリア、ゲーム画面の表示開始を実行。
function GameController:StartGame()
	self.titleBackRequested = false	-- タイトルバック指令 <- 無し
	self.startRequested = false		-- ゲームスタート指令 <- 無し
	self.loadStartRequested = false	-- ゲームロード＆スタート指令 <- 無し
	self.saveRequested = false		-- ゲームセーブ指令 <- 無し
	self.state = "Serve"
	self.seqTimer = Constants.TM_SERVE

	-- ボタンUIの表示変更(ゲーム中はLOADボタンは無効)
	self.view:SetNewOrBackButtonBack()
	self.view:SetButtonEnabled("LOAD", false)
	self.view:SetButtonEnabled("SAVE", true)
	-- タイトル画面オブジェクトがあれば消去
	if self.titleView then
		self.titleView:Clear()
		self.titleView = nil
	end
	-- ゲーム画面生成
	self.gameView = GameViewRenderer.new(self.view.playerGui, self.input)
	self.gameView:CreateGameUI(self.view.root, self.board, self.nextBox, self)
	self.gameView:RefreshBoard(self.board)
	self.gameView:RefreshNextBoxes(self.nextBox)
	self.gameView:CreateFallingTile(self.fallVal)
	self.gameView:ServeFallingTile(self.fallX, self.fallY)
	self.gameView:SetHudScore(self.board:GetScore())
	self.gameView:SetHudElapsed(self.elapsedTime)
	self.gameView:SetHudMergeRecords(self.mergeTimes, self.mergeValues)
end

-- =====================================================================
-- シーケンス＝サーブ状態
-- @param none
-- @return none
-- note:
-- セーブ、タイトルバック操作受付
-- 時間経過後に落下(遅)状態へ遷移
function GameController:SeqServe()
	self:UpdateElapsedTime()

	self:HandleSaveRequest()

	if self:HandleBackRequest() then
		return
	end

	if self.seqTimer == 0 then
		self.seqTimer = Constants.TM_FALL_SLOW_INI
		self.fallTimer = Constants.TM_FALL_SLOW_INI
		self.state = "FallSlow"
	end
end

-- =====================================================================
-- セーブ操作共通処理
-- @param none
-- @return none
-- note:
-- ボタンクリック or [3]キーでセーブ動作。
-- サーブ状態 or 落下(遅)状態 or 落下(速)状態 以外は、処理保留して
-- 状態の遷移後に保存処理を実行する。
function GameController:HandleSaveRequest()
	local keyDown = self.input:GetKeyDown()

	if keyDown.three then
		self.saveRequested = true
	end

	if self.state == "Serve" or
	   self.state == "FallSlow" or
	   self.state == "FallFast" then
		if self.saveRequested then
			self.saveRequested = false
			self:SaveRecent()
		end
	end
end

-- =====================================================================
-- タイトルバック操作共通処理
-- @param none
-- @return none
-- note:
-- ボタンクリック or [1]キーでタイトルバック動作。
-- サーブ状態 or 落下(遅)状態 or 落下(速)状態 以外は、処理保留して
-- 状態の遷移後に保存処理を実行する。
function GameController:HandleBackRequest()
	local keyDown = self.input:GetKeyDown()

	if keyDown.one then
		self.titleBackRequested = true
	end

	if self.state == "Serve" or
	   self.state == "FallSlow" or
	   self.state == "FallFast" then
		if self.titleBackRequested then
			self.titleBackRequested = false
			self.state = "Back"
			return true
		end
	end

	return false
end

-- =====================================================================
-- シーケンス＝落下(遅)状態
-- @param none
-- @return none
-- note:
-- セーブ、タイトルバック操作受付
-- スタック状態になるまでゆっくり落下
-- 操作すると高速落下に遷移
function GameController:SeqFallSlow()
	self:UpdateElapsedTime()

	self:HandleSaveRequest()

	if self:HandleBackRequest() then
		return
	end

	-- 落下中の箱の表示位置を算出してViewRendererに渡す。
	self:UpdateFallingPositionSlow()

	-- 1マスの境界に到達した箱が落下着地しているか判定する。
	-- 落下着地していたら積上状態に遷移。
	-- 降下前(Y=0)で直下に箱がある場合 -> GameOverにはせず、何もしない(Endless仕様)
	if self.seqTimer == 0 then
		self:PerformStackCheck()
	end

	-- 前段で積上状態になった場合を除外してキー判定
	if self.state ~= "Stack" then
		self:TryColumnInputToFast()
	end
end

-- =====================================================================
-- 箱の降下処理 @落下(遅)状態
-- @param none
-- @return none
-- note:
-- 落下中の箱の表示位置を算出してViewRendererに渡す。
function GameController:UpdateFallingPositionSlow()
	local posX = self.fallX * Constants.CELL_SIZE
	local posY

	if self.fallY == 0 then
		posY = 0
	else
		posY = self.fallY * Constants.CELL_SIZE
		posY -= self.seqTimer * Constants.CELL_SIZE / self.fallTimer
	end

	self.gameView:SetFallingTilePositionPixel(posX, posY)
end

-- =====================================================================
-- スタック判定 @落下(遅)状態
-- @param none
-- @return none
-- note:
-- 1マスの境界に到達した箱が落下着地しているか判定する。
-- 落下着地していたら積上状態に遷移。
-- 降下前(Y=0)で直下に箱がある場合 -> GameOverにはせず、何もしない(Endless仕様)
function GameController:PerformStackCheck()
	if self.board:CheckEnStack(self.fallX, self.fallY, self.fallVal) then
		self.sound:PlayStack()
		self.gameView:ApplyStack(self.board)
		self.seqTimer = Constants.TM_STACK
		self.state = "Stack"
	else
		if self.fallY == 0 and self.board:GetVal(self.fallX, self.fallY + 1) >= 0 then
			-- C++版と同じ：明示的なGameOver phaseなし。何もしない。
		else
			self.fallY += 1
			self.seqTimer = self.fallTimer
		end
	end
end

-- =====================================================================
-- キー操作処理 @落下(遅)状態
-- @param none
-- @return none
-- 横方向のブロック移動のキー判定
-- キーボード入力の他、タップ操作、クリック操作も対象。
-- タップ操作、クリック操作はInputContorollerでキーボード入力にエミュレーションしている。
function GameController:TryColumnInputToFast()
	local keyDown = self.input:GetKeyDown()

	for x = 0, Constants.CELL_X_NUM - 1 do
		if keyDown.qwerty[x] then
			if self.board:GetVal(x, self.fallY) < 0 then
				self.fallX = x
				self.seqTimer = math.floor((self.seqTimer * Constants.TM_FALL_FAST + self.fallTimer / 2) / self.fallTimer)
				self.state = "FallFast"
				return
			end
		end
	end
end

-- =====================================================================
-- シーケンス＝落下(速)状態
-- @param none
-- @return none
-- note:
-- セーブ、タイトルバック操作受付
-- スタック状態になるまで高速落下
function GameController:SeqFallFast()
	self:UpdateElapsedTime()

	self:HandleSaveRequest()

	if self:HandleBackRequest() then
		return
	end

	-- 落下中の箱の表示位置を算出してViewRendererに渡す。
	self:UpdateFallingPositionFast()

	-- 1マスの境界に到達した箱が落下着地しているか判定する。
	-- 落下着地していたら積上状態に遷移。
	-- 降下前(Y=0)で直下に箱がある場合 -> GameOverにはせず、何もしない(Endless仕様)
	if self.seqTimer == 0 then
		if self.board:CheckEnStack(self.fallX, self.fallY, self.fallVal) then
			self.sound:PlayStack()
			self.gameView:ApplyStack(self.board)
			self.seqTimer = Constants.TM_STACK
			self.state = "Stack"
		else
			if self.fallY == 0 and self.board:GetVal(self.fallX, self.fallY + 1) >= 0 then
				-- C++版と同じ：明示的なGameOver phaseなし。何もしない。
			else
				self.fallY += 1
				self.seqTimer = Constants.TM_FALL_FAST
			end
		end
	end

	-- 前段で積上状態になった場合を除外してキー判定
	if self.state ~= "Stack" then
		local keyDown = self.input:GetKeyDown()
		for x = 0, Constants.CELL_X_NUM - 1 do
			if keyDown.qwerty[x] then
				if self.board:GetVal(x, self.fallY) < 0 then
					self.fallX = x
					return
				end
			end
		end
	end
end

-- =====================================================================
-- 箱の降下処理 @落下(速)状態
-- @param none
-- @return none
-- note:
-- 落下中の箱の表示位置を算出してViewRendererに渡す。
function GameController:UpdateFallingPositionFast()
	local posX = self.fallX * Constants.CELL_SIZE
	local posY

	if self.fallY == 0 then
		posY = 0
	else
		posY = self.fallY * Constants.CELL_SIZE
		posY -= self.seqTimer * Constants.CELL_SIZE / Constants.TM_FALL_FAST
	end

	self.gameView:SetFallingTilePositionPixel(posX, posY)
end

-- =====================================================================
-- シーケンス＝積上状態
-- @param none
-- @return none
-- note:
-- セーブ、タイトルバック操作は記憶するだけ。
-- 時間経過後に積上状態でデータ更新、スコア更新、タイル表示の更新。
-- 更新処理後に格納値のマージ実行(or次の落下開始)
function GameController:SeqStack()
	self:UpdateElapsedTime()

	self:HandleSaveRequest()

	if self:HandleBackRequest() then
		return
	end

	if self.seqTimer == 0 then
		self.board:ExecStack()

		if self.board:CalcScore() then
			self.gameView:SetHudScore(self.board:GetScore())
		end

		self.gameView:DestroyFallingTile()
		self.gameView:RefreshBoard(self.board)

		-- 格納値のマージ実行(or次の落下開始)
		self:RunApplyMergeOrSpawnNext()
	end
end

-- =====================================================================
-- 格納値のマージ実行(or次の落下開始) @積上状態 or 結合動作状態 or 落下着地動作状態
-- @param none
-- @return none
-- note:
-- 箱の降下移動が完了した時点で呼出される想定。
-- (降下移動が完了＝落下する箱の落下着地、マージの完了、マージ後の再落下着地時)
-- マージ必要な状態なら結合動作状態に遷移する。
-- マージ不要な状態ならサーブ状態に遷移する。
function GameController:RunApplyMergeOrSpawnNext()
	local found, maxMergeVal, maxMergeDifVal = self.board:CheckMerge(self.fallX, self.fallY)

	if found then
		self:UpdateMergeTime(maxMergeVal)
		self.sound:PlayMergeByDiff(maxMergeDifVal)
		self.gameView:ApplyMerge(self.board)

		self.seqTimer = Constants.TM_MERGE
		self.state = "Merge"
	else
		self:SpawnNextFalling()
	end
end

-- =====================================================================
-- 落下開始位置の初期値を設定
-- @param none
-- @return none
function GameController:SpawnNextFalling()
	self.fallY = 0
	self.fallVal = self.nextBox:ExecPush()
	self.gameView:RefreshNextBoxes(self.nextBox)

	self.gameView:CreateFallingTile(self.fallVal)
	self.gameView:ServeFallingTile(self.fallX, self.fallY)

	self.state = "Serve"
	self.seqTimer = Constants.TM_SERVE
end

-- =====================================================================
-- シーケンス＝結合動作状態
-- @param none
-- @return none
-- note:
-- セーブ、タイトルバック操作は記憶するだけ。
-- 時間経過後に積上状態でデータ更新、スコア更新、タイル表示の更新。
-- 更新処理後に格納値のマージ実行(or次の落下開始)
function GameController:SeqMerge()
	self:UpdateElapsedTime()

	self:HandleSaveRequest()

	if self:HandleBackRequest() then
		return
	end

	if self.seqTimer == 0 then
		self.board:ExecMerge()

		if self.board:CalcScore() then
			self.gameView:SetHudScore(self.board:GetScore())
		end

		self.gameView:RefreshBoard(self.board)

		if self.board:CheckEnLand() then
			self.gameView:ApplyLand(self.board)
			self.seqTimer = Constants.TM_LAND
			self.state = "Land"
		else
			-- 格納値のマージ実行(or次の落下開始)
			self:RunApplyMergeOrSpawnNext()
		end
	end
end

-- =====================================================================
-- シーケンス＝落下着地動作状態
-- @param none
-- @return none
-- note:
-- セーブ、タイトルバック操作は記憶するだけ。
-- 時間経過後に積上状態でデータ更新、スコア更新、タイル表示の更新。
-- 更新処理後に格納値のマージ実行(or次の落下開始)
function GameController:SeqLand()
	self:UpdateElapsedTime()

	self:HandleSaveRequest()

	if self:HandleBackRequest() then
		return
	end

	if self.seqTimer == 0 then
		self.board:ExecLand()

		if self.board:CalcScore() then
			self.gameView:SetHudScore(self.board:GetScore())
		end

		self.gameView:RefreshBoard(self.board)
		self:RunApplyMergeOrSpawnNext()
	end
end

-- =====================================================================
-- シーケンス＝タイトルバック状態
-- @param none
-- @return none
-- note:
-- タイトル画面を生成してタイトルへ遷移する。
function GameController:SeqBack()
	self:StartTitle()	-- タイトル開始処理
end

-- =====================================================================
-- ゲーム更新（メインループ）
-- @param none
-- @return none
function GameController:UpdateOneFrame()
	self.seqTimer = Constants.Dec(self.seqTimer)
	
	if self.nextBox then
		self.nextBox:Update()
	end

	if self.state == "Init" then
		self:SeqInit()
	elseif self.state == "Title" then
		self:SeqTitle()
	elseif self.state == "Serve" then
		self:SeqServe()
	elseif self.state == "FallSlow" then
		self:SeqFallSlow()
	elseif self.state == "FallFast" then
		self:SeqFallFast()
	elseif self.state == "Stack" then
		self:SeqStack()
	elseif self.state == "Merge" then
		self:SeqMerge()
	elseif self.state == "Land" then
		self:SeqLand()
	elseif self.state == "Back" then
		self:SeqBack()
	end

	-- ゲーム画面オブジェクトがあれば更新処理
	if self.gameView then
		self.gameView:Update()
	end
end

-- =====================================================================
-- [NEW/BACK]ボタンのクリックorタップイベント
-- @param none
-- @return none
-- note:
-- ViewRenderer:CreateButtonsでボタンGUIの登録時に参照している。
-- [NEW/BACK]ボタンのクリックorタップをトリガに起動する。
function GameController:OnNewOrBackButton()
	if self.state == "Title" then
		self.startRequested = true
	else
		self.titleBackRequested = true
	end
end

-- =====================================================================
-- [LOAD]ボタンのクリックorタップイベント
-- @param none
-- @return none
-- note:
-- ViewRenderer:CreateButtonsでボタンGUIの登録時に参照している。
-- [LOAD]ボタンのクリックorタップをトリガに起動する。
function GameController:OnLoadButton()
	if self.state == "Title" then
		self.loadStartRequested = true
	end
end

-- =====================================================================
-- [SAVE]ボタンのクリックorタップイベント
-- @param none
-- @return none
-- note:
-- ViewRenderer:CreateButtonsでボタンGUIの登録時に参照している。
-- [SAVE]ボタンのクリックorタップをトリガに起動する。
function GameController:OnSaveButton()
	if self.state ~= "Title" then
		self.saveRequested = true
	end
end

return GameController
