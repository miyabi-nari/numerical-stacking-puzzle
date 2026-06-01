/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: timerlib.h													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: タイマー制御クラスライブラリ									*
* Attention		: Winmm.lib 必須												*
*				  mmsystem.h 必須												*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include <MMSystem.h>

/********************************************************************************
*	Define																		*
********************************************************************************/
// TIMER_IS: 時間経過状況。(GetTimeStat()の戻り値)
#define	TIMER_ISIDLE	(0)	// タイマー到達まで余裕あり
#define	TIMER_ISSOON	(1)	// タイマー到達直前
#define	TIMER_ISJUST	(2)	// タイマー到達
#define	TIMER_ISOVER	(3)	// タイマー超過

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CTimerLib
{
public:
	//----- Constructor(s)/Destructor
			CTimerLib();
	virtual	~CTimerLib();
public:
	//----- タイマーコールバック関数
	static void CALLBACK TimerProc(UINT uTimerID,
	                               UINT uMsg,
	                               DWORD_PTR dwUser,
	                               DWORD_PTR dw1,
	                               DWORD_PTR dw2);
	//----- 関数
	// タイマー初期化処理（タイマー最大精度取得と設定）
	bool	Create(u_short nResolution);
	// タイマー動作開始(引数0の場合はデフォルト周波数)
	bool	Start(u_short nFPS = 0);
	// タイマー動作停止
	bool	Kill();
	// タイマー解放処理
	void	Cleanup();
	// タイマー状態取得
	long	GetTimeStat();
private:
	//----- 変数
	u_long	m_uTimerID;			// タイマーID
	long	m_uPeriod;			// タイマー分解能（最大可能分解能）
	bool	m_bCreate;			// 初期化フラグ
	long	m_nTimeResolution;	// 分解能（使用する分解能）
	long	m_nFPS;				// タイマー周波数（Hz）

	volatile long m_nNowTime;	// 現在の時間 ms（volatile=最適化防止）
	long	m_nNextTime;		// 次回タイマー到達時刻
	long	m_nFPS_Cnt;			// 内部カウンタ
	long	m_nWatchDog;		// ウォッチドッグ
};
