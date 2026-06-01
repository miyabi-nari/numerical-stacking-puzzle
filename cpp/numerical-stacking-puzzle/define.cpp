/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: define.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 全ファイル共通宣言											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "fileio.h"
#include "wmstr.h"

/********************************************************************************
*	Global Functions															*
********************************************************************************/

/********************************************************************************
* FunctionName:	RectWidth														*
* Function    :	矩形幅算出														*
* Argument    :	rc=矩形															*
* Return      :	矩形幅															*
********************************************************************************/
long	RectWidth(RECT &rc)
{
	return rc.right - rc.left;
}

/********************************************************************************
* FunctionName:	RectHeight														*
* Function    :	矩形高さ算出													*
* Argument    :	rc=矩形															*
* Return      :	矩形高さ														*
********************************************************************************/
long	RectHeight(RECT &rc)
{
	return rc.bottom - rc.top;
}

/********************************************************************************
* FunctionName:	GetEnv															*
* Function    :	環境設定を取得													*
* Argument    :	キーコード、取得できなかった場合のデフォルト値					*
* Return      :	環境設定値														*
********************************************************************************/
#define ENV_MAX 4096
WCHAR aEnv[ENV_MAX];

long	GetEnv(const LPWSTR lpKeyCode, long nDef)
{
	long n;
	char aFileBuf[ENV_MAX];

	if(aEnv[0] == WCHAR_NULL)
	{
		// 環境データ無し -> 読み込み
		CFileIO fio;
		if(!fio.Open((LPWSTR)(L"environment.txt"), FOPEN_READ) ||
		   !fio.Read(aFileBuf, Min(fio.Size(), ENV_MAX)))
		{
			// 読み込み失敗
			aEnv[1] = WCHAR_NULL;
			aEnv[0] = WCHAR_NULL;
			return nDef;
		}
		// 読み込み成功
		aFileBuf[Min(fio.Size(), ENV_MAX)-1] = '\0';
		mbstowcs_s(nullptr, aEnv, ENV_MAX, aFileBuf, ENV_MAX);
		aEnv[ENV_MAX-1] = WCHAR_NULL;
		_wcsupr_s(aEnv, ENV_MAX);	// 大文字に統一する
		for(n = (long)wcslen(aEnv); n >= 0; n--)
		{
			if(aEnv[n] == L'=')
				aEnv[n] = L' ';
		}
	}
	if(wcslen(aEnv) <= 3)
	{
		// 有効な環境データがないためデフォルト値を返す
		return nDef;
	}

	// キーコード取得
	CWMStr wmsEnv;
	wmsEnv.SetW(aEnv);
	wmsEnv.SeekClrW();
	if(wmsEnv.SeekFindW(lpKeyCode))
	{
		wmsEnv.SeekMoveW(wcslen(lpKeyCode));
		if(wmsEnv.SeekScanDigW(&n) == 0)
		{
			// 有効な環境データがないためデフォルト値を返す
			return nDef;
		}
		return n;
	}

	// 有効な環境データがないためデフォルト値を返す
	return nDef;
}

/********************************************************************************
* FunctionName:	SaveEnv															*
* Function    :	環境設定を設定													*
* Argument    :	キーコード、設定値												*
* Return      :	none															*
********************************************************************************/
void	SaveEnv(const LPWSTR lpKeyCode, long nVal)
{
	static bool bFirst = FALSE;
	CFileIO fio;
	if(!fio.Open((LPWSTR)(L"environment.txt"), (bFirst) ? FOPEN_APPEND : FOPEN_WRITE))
		return;

	CWMStr wmsFileBuf;

	wmsFileBuf.SetW(lpKeyCode);
	wmsFileBuf.AddW((LPWSTR)(L"="));
	if(!fio.WriteLn(wmsFileBuf.PtW(), nVal))
		return;

	bFirst = TRUE;
}
