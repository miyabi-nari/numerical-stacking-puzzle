/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appboxset.h													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 箱オブジェクト全体管理										*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// 箱全体処理状態
#define	E_APP_BOX_SET_STAT_IDLE		( 0)	// 箱全体処理状態：待機
#define	E_APP_BOX_SET_STAT_STACK	( 1)	// 箱全体処理状態：積上動作
#define	E_APP_BOX_SET_STAT_MERGE	( 2)	// 箱全体処理状態：結合動作
#define	E_APP_BOX_SET_STAT_LAND		( 3)	// 箱全体処理状態：着地動作
#define	E_APP_BOX_SET_STAT_NUM		( 4)	// 箱全体処理状態の総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRAppBoxSet : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRAppBoxSet(void);
	virtual	~CRAppBoxSet(void);
private:
	//----- 変数
	long	m_nStat;		// 箱全体処理状態
	long	m_anBoxRt[E_CELL_X_NUM][E_CELL_Y_NUM];	// 積箱のルーチンNo.
	long	m_tmSeq;		// シーケンスタイマー
	//----- 関数
	// GUI系の子プロセスの生成
	bool	CreateGuiChild(void);
	// 子プロセスの生存判定
	bool	IsChildAlive(void);
	// シーケンス
	bool	(CRAppBoxSet::*m_lpSeq[10])(void);
	long	m_nSeq;			// 処理シーケンスNo.
	bool	SeqInit(void);	// 初期状態
	bool	SeqIdle(void);	// 待機状態
	bool	SeqStack(void);	// 積上動作状態
	bool	SeqMerge(void);	// 結合動作状態
	bool	SeqLand(void);	// 着地動作状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// 箱全体処理状態を積上動作に設定
	void	SetStatStack(void);
	// 箱全体処理状態を結合動作に設定
	void	SetStatMerge(void);
	// 箱全体処理状態を着地動作に設定
	void	SetStatLand(void);
};
