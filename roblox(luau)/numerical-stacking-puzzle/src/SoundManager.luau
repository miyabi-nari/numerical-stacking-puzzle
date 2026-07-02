--[[====================================================================
 Copyright 2026 Miyabi
 SoundManager.luau
 サウンド再生管理クラス
 効果音（SE）の再生とON/OFF制御を担当する
 C++版相当クラス: CRMidSnd
 CRMainFrm
  +- CTimerLib
  +- CRMidKey
  +- CRMidGui
  +- CRMidSnd === SoundManager
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
local SoundManager = {}
SoundManager.__index = SoundManager

-- =====================================================================
-- ローカル定数の定義
local DEFAULT_VOLUME = 0.6

-- =====================================================================
-- コンストラクタ
-- @param parent 親GUIインスタンス(Soundインスタンス生成時に必要)
-- @return SoundManagerインスタンス
-- note:
-- parent...GUI配下でも再生可能だが、確実性を求める場合は Workspace に配置すること。
function SoundManager.new(parent)
	local self = setmetatable({}, SoundManager)

	self.enabled = true	-- サウンド有効/無効フラグ

	self.folder = Instance.new("Folder")
	self.folder.Name = "NSP_Sounds"
	self.folder.Parent = parent

	-- サウンドの登録
	local function makeSound(name, soundId)
		local s = Instance.new("Sound")
		s.Name = name					-- サウンド名
		s.SoundId = soundId or ""		-- サウンドID(Asset登録値 "rbxassetid://XXXX" の形式)
		s.Volume = DEFAULT_VOLUME		-- 音量
		s.Parent = self.folder			-- 親GUIインスタンス
		return s
	end

	self.stack = makeSound("Stack", "rbxassetid://71086533645818")
	self.merge1 = makeSound("Merge1", "rbxassetid://133256535351690")
	self.merge2 = makeSound("Merge2", "rbxassetid://74465701802181")
	self.merge3 = makeSound("Merge3", "rbxassetid://77709126479417")

	return self
end

-- =====================================================================
-- サウンド再生の有効化/無効化
-- @param enabled サウンド再生の有効化/無効化設定
-- @return none
function SoundManager:SetEnabled(enabled)
	self.enabled = enabled and true or false
end

-- =====================================================================
-- サウンド再生の有効化/無効化状態の取得
-- @param none
-- @return サウンド再生が有効化状態の設定ならtrueを返す
function SoundManager:IsEnabled()
	return self.enabled
end

-- =====================================================================
-- サウンド再生の指示
-- @param sound 再生させるサウンド
-- @return none
-- note:
-- サウンド再生が無効化状態の設定なら再生しない。
-- 再生前にStopを入れることで同一フレーム内の連続再生を保証する。
function SoundManager:PlaySound(sound)
	if not self.enabled then
		return
	end

	if sound and sound.SoundId ~= "" then
		sound:Stop()
		sound:Play()
	end
end

-- =====================================================================
-- スタック時用のサウンド再生の指示
-- @param none
-- @return none
function SoundManager:PlayStack()
	self:PlaySound(self.stack)
end

-- =====================================================================
-- マージ時用のサウンド再生の指示
-- @param diff マージの規模
-- @return none
-- note:
-- マージ規模によって再生するサウンドを選択する。
-- diffは mergeVal - 元の値 を想定
function SoundManager:PlayMergeByDiff(diff)
	if diff <= 1 then
		self:PlaySound(self.merge1)
	elseif diff == 2 then
		self:PlaySound(self.merge2)
	else
		self:PlaySound(self.merge3)
	end
end

-- =====================================================================
-- マージ時用(規模3)のサウンド再生の指示
-- @param none
-- @return none
function SoundManager:PlayMerge3()
	self:PlaySound(self.merge3)
end

return SoundManager
