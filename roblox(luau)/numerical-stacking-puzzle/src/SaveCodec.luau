--[[====================================================================
 Copyright 2026 Miyabi
 SaveCodec.luau
 セーブデータのコーデック用ユーティリティ
 C++版相当クラス: CRec
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui
  +- CRMidSnd
  +- CRAppli
      +- CRCellDat
      +- CRNextDat
      +- CRec === SaveCodec
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
local Constants = require(script.Parent.Constants)

local SaveCodec = {}

-- =====================================================================
-- ゲームデータ -> データテーブル変換
-- @param gameController ゲーム管理クラスのインスタンス(ゲームデータ収集元)
-- @return データテーブル
-- note:
-- Merge配列は内部処理は[0,1,...]で保持しているが、
-- データテーブル上はlua標準に合わせて[1,2,...]とする。
-- (lua標準...配列は1からスタート)
function SaveCodec.Create(gameController)
	local saveMergeTimes = {}
	local saveMergeValues = {}
	for i = 1, 5 do
		saveMergeTimes[i] = gameController.mergeTimes[i-1] or -1
		saveMergeValues[i] = gameController.mergeValues[i-1] or -1
	end

	return {
		saveVersion = Constants.SAVE_VERSION,

		board = gameController.board and gameController.board:Serialize() or nil,
		nextBox = gameController.nextBox and gameController.nextBox:Serialize() or nil,

		fallX = gameController.fallX,
		fallY = gameController.fallY,
		fallVal = gameController.fallVal,
		fallTimer = gameController.fallTimer,

		elapsedTime = gameController.elapsedTime,
		mergeTimes = saveMergeTimes,
		mergeValues = saveMergeValues,

		state = gameController.state,
	}
end

-- =====================================================================
-- データテーブル -> ゲームデータ変換
-- @param gameController ゲーム管理クラスのインスタンス(ゲームデータ展開先)
-- @param data データテーブル
-- @return none
-- note:
-- gameController.fallYとgameController.fallTimerは初期値固定とする
-- これはゲームロード再開時に、中途半端な状態で復元されるのを防止するため。
-- note:
-- Merge配列は内部処理は[0,1,...]で保持しているが、
-- データテーブル上はlua標準に合わせて[1,2,...]とする。
-- (lua標準...配列は1からスタート)
function SaveCodec.Apply(gameController, data)
	if not data then
		return false
	end

	if data.saveVersion ~= Constants.SAVE_VERSION then
		return false
	end

	if data.board and gameController.board then
		gameController.board:Deserialize(data.board)
	end

	if data.nextBox and gameController.nextBox then
		gameController.nextBox:Deserialize(data.nextBox)
	end

	local loadMergeTimes = data.mergeTimes or {}
	local loadMergeValues = data.mergeValues or {}
	
	gameController.fallX = data.fallX or 0
	gameController.fallY = 0	-- data.fallYは使用しない
	gameController.fallVal = data.fallVal or 0
	gameController.fallTimer = Constants.TM_FALL_SLOW_INI	-- data.fallTimerは使用しない

	gameController.elapsedTime = data.elapsedTime or 0
	for i = 0, 4 do
		gameController.mergeTimes[i] = loadMergeTimes[i+1] or -1
		gameController.mergeValues[i] = loadMergeValues[i+1] or -1
	end

	gameController.state = "Serve"	-- data.stateは使用しない
	gameController.seqTimer = Constants.TM_SERVE

	return true
end

return SaveCodec
