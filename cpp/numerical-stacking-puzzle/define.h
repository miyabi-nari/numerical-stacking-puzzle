/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: define.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 全ファイル共通宣言											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "debug.h"

/********************************************************************************
*	Pragma																		*
********************************************************************************/
#pragma warning( disable : 4800 )  // 警告 4800(long->bool強制変換) を無効にする。

/********************************************************************************
*	Defines																		*
********************************************************************************/
#define	WCHAR_NULL	(L'\0')	// ワイド文字列用のNULL

#define	E_APP_NAME_STR	(L"Numerical Stacking Puzzle")	// アプリケーション名
#define	E_APP_VER_STR	(L"Version 0.1")				// アプリケーションバージョン文字列
#define	E_APP_VER_NUM	(10)							// アプリケーションバージョン番号
#define	E_APP_CR_STR	(L"Copyright(C) 2026 Miyabi.")	// アプリケーション著作権情報

#define	E_APP_SAVE_ID_KEY	(L"N.S.P.SaveData.Version")	// 保存データの識別キー
#define	E_APP_SAVE_ID_NUM	10							// 保存データの識別Ver

#define	E_WID_X	1600
#define	E_WID_Y	 900

/********************************************************************************
*	TypeDefines																	*
********************************************************************************/
typedef unsigned long	u_long;
typedef unsigned short	u_short;
typedef unsigned char	u_char;

/********************************************************************************
*	上限値																		*
********************************************************************************/
#define	E_CELL_X_NUM	(11)	// セル数(横方向)
#define	E_CELL_Y_NUM	( 7)	// セル数(縦方向)

#define	E_NEXT_BOX_NUM	( 4)	// 次回箱の数

/********************************************************************************
*	ＩＤ情報																	*
********************************************************************************/

/********************************************************************************
*	Global Functions															*
********************************************************************************/
// ポインタ処理用マクロ
#define	NEW(POLD, PNEW)	{if(POLD != NULL){ delete POLD;} POLD = new PNEW;}
#define	FREE(P)		{if(P != NULL){ delete P; P = NULL;}}

// 数値評価用マクロ
#define	NUMOK(NNN,MMM)	(((NNN) >= 0 && (NNN) < (MMM)) ? TRUE : FALSE)

//-----------------------------------------------------------------------
// インライン展開関数
inline	void	Swap(long &n1, long &n2)
{
	long n3 = n1;
	n1 = n2;
	n2 = n3;
}

inline	long	Abs(long n)
{
	if(n < 0)
		return -n;
	else
		return n;
}

inline	float	FAbs(float f)
{
	if(f < 0.0f)
		return -f;
	else
		return f;
}

inline	float	FMin(float f1, float f2)
{
	if(f1 < f2)
		return f1;
	else
		return f2;
}

inline	float	FMax(float f1, float f2)
{
	if(f1 > f2)
		return f1;
	else
		return f2;
}

inline	long	Max(long n1, long n2)
{
	if(n1 > n2)
		return n1;
	else
		return n2;
}

inline	long	Min(long n1, long n2)
{
	if(n1 < n2)
		return n1;
	else
		return n2;
}

inline	long	Dec(long &n, long m = 1)
{
	if(n <= m)
	{
		n = 0;
	}
	else
	{
		n -= m;
	}
	return n;
}

// 矩形幅算出
long	RectWidth(RECT &rc);
// 矩形高さ算出
long	RectHeight(RECT &rc);

// 環境設定を取得
long	GetEnv(const LPWSTR lpKeyCode, long nDef);
// 環境設定を設定
void	SaveEnv(const LPWSTR lpKeyCode, long nVal);
