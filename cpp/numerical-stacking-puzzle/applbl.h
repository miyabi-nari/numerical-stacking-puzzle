/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: applbl.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ラベルオブジェクト											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// ラベル種類
#define	E_APP_LBL_TYPE_NEXT_BOX				( 0)	// 次落下箱案内
#define	E_APP_LBL_TYPE_SCORE_HEAD			( 1)	// スコア項目
#define	E_APP_LBL_TYPE_ELAPSED_TIME_HEAD	( 2)	// 経過時間項目
#define	E_APP_LBL_TYPE_MERGED_TIME_HEAD		( 3)	// マージ時間項目
#define	E_APP_LBL_TYPE_SCORE_VAL			( 4)	// スコア値
#define	E_APP_LBL_TYPE_ELAPSED_TIME_VAL		( 5)	// 経過時間値
#define	E_APP_LBL_TYPE_MERGED_TIME_VAL_1	( 6)	// マージ時間値1
#define	E_APP_LBL_TYPE_MERGED_TIME_VAL_2	( 7)	// マージ時間値2
#define	E_APP_LBL_TYPE_MERGED_TIME_VAL_3	( 8)	// マージ時間値3
#define	E_APP_LBL_TYPE_MERGED_TIME_VAL_4	( 9)	// マージ時間値4
#define	E_APP_LBL_TYPE_MERGED_TIME_VAL_5	(10)	// マージ時間値5
#define	E_APP_LBL_TYPE_NUM					(11)	// ラベル種類の総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRAppLbl : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRAppLbl(void);
			CRAppLbl(long nLblTypeID);
	virtual	~CRAppLbl(void);
private:
	//----- 変数
	long	m_nLblTypeID;	// ラベル種類
	long	m_nVal1;		// 表示値1
	long	m_nVal2;		// 表示値2
	long	m_nValOld;		// 表示値old
	long	m_nGuiTxtID;	// GUIのテキストID
	//----- 関数
	// GUI表示の更新処理
	bool	UpdateGui(void);
	// シーケンス
	bool	(CRAppLbl::*m_lpSeq[20])(void);
	long	m_nSeq;				// 処理シーケンスNo.
	bool	SeqInit(void);		// 初期状態
	bool	SeqIdle(void);		// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// ラベル表示値を設定
	void	SetVal(long nVal1, long nVal2);
};
