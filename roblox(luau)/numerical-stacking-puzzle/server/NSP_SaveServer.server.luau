--[[====================================================================
 Copyright 2026 Miyabi
 NSP_Client.client.luau
 サーバー側スクリプト
======================================================================]]
local DataStoreService = game:GetService("DataStoreService")
local ReplicatedStorage = game:GetService("ReplicatedStorage")

-- 開発フラグ
local USE_DATASTORE = true  -- サーバー動作時はtrue、ローカル動作時はfalseとすること

local store = nil

if USE_DATASTORE then
	store = DataStoreService:GetDataStore("NumericalStackingPuzzle_Save_v10")
end

-- リモート生成（これは必ず動く）
local remotes = ReplicatedStorage:FindFirstChild("NSP_Remotes")
if not remotes then
	remotes = Instance.new("Folder")
	remotes.Name = "NSP_Remotes"
	remotes.Parent = ReplicatedStorage
end

local saveRemote = remotes:FindFirstChild("SaveGame")
if not saveRemote then
	saveRemote = Instance.new("RemoteFunction")
	saveRemote.Name = "SaveGame"
	saveRemote.Parent = remotes
end

local loadRemote = remotes:FindFirstChild("LoadGame")
if not loadRemote then
	loadRemote = Instance.new("RemoteFunction")
	loadRemote.Name = "LoadGame"
	loadRemote.Parent = remotes
end

-- 開発用メモリ保存
local devStorage = {}

saveRemote.OnServerInvoke = function(player, data)
	if typeof(data) ~= "table" then
		return false
	end

	if USE_DATASTORE then
		local ok = pcall(function()
			store:SetAsync(tostring(player.UserId), data)
		end)
		return ok
	else
		-- メモリ保存
		devStorage[player.UserId] = data
		print("DEV Save:", player.UserId)
		return true
	end
	return ok and true or false
end

loadRemote.OnServerInvoke = function(player)
	if USE_DATASTORE then
		local ok, data = pcall(function()
			return store:GetAsync(tostring(player.UserId))
		end)
		if ok then
			return data
		end
		return nil
	else
		-- メモリロード
		print("DEV Load:", player.UserId)
		return devStorage[player.UserId]
	end
end
