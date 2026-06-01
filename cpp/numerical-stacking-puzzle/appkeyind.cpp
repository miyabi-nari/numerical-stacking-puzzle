/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appkeyind.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: キー表示オブジェクト											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "appkeyind.h"
#include "midgui.h"
#include "midkey.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT		( 0)	// 初期状態
#define	E_SEQ_IDLE		( 1)	// 待機状態

// GUI配置情報(構造体定義)
typedef struct T_APP_KEY_IND_GUI_INF
{
	long nFrmPosX, nFrmPosY;	// フレーム位置
	long nFrmWidX, nFrmWidY;	// フレーム幅
	long nTxtA_PosX, nTxtA_PosY;	// 文字A位置
	long nTxtA_WidX, nTxtA_WidY;	// 文字A幅
	long nTxtB_PosX, nTxtB_PosY;	// 文字B位置
	long nTxtB_WidX, nTxtB_WidY;	// 文字B幅
	LPWSTR lpwsTxtA;				// 文字列A
	LPWSTR lpwsTxtB;				// 文字列B
	COLORREF crFrmLineColor;	// フレームの線色
	COLORREF crFrmFilColorOff;	// フレームの塗潰し色(OFF時)
	COLORREF crFrmFilColorOn;	// フレームの塗潰し色(ON時)
	COLORREF crTxtColor;		// 文字色
	long nFontSizeY;			// 文字サイズ
}T_APP_KEY_IND_GUI_INF;

// GUI配置情報(テーブル)
static const T_APP_KEY_IND_GUI_INF F_APP_KEY_IND_GUI_INF[E_APP_KEY_IND_TYPE_NUM] =
{
// キー位置 0番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｚっ"),	// 文字列A
	(LPWSTR)(L"　つ"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 1番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｘ　"),	// 文字列A
	(LPWSTR)(L"　さ"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 2番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｃ　"),	// 文字列A
	(LPWSTR)(L"　そ"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 3番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｖ　"),	// 文字列A
	(LPWSTR)(L"　ひ"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 4番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｂ　"),	// 文字列A
	(LPWSTR)(L"　こ"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 5番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｎ　"),	// 文字列A
	(LPWSTR)(L"　み"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 6番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"Ｍ　"),	// 文字列A
	(LPWSTR)(L"　も"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 7番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"＜、"),	// 文字列A
	(LPWSTR)(L"，ね"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 8番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"＞。"),	// 文字列A
	(LPWSTR)(L"．る"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 9番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"？・"),	// 文字列A
	(LPWSTR)(L"／め"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
// キー位置 10番目
	  10,  10,			// フレーム位置
	  80,  80,			// フレーム幅
	  10,  10,			// 文字A位置
	  80,  40,			// 文字A幅
	  10,  50,			// 文字B位置
	  80,  40,			// 文字B幅
	(LPWSTR)(L"＿　"),	// 文字列A
	(LPWSTR)(L"＼ろ"),	// 文字列B
	RGB(  0,128,  0),	// フレームの線色
	RGB(160,192,128),	// フレームの塗潰し色(OFF時)
	RGB(240,192,128),	// フレームの塗潰し色(ON時)
	RGB(  0,  0,  0),	// 文字色
	36,					// 文字サイズ
};

/********************************************************************************
*	CRAppKeyInd Public Constructor(s)/Destructor								*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppKeyInd														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRAppKeyInd::CRAppKeyInd(void)
{
	// 引数付きで生成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRAppKeyInd														*
* Function    :	コンストラクタ													*
* Argument(s) :	nKeyIndTypeID=キー表示タイプ									*
********************************************************************************/
CRAppKeyInd::CRAppKeyInd(long nKeyIndTypeID, long nPosX, long nPosY)
{
	//-----------------------------------------------------------------------
	// 引数チェック
	if(!NUMOK(nKeyIndTypeID, E_APP_KEY_IND_TYPE_NUM))
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT  ] = &CRAppKeyInd::SeqInit;		// 初期状態
	m_lpSeq[E_SEQ_IDLE  ] = &CRAppKeyInd::SeqIdle;		// 待機状態

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nKeyIndTypeID = nKeyIndTypeID;		// キー表示タイプ設定
	m_nPosX = nPosX;						// 初期位置X
	m_nPosY = nPosY;						// 初期位置Y
	m_nStat = E_APP_KEY_IND_STAT_KEY_OFF;	// 初期キー表示状態=キーオフ表示
	m_nGuiRectID = -1;				// 初期GUIの矩形ID=-1
	m_nGuiTxtA_ID = -1;				// 初期GUIのテキストAのID=-1
	m_nGuiTxtB_ID = -1;				// 初期GUIのテキストBのID=-1
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

	Debuglog((LPWSTR)(L"CRAppKeyInd"));
}

/********************************************************************************
* FunctionName:	~CRAppKeyInd													*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppKeyInd::~CRAppKeyInd(void)
{
	CRMidGui* lpMidGui;					// GUIインスタンス

	//-----------------------------------------------------------------------
	// GUIインスタンスを取得する
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui != nullptr)
	{
		if(NUMOK(m_nGuiRectID, E_MIDGUI_RECT_NUM) != FALSE)
		{
			// 描画する矩形を登録削除する
			lpMidGui->ClrRect(m_nGuiRectID);
			m_nGuiRectID = -1;				
		}
		if(NUMOK(m_nGuiTxtA_ID, E_MIDGUI_TXT_NUM) != FALSE)
		{
			// 描画するテキストを登録削除する
			lpMidGui->ClrTxt(m_nGuiTxtA_ID);
			m_nGuiTxtA_ID = -1;
		}
		if(NUMOK(m_nGuiTxtB_ID, E_MIDGUI_TXT_NUM) != FALSE)
		{
			// 描画するテキストを登録削除する
			lpMidGui->ClrTxt(m_nGuiTxtB_ID);
			m_nGuiTxtB_ID = -1;
		}
	}

	Debuglog((LPWSTR)(L"~CRAppKeyInd"));
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
CRAppKeyInd::UpdateGui(void)
{
	CRMidGui* lpMidGui;					// GUIインスタンス
	CRMidKey* lpMidKey;					// Key入力インスタンス
	const T_APP_KEY_IND_GUI_INF *lpGuiInf;	// GUI配置情報
	long nPosX;							// 表示位置X
	long nPosY;							// 表示位置Y
	RECT rcPos;							// フレーム矩形(文字表示範囲矩形も兼用)
	CWMStr wmsTxt;						// 文字列
	COLORREF crFrmFilColor;				// フレームの塗潰し色

	//-----------------------------------------------------------------------
	// GUIインスタンスを取得する
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// GUI配置情報を特定する
	lpGuiInf = &F_APP_KEY_IND_GUI_INF[m_nKeyIndTypeID];

	//-----------------------------------------------------------------------
	// 基準位置設定
	nPosX = m_nPosX;
	nPosY = m_nPosY;

	//-----------------------------------------------------------------------
	// キー入力分岐
	if(lpMidKey->GetKeyOn()->qwerty[m_nKeyIndTypeID] == FALSE)
	{
		crFrmFilColor = lpGuiInf->crFrmFilColorOff;
	}
	else
	{
		crFrmFilColor = lpGuiInf->crFrmFilColorOn;
	}

	//-----------------------------------------------------------------------
	// フレーム表示
	// フレーム矩形の設定
	SetRect(&rcPos,
	        nPosX + lpGuiInf->nFrmPosX,
	        nPosY + lpGuiInf->nFrmPosY,
	        nPosX + lpGuiInf->nFrmPosX + lpGuiInf->nFrmWidX,
	        nPosY + lpGuiInf->nFrmPosY + lpGuiInf->nFrmWidY);
	// GUIの矩形ID未取得時は取得する
	if(NUMOK(m_nGuiRectID, E_MIDGUI_RECT_NUM) == FALSE)
	{
		m_nGuiRectID = lpMidGui->GetUnsetRectID(E_MIDGUI_RECT_KEY_IND);
	}
	// GUIドライバに矩形の配置を指示する
	if(NUMOK(m_nGuiRectID, E_MIDGUI_RECT_NUM) != FALSE)
	{
		lpMidGui->SetRect(m_nGuiRectID,
		                 &rcPos,
		                 lpGuiInf->crFrmLineColor,
		                 crFrmFilColor,
		                 TRUE);
	}

	//-----------------------------------------------------------------------
	// 文字表示A
	// 文字表示範囲の矩形の設定
	SetRect(&rcPos,
	        nPosX + lpGuiInf->nTxtA_PosX,
	        nPosY + lpGuiInf->nTxtA_PosY,
	        nPosX + lpGuiInf->nTxtA_PosX + lpGuiInf->nTxtA_WidX,
	        nPosY + lpGuiInf->nTxtA_PosY + lpGuiInf->nTxtA_WidY);
	// 文字列の設定
	wmsTxt.SetW(lpGuiInf->lpwsTxtA);
	// GUIのテキストID未取得時は取得する
	if(NUMOK(m_nGuiTxtA_ID, E_MIDGUI_TXT_NUM) == FALSE)
	{
		m_nGuiTxtA_ID = lpMidGui->GetUnsetTxtID(E_MIDGUI_TXT_KEY_IND);
	}
	// GUIドライバに文字列の配置を指示する
	if(NUMOK(m_nGuiTxtA_ID, E_MIDGUI_TXT_NUM) != FALSE)
	{
		lpMidGui->SetTxt(m_nGuiTxtA_ID,
		                &rcPos,
		                lpGuiInf->crTxtColor,
		                &wmsTxt,
		                lpGuiInf->nFontSizeY,
		                FALSE,
		                TRUE);
	}

	//-----------------------------------------------------------------------
	// 文字表示B
	// 文字表示範囲の矩形の設定
	SetRect(&rcPos,
	        nPosX + lpGuiInf->nTxtB_PosX,
	        nPosY + lpGuiInf->nTxtB_PosY,
	        nPosX + lpGuiInf->nTxtB_PosX + lpGuiInf->nTxtB_WidX,
	        nPosY + lpGuiInf->nTxtB_PosY + lpGuiInf->nTxtB_WidY);
	// 文字列の設定
	wmsTxt.SetW(lpGuiInf->lpwsTxtB);
	// GUIのテキストID未取得時は取得する
	if(NUMOK(m_nGuiTxtB_ID, E_MIDGUI_TXT_NUM) == FALSE)
	{
		m_nGuiTxtB_ID = lpMidGui->GetUnsetTxtID(E_MIDGUI_TXT_KEY_IND);
	}
	// GUIドライバに文字列の配置を指示する
	if(NUMOK(m_nGuiTxtB_ID, E_MIDGUI_TXT_NUM) != FALSE)
	{
		lpMidGui->SetTxt(m_nGuiTxtB_ID,
		                &rcPos,
		                lpGuiInf->crTxtColor,
		                &wmsTxt,
		                lpGuiInf->nFontSizeY,
		                FALSE,
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
CRAppKeyInd::SeqInit(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

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
CRAppKeyInd::SeqIdle(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
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
CRAppKeyInd::Update(void)
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

/********************************************************************************
* FunctionName:	SetPos															*
* Function    :	位置を設定														*
* Argument    :	位置X,Y															*
* Return      :	none															*
********************************************************************************/
void
CRAppKeyInd::SetPos(long nPosX, long nPosY)
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

/********************************************************************************
* FunctionName:	GetStat															*
* Function    :	キー表示状態を取得												*
* Argument    :	none															*
* Return      :	キー表示状態													*
********************************************************************************/
long
CRAppKeyInd::GetStat(void)
{
	return m_nStat;
};
