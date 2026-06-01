/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: debug.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: デバッグ用ライブラリ。エラー時のメッセージ表示用も込み		*
********************************************************************************/
#ifndef	DEBUG_H
#define	DEBUG_H

/********************************************************************************
*	Public functions															*
********************************************************************************/
void MsgBox(const LPWSTR lpMsg);	// エラー発生時のメッセージボックス表示

#ifdef _DEBUG

// デバッグビルド用。
void Debuglog(const LPWSTR lpMsg);
void Debuglog2(const LPWSTR lpMsg, long n);
void Debuglog3(const LPWSTR lpMsg, long n, long m);
void DumpData(void *lpData, unsigned long size);
void Assert(bool isOK);

#else

// リリースビルドでは削除する。
#define Debuglog(XXXX)
#define Debuglog2(XXXX, YYYY)
#define Debuglog3(XXXX, YYYY, ZZZZ)
#define DumpData(XXXX, YYYY)
#define Assert(XXXX)

#endif

#endif// DEBUG_H
