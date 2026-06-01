/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: midkey.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: Key入力のミドルウェア											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
#define	E_MIDKEY_QWERTY_NUM	(11)

//-------------------------------------------------------------------------------
// キー入力情報
typedef union KEYINFO
{
	bool	abBit[1];	// 全キー状態を配列として扱うための参照口（内部処理用）
	struct
	{
		// マウス位置
		bool	mouseL;	// マウス左クリック
		bool	mouseR;	// マウス右クリック
		// キー配列
		bool	qwerty[E_MIDKEY_QWERTY_NUM];
		// 共通処理用
		bool	esc;	// エスケープキー
		bool	fn[12];	// F1～F12キー
	};
}KEYINFO;

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRMidKey : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRMidKey(void);
			CRMidKey(HWND hWnd);
	virtual	~CRMidKey(void);
private:
	//----- static変数
	static	CRMidKey* m_lpThis;
	//----- 変数
	HWND	m_hAppWnd;		// ウィンドウハンドル
	// キー入力バッファ
	long	m_nMouseX;		// マウス入力状態(X座標)
	long	m_nMouseY;		// マウス入力状態(Y座標)
	KEYINFO	m_kiKeyIn;		// キー入力情報(input)
	KEYINFO	m_kiKeyOn;		// キー入力情報(確定値)
	KEYINFO	m_kiKeyDown;	// キー入力情報(ON立上り検出)
	//----- 関数
	// マウス位置情報の更新処理
	void	UpdateMousePos(void);
	// キー入力状態の更新処理
	void	UpdateKey(void);
	// シーケンス
	bool	(CRMidKey::*m_lpSeq[10])(void);
	long	m_nSeq;				// 処理シーケンスNo.
	bool	SeqInit(void);		// 初期状態
	bool	SeqIdle(void);		// 待機中
public:
	//----- 関数
	bool	Update(void);
	// キー入力情報(確定値)の取得
	KEYINFO* GetKeyOn(void){return &m_kiKeyOn;};
	// キー入力情報(ON立上り検出)の取得
	KEYINFO* GetKeyDown(void){return &m_kiKeyDown;};
	// キー入力情報(input)を設定(マウスLクリック)
	void	SetMouseL_In(bool bDown){m_kiKeyIn.mouseL = bDown;};
	// キー入力情報(input)を設定(ESCキー)
	void	SetEscIn(bool bDown){m_kiKeyIn.esc = bDown;};
	// キー入力情報(input)を設定(F1～F12キー)
	void	SetFnIn(long nIndex, bool bDown){m_kiKeyIn.fn[nIndex] = bDown;};
	// キー入力情報(input)を設定(キー配列)
	void	SetQwertyIn(long nIndex, bool bDown){m_kiKeyIn.qwerty[nIndex] = bDown;};
	// キー入力情報(input)をクリア
	void	SetAllInOff(void){memset(&m_kiKeyIn, 0, sizeof(m_kiKeyIn));};
	// マウス入力状態(X座標ポイント)を取得
	long	GetMouseX(void){return m_nMouseX;};
	// マウス入力状態(Y座標ポイント)を取得
	long	GetMouseY(void){return m_nMouseY;};

	static	CRMidKey*	GetInstPt(void){return m_lpThis;};
};
