/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appnextboxset.h												*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 次回箱オブジェクト全体管理									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// 次回箱全体処理状態
#define	E_APP_NEXT_BOX_SET_STAT_IDLE	( 0)	// 次回箱全体処理状態：待機
#define	E_APP_NEXT_BOX_SET_STAT_PUSH	( 1)	// 次回箱全体処理状態：PUSH動作
#define	E_APP_NEXT_BOX_SET_STAT_NUM		( 2)	// 次回箱全体処理状態の総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRAppNextBoxSet : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRAppNextBoxSet(void);
	virtual	~CRAppNextBoxSet(void);
private:
	//----- 変数
	long	m_nStat;		// 次回箱全体処理状態
	long	m_tmSeq;		// シーケンスタイマー
	//----- 関数
	// GUI系の子プロセスの生成
	bool	CreateGuiChild(void);
	// 子プロセスの生存判定
	bool	IsChildAlive(void);
	// シーケンス
	bool	(CRAppNextBoxSet::*m_lpSeq[10])(void);
	long	m_nSeq;			// 処理シーケンスNo.
	bool	SeqInit(void);	// 初期状態
	bool	SeqIdle(void);	// 待機状態
	bool	SeqPush(void);	// PUSH動作状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// 次回箱全体処理状態をPUSH動作に設定
	void	SetStatPush(void);
};
