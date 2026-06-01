/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: celldat.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: セルのデータ管理												*
********************************************************************************/
#ifndef CELLDAT_H
#define CELLDAT_H

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
typedef struct T_LD_CELLDAT
{
	long nVal;		// セル格納値
	long nStackVal;	// スタック処理値
	long nMergeX;	// マージ位置X
	long nMergeY;	// マージ位置Y
	long nMergeVal;	// マージ処理値
	long nLandY;	// 落下位置Y
	long nLandVal;	// 落下情報(使用しない)
}T_LD_CELLDAT;

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRCellDat : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRCellDat(void);
	virtual	~CRCellDat(void);
private:
	//----- static変数
	static	CRCellDat*	m_lpThis;
	//----- 変数
	T_LD_CELLDAT	m_atatCellDat[E_CELL_X_NUM][E_CELL_Y_NUM];
	long	m_nScore;			// 得点
	//----- 関数
	// シーケンス
	bool	(CRCellDat::*m_lpSeq[10])(void);
	long	m_nSeq;		// 処理シーケンスNo.
	bool	SeqInit();	// 初期状態
	bool	SeqIdle();	// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update();
	// セル格納値を設定
	void	SetVal(long nX, long nY, long nVal);
	// セル格納値を取得
	long	GetVal(long nX, long nY);
	// スタック情報を取得
	long	GetStackVal(long nX, long nY);
	// マージするセルの値を取得
	long	GetMergeVal(long nX, long nY);
	// マージ位置を取得
	long	GetMergeX(long nX, long nY);
	// マージ位置を取得
	long	GetMergeY(long nX, long nY);
	// 落下位置を取得
	long	GetLandY(long nX, long nY);
	// セル格納値の積み上げ可否を検査
	bool	CheckEnStack(long nX, long nY, long nVal);
	// セル格納値の積み上げを実行
	void	ExecStack(void);
	// セル格納値のマージ可否を検査
	bool	CheckMerge(long nX, long nY, long* lpMaxMergeVal, long* lpMaxMergeDifVal);
	long	CheckMergeSub(long nX, long nY, long nBaseX, long nBaseY);
	// セル格納値のマージを実行
	void	ExecMerge(void);
	// セル格納値の着地可否を検査
	bool	CheckEnLand(void);
	bool	CheckEnLandSub(long nX, long nY);
	// セル格納値の着地を実行
	void	ExecLand(void);
	// 得点計算
	bool	CalcScore(void);
	// 得点を取得
	long	GetScore(void);
	// 得点を設定
	void	SetScore(long nScore);

	static	CRCellDat*	GetInstPt(void){return m_lpThis;};

	// 格納値の変数アドレスを取得
	T_LD_CELLDAT*	GetCellDatAdr(void){return &m_atatCellDat[0][0];};
};

#endif// CELLDAT_H
