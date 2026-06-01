/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: debug.cpp														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: デバッグ用ライブラリ。エラー時のメッセージ表示用も込み		*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include <stdio.h>

/********************************************************************************
*	Public functions															*
********************************************************************************/

/********************************************************************************
* FunctionName:	MsgBox															*
* Function    :	異常処理時のメッセージボックス表示								*
* Argument(s) :	lpMsg=表示文字列												*
* Return      :	none															*
********************************************************************************/
void MsgBox(const LPWSTR lpMsg)
{
	size_t size;
	size = wcslen(lpMsg) + 5;
	
	LPWSTR lpBuf = new WCHAR[size];
	wcscpy_s(lpBuf, size, L"ERR:");
	wcscat_s(lpBuf, size, lpMsg);

	::MessageBoxW(NULL, lpBuf, NULL, MB_OK);
	Debuglog(lpBuf);

	delete lpBuf;
}

#ifdef _DEBUG

/********************************************************************************
* FunctionName:	Debuglog														*
* Function    :	デバッグ用のログファイル出力									*
* Argument(s) :	lpMsg=表示文字列												*
* Return      :	none															*
********************************************************************************/
static long g_nDLCnt;
static bool g_bDLInit;	// 初期化有無フラグ

void Debuglog(const LPWSTR lpMsg)
{
	if(lpMsg)
	{
		// 初回呼出時はファイル新規作成とする
		FILE* file;
		_wfopen_s(&file, L"debug.txt", (g_bDLInit) ? L"a" : L"w");
		if(file)
		{
			// 時刻を付ける
			WCHAR sTime[9];
			_wstrtime_s(sTime, 9);
			fwprintf(file, L"%06d-%s-%s\n", g_nDLCnt, sTime, lpMsg);
			fclose(file);
			g_bDLInit = true;
		}
	}
	else
	{
		g_nDLCnt++;
	}
}

/********************************************************************************
* FunctionName:	Debuglog2														*
* Function    :	デバッグ用のログファイル出力									*
* Argument(s) :	lpMsg=表示文字列, n=出力数値									*
* Return      :	none															*
* Attention(s): 文字列と出力数値はfwprintfでフォーマット書込みするので要注意。	*
********************************************************************************/
void Debuglog2(const LPWSTR lpMsg, long n)
{
	if(lpMsg)
	{
		// 初回呼出時はファイル新規作成とする
		FILE* file;
		WCHAR buf[256];
		wsprintf(buf, lpMsg, n);
		_wfopen_s(&file, L"debug.txt", (g_bDLInit) ? L"a" : L"w");
		if(file)
		{
			// 時刻を付ける
			WCHAR sTime[9];
			_wstrtime_s(sTime, 9);
			fwprintf(file, L"%06d-%s-%s\n", g_nDLCnt, sTime, buf);
			fclose(file);
			g_bDLInit = true;
		}
	}
	else
	{
		g_nDLCnt++;
	}
}

/********************************************************************************
* FunctionName:	Debuglog3														*
* Function    :	デバッグ用のログファイル出力									*
* Argument(s) :	lpMsg=表示文字列, n=出力数値1, m=出力数値2						*
* Return      :	none															*
* Attention(s): 文字列と出力数値はfwprintfでフォーマット書込みするので要注意。	*
********************************************************************************/
void Debuglog3(const LPWSTR lpMsg, long n, long m)
{
	if(lpMsg)
	{
		// 初回呼出時はファイル新規作成とする
		FILE* file;
		WCHAR buf[256];
		wsprintf(buf, lpMsg, n, m);
		_wfopen_s(&file, L"debug.txt", (g_bDLInit) ? L"a" : L"w");
		if(file)
		{
			// 時刻を付ける
			WCHAR sTime[9];
			_wstrtime_s(sTime, 9);
			fwprintf(file, L"%06d-%s-%s\n", g_nDLCnt, sTime, buf);
			fclose(file);
		
			g_bDLInit = true;
		}
	}
	else
	{
		g_nDLCnt++;
	}
}

/********************************************************************************
* FunctionName:	DumpData														*
* Function    :	デバッグ情報出力												*
* Argument(s) :	lpMsg=出力データ, size=出力バイト数								*
* Return      :	none															*
* Attention(s): 文字列と出力数値はfwprintfでフォーマット書込みするので要注意。	*
********************************************************************************/
void DumpData(void *lpData, unsigned long size)
{
	static bool bInit;	// 初期化有無ＦＧ
	FILE* file;
	
	// 初回呼出時はファイル新規作成とする
	_wfopen_s(&file, L"dump.dat", (bInit) ? L"ab" : L"wb");

	if(file)
	{
		fwrite(lpData, 1, size, file);
		fclose(file);
	}

	bInit = true;

	return;
}

/********************************************************************************
* FunctionName:	Assert															*
* Function    :	デバッグビルド用 不正値を引っかける								*
* Argument(s) :	isOK=FALSEならメッセージボックス表示							*
* Return      :	none															*
********************************************************************************/
void Assert(bool isOK)
{
	if(isOK == FALSE)
	{
		::MessageBoxW(NULL, L"Assert", NULL, MB_OK);
	}
}

#endif
