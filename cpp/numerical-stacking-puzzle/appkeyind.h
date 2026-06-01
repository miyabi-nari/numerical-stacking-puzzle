/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appkeyind.h													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: キー表示オブジェクト											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// キー表示種類
#define	E_APP_KEY_IND_TYPE_00	( 0)	// キー位置 0番目
#define	E_APP_KEY_IND_TYPE_01	( 1)	// キー位置 1番目
#define	E_APP_KEY_IND_TYPE_02	( 2)	// キー位置 2番目
#define	E_APP_KEY_IND_TYPE_03	( 3)	// キー位置 3番目
#define	E_APP_KEY_IND_TYPE_04	( 4)	// キー位置 4番目
#define	E_APP_KEY_IND_TYPE_05	( 5)	// キー位置 5番目
#define	E_APP_KEY_IND_TYPE_06	( 6)	// キー位置 6番目
#define	E_APP_KEY_IND_TYPE_07	( 7)	// キー位置 7番目
#define	E_APP_KEY_IND_TYPE_08	( 8)	// キー位置 8番目
#define	E_APP_KEY_IND_TYPE_09	( 9)	// キー位置 9番目
#define	E_APP_KEY_IND_TYPE_10	(10)	// キー位置10番目
#define	E_APP_KEY_IND_TYPE_NUM	(11)	// キー表示種類の総数

// キー表示状態
#define	E_APP_KEY_IND_STAT_KEY_OFF	( 0)	// キー表示状態：キーオフ表示
#define	E_APP_KEY_IND_STAT_KEY_ON	( 1)	// キー表示状態：キーオン表示
#define	E_APP_KEY_IND_STAT_NUM		( 2)	// キー表示状態の総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRAppKeyInd : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRAppKeyInd(void);
			CRAppKeyInd(long nKeyIndTypeID, long nPosX, long nPosY);
	virtual	~CRAppKeyInd(void);
private:
	//----- 変数
	long	m_nKeyIndTypeID;	// キー表示種類
	long	m_nPosX;		// 位置X
	long	m_nPosY;		// 位置Y
	long	m_nStat;		// キー表示状態
	long	m_nGuiRectID;	// GUIの矩形ID
	long	m_nGuiTxtA_ID;	// GUIのテキストAのID
	long	m_nGuiTxtB_ID;	// GUIのテキストBのID
	//----- 関数
	// GUI表示の更新処理
	bool	UpdateGui(void);
	// シーケンス
	bool	(CRAppKeyInd::*m_lpSeq[20])(void);
	long	m_nSeq;				// 処理シーケンスNo.
	bool	SeqInit(void);		// 初期状態
	bool	SeqIdle(void);		// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// 位置を設定
	void	SetPos(long nPosX, long nPosY);
	// キー表示状態を取得
	long	GetStat(void);
};
