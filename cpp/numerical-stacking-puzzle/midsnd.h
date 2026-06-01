/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: midsnd.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 音再生のミドルウェア											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// 音ID
#define	E_MIDSND_STACK		(0)	// 音ID:スタック音
#define	E_MIDSND_MERGE_1	(1)	// 音ID:マージ音1
#define	E_MIDSND_MERGE_2	(2)	// 音ID:マージ音2
#define	E_MIDSND_MERGE_3	(3)	// 音ID:マージ音3
#define	E_MIDSND_NUM		(4)	// 音IDの総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRMidSnd : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRMidSnd();
			CRMidSnd(HINSTANCE hInst);
	virtual	~CRMidSnd();
private:
	//----- static変数
	static	CRMidSnd* m_lpThis;
	//----- 変数
	HINSTANCE	m_hAppInst;			// インスタンスハンドル
	bool	m_bDePlay;				// 再生禁止FG
	// 変数(音再生情報)
	bool	m_abSndSet[E_MIDSND_NUM];
	//----- 関数
	// 音の再生処理
	void	PlaySnd(void);
	// シーケンス
	long	m_nSeq;		// 処理シーケンスNo.
	bool	(CRMidSnd::*m_lpSeq[10])(void);
	bool	SeqInit();	// 初期状態
	bool	SeqIdle();	// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update();
	// 再生禁止FGの更新
	void	DisablePlay(bool bDePlay){m_bDePlay = bDePlay;};
	// 再生禁止FGの取得
	bool	GetDisablePlay(void){return m_bDePlay;};
	// 再生する音声を登録する
	void	SetSnd(long nSndID);

	static	CRMidSnd*	GetInstPt(void){return m_lpThis;};
};
