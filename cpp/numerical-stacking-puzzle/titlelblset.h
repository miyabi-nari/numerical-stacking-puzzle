/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: titlelblset.h													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: タイトル用ラベルオブジェクト全体管理							*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRTitleLblSet : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRTitleLblSet(void);
	virtual	~CRTitleLblSet(void);
private:
	//----- 変数
	//----- 関数
	// GUI系の子プロセスの生成
	bool	CreateGuiChild(void);
	// 子プロセスの生存判定
	bool	IsChildAlive(void);
	// シーケンス
	bool	(CRTitleLblSet::*m_lpSeq[10])(void);
	long	m_nSeq;			// 処理シーケンスNo.
	bool	SeqInit(void);	// 初期状態
	bool	SeqIdle(void);	// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
};
