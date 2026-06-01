/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: routine.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 全ルーチン管理基底クラス										*
********************************************************************************/
#ifndef	ROUTINE_H
#define	ROUTINE_H

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRoutine();
	virtual	~CRoutine();
private:
	//----- 変数
	bool	m_bInitOK;		// 初期化正常終了フラグ
	long	m_nChildNum;	// 子ルーチン数
	//----- 関数
	void	Cleanup();		// 子ルーチンのクリア処理
protected:
	//----- 子ルーチン
	CRoutine	**m_lplpRtChild;	// 子ルーチンへのポインタ
	long	*m_lpChildRet;			// 子ルーチンからの戻り値
	//----- 継承用変数
	long	m_nReturn;	// 親ルーチンへの戻り値
	//----- 継承用関数
	// 子ルーチン数の設定と初期化
	bool	SetChildNum(long nChildNum);
	// 初期化正常終了フラグセット
	void	InitOK(void);
public:
	//----- 関数
	virtual	bool	Update();
};

#endif// ROUTINE_H
