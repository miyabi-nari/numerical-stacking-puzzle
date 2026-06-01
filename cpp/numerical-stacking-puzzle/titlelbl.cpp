/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: titlelbl.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: タイトル用ラベルオブジェクト									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "titlelbl.h"
#include "midgui.h"
#include "midsnd.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT		( 0)	// 初期状態
#define	E_SEQ_IDLE		( 1)	// 待機状態

// GUI配置情報(構造体定義)
typedef struct T_TITLE_LBL_GUI_INF
{
	long nTxtPosX, nTxtPosY;	// 文字位置
	long nTxtWidX, nTxtWidY;	// 文字幅
	LPWSTR lpwsTxt;				// 文字列
	COLORREF crTxtColor;		// 文字色
	long nFontSizeY;			// 文字サイズ
}T_TITLE_LBL_GUI_INF;

// GUI配置情報(テーブル)
static const T_TITLE_LBL_GUI_INF F_TITLE_LBL_GUI_INF[E_TITLE_LBL_TYPE_NUM] =
{
// タイトルネーム項目
	 100, 100,			// 文字位置
	1400, 100,			// 文字幅
	(LPWSTR)E_APP_NAME_STR,	// 文字列
	RGB(  0,  0,  0),	// 文字色
	100,
// バージョン情報項目
	 100, 200,			// 文字位置
	1400,  50,			// 文字幅
	(LPWSTR)E_APP_VER_STR,	// 文字列
	RGB(  0,  0,  0),	// 文字色
	25,
// コピーライト項目
	 100, 225,			// 文字位置
	1400,  50,			// 文字幅
	(LPWSTR)E_APP_CR_STR,	// 文字列
	RGB(  0,  0,  0),	// 文字色
	25,
// ニューゲーム項目
	 100, 350,			// 文字位置
	1400,  50,			// 文字幅
	(LPWSTR)(L"New Game ... Please press the [Z] key."),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// 最近のゲーム項目
	 100, 450,			// 文字位置
	1400,  50,			// 文字幅
	(LPWSTR)(L"Recent Game ... Please press the [X] key."),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// サウンドオフ項目
	 100, 750,			// 文字位置
	1400,  50,			// 文字幅
	(LPWSTR)(L"Sound Off ... Please press the [C] key. (now, ON)"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// サウンドオン項目
	 100, 750,			// 文字位置
	1400,  50,			// 文字幅
	(LPWSTR)(L"Sound On ... Please press the [V] key. (now, OFF)"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
};

/********************************************************************************
*	CRTitleLbl Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRTitleLbl														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRTitleLbl::CRTitleLbl(void)
{
	// 引数付きで生成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRTitleLbl														*
* Function    :	コンストラクタ													*
* Argument(s) :	ラベル種類														*
********************************************************************************/
CRTitleLbl::CRTitleLbl(long nLblTypeID)
{
	//-----------------------------------------------------------------------
	// 引数チェック
	if(!NUMOK(nLblTypeID, E_TITLE_LBL_TYPE_NUM))
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT ] = &CRTitleLbl::SeqInit;		// 初期状態
	m_lpSeq[E_SEQ_IDLE ] = &CRTitleLbl::SeqIdle;		// 待機状態

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nLblTypeID = nLblTypeID;		// ラベル種類設定
	m_nGuiTxtID = -1;				// 初期GUIのテキストID=-1
	m_nSeq = E_SEQ_INIT;			// 初期シーケンス=初期状態

	//-----------------------------------------------------------------------
	// この時点でGUI表示の初期設定をする
	if(UpdateGui() == FALSE)
	{
		Assert(FALSE);
		return;
	}

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRTitleLbl"));
}

/********************************************************************************
* FunctionName:	~CRTitleLbl														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRTitleLbl::~CRTitleLbl(void)
{
	CRMidGui* lpMidGui;					// GUIインスタンス

	//-----------------------------------------------------------------------
	// GUIインスタンスを取得する
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui != nullptr)
	{
		if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) != FALSE)
		{
			// 描画するテキストを登録削除する
			lpMidGui->ClrTxt(m_nGuiTxtID);
			m_nGuiTxtID = -1;
		}
	}

	Debuglog((LPWSTR)(L"~CRTitleLbl"));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	UpdateGui														*
* Function    :	GUI表示の更新処理												*
* Argument(s) :	none															*
* Return      :	FALSE=更新失敗													*
********************************************************************************/
bool
CRTitleLbl::UpdateGui(void)
{
	CRMidGui* lpMidGui;					// GUIインスタンス
	CRMidSnd* lpMidSnd;					// 音インスタンス
	const T_TITLE_LBL_GUI_INF *lpGuiInf;	// GUI配置情報
	RECT rcPos;							// フレーム矩形(文字表示範囲矩形も兼用)
	CWMStr wmsTxt;						// 文字列

	//-----------------------------------------------------------------------
	// GUIインスタンスを取得する
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// GUI配置情報を特定する
	lpGuiInf = &F_TITLE_LBL_GUI_INF[m_nLblTypeID];

	//-----------------------------------------------------------------------
	// 文字表示
	// 文字表示範囲の矩形の設定
	SetRect(&rcPos,
	        lpGuiInf->nTxtPosX,
	        lpGuiInf->nTxtPosY,
	        lpGuiInf->nTxtPosX + lpGuiInf->nTxtWidX,
	        lpGuiInf->nTxtPosY + lpGuiInf->nTxtWidY);
	// 文字列の設定(音のON/OFF設定に応じて表示する文字列を切替)
	wmsTxt.SetW(lpGuiInf->lpwsTxt);
	if(m_nLblTypeID == E_TITLE_LBL_TYPE_SOUND_ON)
	{
		if(lpMidSnd->GetDisablePlay() != FALSE)
		{
			wmsTxt.SetW(nullptr);
		}
	}
	else if(m_nLblTypeID == E_TITLE_LBL_TYPE_SOUND_OFF)
	{
		if(lpMidSnd->GetDisablePlay() == FALSE)
		{
			wmsTxt.SetW(nullptr);
		}
	}
	// GUIのテキストID未取得時は取得する
	if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) == FALSE)
	{
		m_nGuiTxtID = lpMidGui->GetUnsetTxtID(E_MIDGUI_TXT_LBL);
	}
	// GUIドライバに文字列の配置を登録
	if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) != FALSE)
	{
		lpMidGui->SetTxt(m_nGuiTxtID,
		                &rcPos,
		                lpGuiInf->crTxtColor,
		                &wmsTxt,
		                lpGuiInf->nFontSizeY,
		                TRUE,
		                TRUE);
	}

	return TRUE;
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
CRTitleLbl::SeqInit(void)
{
	//-----------------------------------------------------------------------
	// GUI表示更新
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// シーケンス -> 待機状態
	m_nSeq = E_SEQ_IDLE;

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqIdle															*
* Function    :	シーケンス処理:待機状態											*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRTitleLbl::SeqIdle(void)
{
	//-----------------------------------------------------------------------
	// GUI表示更新
	if(UpdateGui() == FALSE)
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
* Attention(s):	呼び出し周期＝1/60sec											*
********************************************************************************/
bool
CRTitleLbl::Update(void)
{
	//-----------------------------------------------------------------------
	// 基底クラスの処理
	if(this->CRoutine::Update() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// シーケンス処理
	if((this->*m_lpSeq[m_nSeq])() == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}
