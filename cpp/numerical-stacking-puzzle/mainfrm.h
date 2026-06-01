/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: mainfrm.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 全体構造制御													*
********************************************************************************/
#ifndef	MAINFRM_H
#define	MAINFRM_H

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"
#include "timerlib.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRMainFrm : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRMainFrm(void);
			CRMainFrm(HINSTANCE hInst, HWND hWnd);
	virtual	~CRMainFrm(void);
private:
	//----- 変数
	CTimerLib	m_cTimer;		// タイマーオブジェクト
	HINSTANCE	m_hAppInst;		// インスタンスハンドル
	HWND		m_hAppWnd;		// ウィンドウハンドル
	long		m_tm1stWait;	// 起動初期待機カウンタ
	// シーケンス
	bool	(CRMainFrm::*m_lpSeq[10])(void);
	long	m_nSeq;				// 処理シーケンスNo.
	bool	SeqInit(void);		// 初期状態
	bool	SeqMid(void);		// ミドルウェア起動中
	bool	SeqAppli(void);		// アプリケーション動作中
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// WindowMessageの伝達処理
	void	PassWndMsg(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif// MAINFRM_H
