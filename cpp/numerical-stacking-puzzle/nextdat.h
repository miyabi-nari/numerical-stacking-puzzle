/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: nextdat.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 次に落下する箱のデータ管理									*
********************************************************************************/
#ifndef NEXTDAT_H
#define NEXTDAT_H

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
typedef struct T_LD_NEXTDAT
{
	long nVal;		// 格納値
}T_LD_NEXTDAT;

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRNextDat : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRNextDat(void);
	virtual	~CRNextDat(void);
private:
	//----- static変数
	static	CRNextDat*	m_lpThis;
	//----- 変数
	long	m_tmBoxVal;		// 箱の値ランダムタイマ
	long	m_nNextBoxCnt;	// 次回箱の生成数
	long	m_anNextBoxVal[E_NEXT_BOX_NUM];	// 次回箱の格納値
	//----- 関数
	// 次回箱の値を算出する
	long	GetNextBoxNum(void);
	// シーケンス
	bool	(CRNextDat::*m_lpSeq[10])(void);
	long	m_nSeq;		// 処理シーケンスNo.
	bool	SeqInit();	// 初期状態
	bool	SeqIdle();	// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update();
	// 次回箱の格納値を設定
	void	SetVal(long nX, long nVal);
	// 次回箱の格納値を取得
	long	GetVal(long nX);
	// 次回箱の生成数を取得
	long	GetNextBoxCnt(void);
	// 次回箱の生成数を設定
	void	SetNextBoxCnt(long nNextBoxCnt);
	// 次回箱の値ランダムタイマを取得
	long	GetBoxValTm(void);
	// 次回箱の値ランダムタイマを設定
	void	SetBoxValTm(long tmBoxVal);
	// 次回箱の落下指示
	long	ExecPush(void);

	static	CRNextDat*	GetInstPt(void){return m_lpThis;};

	// 次回箱の格納値の変数アドレスを取得
	long*	GetNextBoxValAdr(void){return &m_anNextBoxVal[0];};
};

#endif// NEXTDAT_H
