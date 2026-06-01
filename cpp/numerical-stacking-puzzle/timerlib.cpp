/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: timerlib.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: タイマー制御クラスライブラリ									*
* Attention		: Winmm.lib 必須												*
*				  mmsystem.h 必須												*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "timerlib.h"

#pragma comment(lib, "Winmm.lib")

/********************************************************************************
*	Defines																		*
********************************************************************************/
#define E_FPS_DEFAULT	(  60)	// FramesPerSeconds DefaultValue
#define E_TM_WDT		(5000)	// ウォッチドッグタイマー(5s)
#define E_TM_LIMIT		( 500)	// タイマーカウンタ加算打ち切り時間(500ms)

/********************************************************************************
*	CTimerLib Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CTimerLib														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CTimerLib::CTimerLib(void)
{
	m_uTimerID = NULL;	// タイマーID <- 未使用
	m_bCreate = FALSE;	// 初期化フラグ <- 未初期化
	m_nFPS = 1;			// 安全策のためここで１に初期化
	m_nWatchDog = 0;	// ウォッチドッグリセット
}

/********************************************************************************
* FunctionName:	~CTimerLib														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CTimerLib::~CTimerLib(void)
{
	CTimerLib::Cleanup();
}

/********************************************************************************
*	CTimerLib Public Method(s)													*
********************************************************************************/

/********************************************************************************
* Function Name	: TimerProc														*
* Function		: タイマーイベント処理											*
* Argument		: dwUser=タイマークラスへのポインタ								*
*				  その他の引数は使用しない										*
* Return		: none															*
* Caution(s)	: 通常，m_nTimeResolution (mSec.)周期で呼ばれるので				*
*				  それ以上の処理時間を消費しないこと							*
********************************************************************************/
void CALLBACK
CTimerLib::TimerProc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	// コールバック関数のため、thisポインタを明示的に取得する
	CTimerLib* lpTm = (CTimerLib*)dwUser;

	// ウォッチドッグタイマー（タイムオーバー -> 1,000msスリープ）
	lpTm->m_nWatchDog += lpTm->m_nTimeResolution;
	if(lpTm->m_nWatchDog > E_TM_WDT)
	{
		Sleep(1000);
	}

	// NextTimeを大幅に超えている場合，加算する意味が無くなるので加算しない
	if(lpTm->m_nNowTime - lpTm->m_nNextTime < E_TM_LIMIT)
	{
		lpTm->m_nNowTime += lpTm->m_nTimeResolution;
	}

	// タイマーカウンタ調整
	if(lpTm->m_nNowTime > 1000 && lpTm->m_nNextTime > 1000 && lpTm->m_nFPS_Cnt > lpTm->m_nFPS)
	{
		lpTm->m_nNowTime -= 1000;
		lpTm->m_nNextTime -= 1000;
		lpTm->m_nFPS_Cnt -= lpTm->m_nFPS;
	}
}

/********************************************************************************
* Function Name	: Create														*
* Function		: タイマー初期化処理（タイマー最大精度取得と設定）				*
* Argument		: nResolution=希望するタイマー分解能(msec)						*
*				  OSの分解能の値が高ければ無視する								*
* Return		: 初期化 成功:true 失敗:false									*
* Caution(s)	:																*
********************************************************************************/
bool
CTimerLib::Create(u_short nResolution)
{
	Debuglog((LPWSTR)(L"TimerCreate."));

	// OSのタイマー最大精度を求める
	TIMECAPS tc;
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
	{
		// 精度検出失敗
		MsgBox((LPWSTR)(L"Could not create Timer object!"));
		return FALSE;
	}
	m_uPeriod = tc.wPeriodMin;
	// タイマー最大精度の動作を要求
	if(timeBeginPeriod(m_uPeriod) != TIMERR_NOERROR)
	{
		// タイマー要求失敗
		MsgBox((LPWSTR)(L"Could not create Timer object!"));
		return FALSE;
	}
	// 分解能決定
	m_nTimeResolution = __max(m_uPeriod, nResolution);

	m_bCreate = TRUE;

	Debuglog((LPWSTR)(L"TimerCreateOK."));
	
	return TRUE;
}

/********************************************************************************
* Function Name	: Start															*
* Function		: タイマー動作開始												*
* Argument		: nFPS=希望するタイマー周波数（Hz）								*
*				  0を指定した場合，デフォルトのFPS(=E_FPS_DEFAULT)を用いる		*
* Return		: 開始 成功:true 失敗:false										*
* Caution(s)	:																*
********************************************************************************/
bool
CTimerLib::Start(u_short nFPS)
{
	Assert(m_bCreate);

	// FPSの値が0（引数省略時）は，デフォルトのＦＰＳとする．
	if(nFPS)
	{
		m_nFPS = nFPS;
	}
	else
	{
		m_nFPS = E_FPS_DEFAULT;
	}
	m_nNowTime = 0;				// 現在の時間mSec
	m_nFPS_Cnt = 1;				// 内部カウンタ
	m_nNextTime = 1000/m_nFPS;	// 次回タイマー到達時刻

	// タイマー起動
	m_uTimerID = timeSetEvent(m_nTimeResolution, 0, TimerProc, (DWORD_PTR)this, TIME_PERIODIC);

	return (m_uTimerID != NULL);
}

/********************************************************************************
* Function Name	: Kill															*
* Function		: タイマー動作停止												*
* Argument		: none															*
* Return		: 停止 成功:true 失敗:false										*
* Caution(s)	:																*
********************************************************************************/
bool
CTimerLib::Kill(void)
{
	Assert(m_bCreate);

	// タイマー停止中は処理なし
	if(m_uTimerID == NULL)
	{
		return TRUE;
	}

	// タイマーストップ
	if(timeKillEvent(m_uTimerID) != TIMERR_NOERROR)
	{
		// 停止失敗
		MsgBox((LPWSTR)(L"Could not kill Timer object!"));
		return FALSE;
	}

	m_uTimerID = NULL;
	return TRUE;
}

/********************************************************************************
* Function Name	: Cleanup														*
* Function		: タイマー解放処理												*
* Argument		: none															*
* Return		: none															*
* Caution(s)	:																*
********************************************************************************/
void
CTimerLib::Cleanup(void)
{
	Debuglog((LPWSTR)(L"TimerCleanup."));

	// 先にタイマー停止
	if(m_uTimerID != NULL)
	{
		Kill();
	}

	// タイマー解放
	if(m_bCreate)
	{
		timeEndPeriod(m_uPeriod);
	}

	m_bCreate = FALSE;

	return;
}

/********************************************************************************
* Function Name	: GetTimeStat													*
* Function		: タイマー状態取得												*
* Argument		: none															*
* Return		: TIMER_ISOVER=タイマー超過										*
*				  TIMER_ISJUST=タイマー到達										*
*				  TIMER_ISSOON=タイマー到達直前									*
*				  TIMER_ISIDLE=タイマー到達まで余裕あり							*
* Caution(s)	: 現在時刻と次回実行時刻の差から状態を判定						*
*				  １秒間の間にStart函数で指定した回数、TIMER_ISJUST/OVERを返す。*
*				  TIMER_ISJUST/OVERに合わせて処理を行う事でタイマーとして利用。	*
*				  TIMER_ISSOONが返った場合，タイマー到達が近いので、			*
*				  TIMER_ISJUST/OVERが返るまで繰返しこの函数を呼んで監視する事。	*
********************************************************************************/
long
CTimerLib::GetTimeStat(void)
{
	m_nWatchDog = 0;	// ウォッチドッグタイマークリア

	// タイマーアップ時刻 － タイマー現在時刻
	long n = m_nNowTime - m_nNextTime;

	if(n > m_nTimeResolution)
	{
		// タイマー超過
		m_nFPS_Cnt++;							// 内部カウンタ+1
		m_nNextTime = 1000*m_nFPS_Cnt/m_nFPS;	// 次回タイマー到達時刻 set
		return TIMER_ISOVER;
	}
	else if(n >= 0)
	{
		// タイマー到達
		m_nFPS_Cnt++;							// 内部カウンタ+1
		m_nNextTime = 1000*m_nFPS_Cnt/m_nFPS;	// 次回タイマー到達時刻 set
		return TIMER_ISJUST;
	}
	else if(n >= - m_nTimeResolution)
	{
		// タイマー到達間近
		return TIMER_ISSOON;
	}
	else
	{
		// タイマー到達まで余裕あり
		return TIMER_ISIDLE;
	}
}
