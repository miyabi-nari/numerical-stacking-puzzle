/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: titlelbl.h													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: タイトル用ラベルオブジェクト									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// ラベル種類
#define	E_TITLE_LBL_TYPE_NAME			( 0)	// タイトルネーム項目
#define	E_TITLE_LBL_TYPE_VER			( 1)	// バージョン情報項目
#define	E_TITLE_LBL_TYPE_CR				( 2)	// コピーライト項目
#define	E_TITLE_LBL_TYPE_NEW_GAME		( 3)	// ニューゲーム項目
#define	E_TITLE_LBL_TYPE_RECENT_GAME	( 4)	// 最近のゲーム項目
#define	E_TITLE_LBL_TYPE_SOUND_ON		( 5)	// サウンドオン項目
#define	E_TITLE_LBL_TYPE_SOUND_OFF		( 6)	// サウンドオフ項目
#define	E_TITLE_LBL_TYPE_NUM			( 7)	// ラベル種類の総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRTitleLbl : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRTitleLbl(void);
			CRTitleLbl(long nLblTypeID);
	virtual	~CRTitleLbl(void);
private:
	//----- 変数
	long	m_nLblTypeID;	// ラベル種類
	long	m_nGuiTxtID;	// GUIのテキストID
	//----- 関数
	// GUI表示の更新処理
	bool	UpdateGui(void);
	// シーケンス
	bool	(CRTitleLbl::*m_lpSeq[20])(void);
	long	m_nSeq;				// 処理シーケンスNo.
	bool	SeqInit(void);		// 初期状態
	bool	SeqIdle(void);		// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
};
