--[[====================================================================
 Copyright 2026 Miyabi
 NSP_Client.client.luau
 クライアント側スクリプト
======================================================================]]
local RunService = game:GetService("RunService")
local Players = game:GetService("Players")
local ReplicatedStorage = game:GetService("ReplicatedStorage")

local player = Players.LocalPlayer
local playerGui = player:WaitForChild("PlayerGui")

local NSP = ReplicatedStorage:WaitForChild("NSP")

local InputController = require(NSP.InputController)
local ViewRenderer = require(NSP.ViewRenderer)
local SoundManager = require(NSP.SoundManager)
local GameController = require(NSP.GameController)

local input = InputController.new()
input:Bind()

local view = ViewRenderer.new(playerGui, input)
local sound = SoundManager.new(view.gui)

local gameController = GameController.new(input, view, sound)

local accumulator = 0
local step = 1 / 60

RunService.Heartbeat:Connect(function(dt)
	accumulator += dt

	while accumulator >= step do
		accumulator -= step

		gameController:UpdateOneFrame()
		input:EndFrame()
	end
end)
