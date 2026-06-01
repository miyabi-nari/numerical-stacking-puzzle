/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: mainfrm.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 全体構造制御													*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "numerical-stacking-puzzle.h"
#include "define.h"
#include "mainfrm.h"
#include "midgui.h"
#include "midkey.h"
#include "midsnd.h"
#include "appli.h"
#include "wmstr.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT	( 0)	// 初期状態
#define	E_SEQ_MID	( 1)	// ミドルウェア起動中
#define	E_SEQ_APPLI	( 2)	// アプリケーション動作中

// 子プロセス
#define	E_RT_MID_KEY	( 0)	// Key入力のミドルウェアルーチン
#define	E_RT_MID_GUI	( 1)	// GUI描画のミドルウェアルーチン
#define	E_RT_MID_SND	( 2)	// 音再生のミドルウェアルーチン
#define	E_RT_APPLI		( 3)	// アプリケーションルーチン
#define	E_RT_NUM		(10)	//子プロセスの上限数

// タイマー
#define	E_TM_1STWAIT	(60)	// 初期安定化時間

/********************************************************************************
*	CRMainFrm Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRMainFrm														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRMainFrm::CRMainFrm(void)
{
	// 引数付きで作成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRMainFrm														*
* Function    :	コンストラクタ													*
* Argument(s) :	hInst=インスタンスハンドル										*
*				hWnd=ウィンドウハンドル											*
********************************************************************************/
CRMainFrm::CRMainFrm(HINSTANCE hInst, HWND hWnd)
{
	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT]  = &CRMainFrm::SeqInit;		// 初期状態
	m_lpSeq[E_SEQ_MID]   = &CRMainFrm::SeqMid;		// ミドルウェア起動中
	m_lpSeq[E_SEQ_APPLI] = &CRMainFrm::SeqAppli;	// アプリケーション動作中
	// 子プロセス
	if(SetChildNum(E_RT_NUM) == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// タイマーオブジェクト起動 分解能＝1ms, 60fps
	if(!m_cTimer.Create(1) || !m_cTimer.Start(60))
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期値設定
	m_hAppInst = hInst;			// インスタンスハンドル
	m_hAppWnd = hWnd;			// ウィンドウハンドル
	m_tm1stWait = E_TM_1STWAIT;	// 初期待機時間設定
	m_nSeq = E_SEQ_INIT;		// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRMainFrm"));
}

/********************************************************************************
* FunctionName:	~CRMainFrm														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRMainFrm::~CRMainFrm(void)
{
	Debuglog((LPWSTR)(L"~CRMainFrm"));
}

/********************************************************************************
*	Private Methods (Sequence)													*
********************************************************************************/

/********************************************************************************
* FunctionName:	SeqInit															*
* Function    :	シーケンス処理:初期状態											*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRMainFrm::SeqInit(void)
{
	// Key入力のミドルウェアルーチン起動
	NEW(m_lplpRtChild[E_RT_MID_KEY], CRMidKey(m_hAppWnd));
	if(m_lplpRtChild[E_RT_MID_KEY] == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	// GUI描画のミドルウェアルーチン起動
	NEW(m_lplpRtChild[E_RT_MID_GUI], CRMidGui(m_hAppWnd));
	if(m_lplpRtChild[E_RT_MID_GUI] == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	// 音再生のミドルウェアルーチン起動
	NEW(m_lplpRtChild[E_RT_MID_SND], CRMidSnd(m_hAppInst));
	if(m_lplpRtChild[E_RT_MID_SND] == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	// シーケンス -> ミドルウェア起動中
	m_nSeq = E_SEQ_MID;

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqMid															*
* Function    :	シーケンス処理:ミドルウェア起動中								*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRMainFrm::SeqMid(void)
{
	CRMidGui* lpMidGui;

	//-----------------------------------------------------------------------
	// Key入力のミドルウェアルーチン生成無し -> 強制終了
	// GUI描画のミドルウェアルーチン生成無し -> 強制終了
	// 音再生のミドルウェアルーチン生成無し -> 強制終了
	if(m_lplpRtChild[E_RT_MID_KEY] == nullptr ||
	   m_lplpRtChild[E_RT_MID_GUI] == nullptr ||
	   m_lplpRtChild[E_RT_MID_SND] == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// GUIインスタンスを取得する
	// 設定...60fps
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui != nullptr)
	{
		lpMidGui->SetFPS(1);
	}

	//-----------------------------------------------------------------------
	// アプリケーションルーチン起動
	NEW(m_lplpRtChild[E_RT_APPLI], CRAppli(m_hAppWnd));
	if(m_lplpRtChild[E_RT_APPLI] == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	// シーケンス -> アプリケーション動作中
	m_nSeq = E_SEQ_APPLI;

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqAppli														*
* Function    :	シーケンス処理:アプリケーション動作中							*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRMainFrm::SeqAppli(void)
{
	//-----------------------------------------------------------------------
	// Key入力のミドルウェアルーチン生成無し -> 強制終了
	// GUI描画のミドルウェアルーチン生成無し -> 強制終了
	// 音再生のミドルウェアルーチン生成無し -> 強制終了
	if(m_lplpRtChild[E_RT_MID_KEY] == nullptr ||
	   m_lplpRtChild[E_RT_MID_GUI] == nullptr ||
	   m_lplpRtChild[E_RT_MID_SND] == nullptr ||
	   m_lplpRtChild[E_RT_APPLI  ] == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	if(((CRAppli*)m_lplpRtChild[E_RT_APPLI])->IsExitSeq() != FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************
*	Public Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	Update															*
* Function    :	定期更新処理													*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRMainFrm::Update(void)
{
	bool bDisableDraw;			// 描画有無判定FG
	CRMidGui* lpMidGui;

	bDisableDraw = FALSE;

	//-----------------------------------------------------------------------
	// タイマー状態に応じて処理
	switch(m_cTimer.GetTimeStat())
	{
	case TIMER_ISOVER:	//タイマー時間経過超過 -> 描画無し処理
		bDisableDraw = TRUE;
		break;
	case TIMER_ISJUST:	//タイマー時間経過直後 -> 描画あり処理
		bDisableDraw = FALSE;
		break;
	case TIMER_ISSOON:	//タイマー経過直前 -> ここで待つ
		bDisableDraw = FALSE;
		while(m_cTimer.GetTimeStat() == TIMER_ISSOON);
		break;
	case TIMER_ISIDLE:	//余裕のある状態 -> 処理無し
	default:
		Sleep(1);
		return TRUE;	// 以降の処理無し
	}
	// ↓↓↓以降の部分は1/60sec周期で処理されます

	//-----------------------------------------------------------------------
	// 初期待機時間中はすべて描画処理有りとする
	if(m_tm1stWait)
	{
		// 初期待機時間中
		bDisableDraw = FALSE;
		// タイマーチェック＆タイマーリセット
		while(m_cTimer.GetTimeStat() == TIMER_ISOVER);
	}

	//-----------------------------------------------------------------------
	// 描画有無の更新
	// GUIインスタンスを取得する
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui != nullptr)
	{
		lpMidGui->DisableDraw(bDisableDraw);
	}

	//-----------------------------------------------------------------------
	// 基底クラスの処理
	if(this->CRoutine::Update() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマー
	Dec(m_tm1stWait);

	//-----------------------------------------------------------------------
	// シーケンス処理
	if((this->*m_lpSeq[m_nSeq])() == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	PassWndMsg														*
* Function    :	WindowMessageの伝達処理											*
* Argument(s) :	windowsメッセージとパラメータ									*
* Return      :	none															*
* Attention(s):	WindowMessageのコールバック関数から必要に応じてパスされる想定	*
********************************************************************************/
void
CRMainFrm::PassWndMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	CRMidKey*	lpMidKey;

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return;
	}

	switch (message)
	{
	case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// 選択されたメニューの解析:
			switch (wmId)
			{
            case IDM_EXIT:
            	if(m_lplpRtChild[E_RT_APPLI] != nullptr)
            	{
					((CRAppli*)m_lplpRtChild[E_RT_APPLI])->SetExit();
				}
                break;
			case IDM_TITLE:
            	if(m_lplpRtChild[E_RT_APPLI] != nullptr)
            	{
					((CRAppli*)m_lplpRtChild[E_RT_APPLI])->SetTitleBack();
				}
				break;
			}
		}
		break;
    case WM_CLOSE:
		((CRAppli*)m_lplpRtChild[E_RT_APPLI])->SetExit();
		break;
	case WM_LBUTTONDOWN:
		lpMidKey->SetMouseL_In(TRUE);
		break;
	case WM_LBUTTONUP:
		lpMidKey->SetMouseL_In(FALSE);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:
			lpMidKey->SetEscIn(TRUE);
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12:
			lpMidKey->SetFnIn(wParam - VK_F1, TRUE);
			break;
		case 0x5A:	// Z
			lpMidKey->SetQwertyIn(0, TRUE);
			break;
		case 0x58:	// X
			lpMidKey->SetQwertyIn(1, TRUE);
			break;
		case 0x43:	// C
			lpMidKey->SetQwertyIn(2, TRUE);
			break;
		case 0x56:	// V
			lpMidKey->SetQwertyIn(3, TRUE);
			break;
		case 0x42:	// B
			lpMidKey->SetQwertyIn(4, TRUE);
			break;
		case 0x4E:	// N
			lpMidKey->SetQwertyIn(5, TRUE);
			break;
		case 0x4D:	// M
			lpMidKey->SetQwertyIn(6, TRUE);
			break;
		case VK_OEM_COMMA:	// ,
			lpMidKey->SetQwertyIn(7, TRUE);
			break;
		case VK_OEM_PERIOD:	// .
			lpMidKey->SetQwertyIn(8, TRUE);
			break;
		case VK_OEM_2:	// /?
			lpMidKey->SetQwertyIn(9, TRUE);
			break;
		case VK_OEM_102:	// エスケープシーケンス
			lpMidKey->SetQwertyIn(10, TRUE);
			break;
		default:
			break;
		}
		break;
	case WM_KEYUP:
		switch(wParam)
		{
		case VK_ESCAPE:
			lpMidKey->SetEscIn(FALSE);
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
		case VK_F4:
		case VK_F5:
		case VK_F6:
		case VK_F7:
		case VK_F8:
		case VK_F9:
		case VK_F10:
		case VK_F11:
		case VK_F12:
			lpMidKey->SetFnIn(wParam - VK_F1, FALSE);
			break;
		case 0x5A:	// Z
			lpMidKey->SetQwertyIn(0, FALSE);
			break;
		case 0x58:	// X
			lpMidKey->SetQwertyIn(1, FALSE);
			break;
		case 0x43:	// C
			lpMidKey->SetQwertyIn(2, FALSE);
			break;
		case 0x56:	// V
			lpMidKey->SetQwertyIn(3, FALSE);
			break;
		case 0x42:	// B
			lpMidKey->SetQwertyIn(4, FALSE);
			break;
		case 0x4E:	// N
			lpMidKey->SetQwertyIn(5, FALSE);
			break;
		case 0x4D:	// M
			lpMidKey->SetQwertyIn(6, FALSE);
			break;
		case VK_OEM_COMMA:	// ,
			lpMidKey->SetQwertyIn(7, FALSE);
			break;
		case VK_OEM_PERIOD:	// .
			lpMidKey->SetQwertyIn(8, FALSE);
			break;
		case VK_OEM_2:	// /?
			lpMidKey->SetQwertyIn(9, FALSE);
			break;
		case VK_OEM_102:	// エスケープシーケンス
			lpMidKey->SetQwertyIn(10, FALSE);
			break;
		default:
			break;
		}
		break;
    case WM_ACTIVATE:
		lpMidKey->SetAllInOff();
		break;
	default:
		break;
	}

	return;
}
