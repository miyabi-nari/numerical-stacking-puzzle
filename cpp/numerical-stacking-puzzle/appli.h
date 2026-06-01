/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appli.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: アプリケーション層全体制御									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"
#include "wmstr.h"

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRAppli : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRAppli(void);
			CRAppli(HWND hWnd);
	virtual	~CRAppli(void);
private:
	//----- 変数
	HWND	m_hAppWnd;		// ウィンドウハンドル
	long	m_tmSeq;		// シーケンスタイマー
	long	m_nFallBoxX;	// 落下開始位置X
	long	m_nFallBoxY;	// 落下開始位置Y
	long	m_nFallBoxVal;	// 落下箱の値
	long	m_tmFall;		// 落下時間
	long	m_nScore;		// 得点
	long	m_nElapsedTime;	// 経過時間
	long	m_anMergeTime[5];	// マージ時間
	long	m_anMergeVal[5];	// マージ値
	bool	m_bTitleBack;		// タイトルバック指令
	bool	m_bExit;			// EXIT指令
	//----- 関数
	// パズル系の子プロセスの生成
	bool	CreatePzlChild(void);
	// パズル系の子プロセスの生存判定
	bool	IsPzlChildAlive(void);
	// パズル系のデータの初期設定
	bool	InitPzlData(void);
	// パズル系の子プロセスの破棄
	void	DeletePzlChild(void);
	// 経過時間の更新処理
	bool	UpdateElapsedTime(void);
	// マージ時間の更新処理
	bool	UpdateMergeTime(long nMaxMergeVal);
	// 読出ファイルの選択処理(Dialog)
	bool	OpenLoadFile(CWMStr* lpFilename);
	// ファイル保存オペレーション
	bool	SaveFileOp(void);
	// ファイル読出オペレーション
	bool	LoadFileOp(CWMStr* lpFilename);
	// シーケンス
	bool	(CRAppli::*m_lpSeq[10])(void);
	long	m_nSeq;			// 処理シーケンスNo.
	bool	SeqInit(void);	// 初期状態
	bool	SeqTitle(void);	// タイトル状態
	bool	SeqServe(void);	// サーブ状態
	bool	SeqFallSlow(void);	// 落下(遅)状態
	bool	SeqFallFast(void);	// 落下(速)状態
	bool	SeqStack(void);	// 積上状態
	bool	SeqMerge(void);	// 結合動作状態
	bool	SeqLand(void);	// 着地動作状態
	bool	SeqBack(void);	// タイトルバック状態
	bool	SeqExit(void);	// EXIT状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// タイトルバック指令
	void	SetTitleBack(void);
	// EXIT指令
	void	SetExit(void);
	// シーケンス状態がEXIT状態か否か取得
	bool	IsExitSeq(void);
};
