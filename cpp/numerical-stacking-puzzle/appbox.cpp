/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appbox.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 箱オブジェクト												*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "appbox.h"
#include "midgui.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT		( 0)	// 初期状態
#define	E_SEQ_IDLE		( 1)	// 待機状態
#define	E_SEQ_SLIDE		( 2)	// スライド移動状態
#define	E_SEQ_CHANGE	( 3)	// 種類の変更状態
#define	E_SEQ_FALL		( 4)	// 落下状態
#define	E_SEQ_SERVE		( 5)	// サーブ状態

// タイマー
#define	E_TM_SLIDE		(12)	// スライド移動状態の処理時間
#define	E_TM_CHANGE		(12)	// 種類の変更状態の処理時間
#define	E_TM_FALL		(12)	// 落下状態の処理時間
#define	E_TM_SERVE		(12)	// サーブ移動状態の処理時間

// GUI配置情報(構造体定義)
typedef struct T_APP_BOX_GUI_INF
{
	long nFrmPosX, nFrmPosY;	// フレーム位置
	long nFrmWidX, nFrmWidY;	// フレーム幅
	long nTxtPosX, nTxtPosY;	// 文字位置
	long nTxtWidX, nTxtWidY;	// 文字幅
	LPWSTR lpwsTxt;				// 文字列
	COLORREF crFrmLineColor;	// フレームの線色
	COLORREF crFrmFilColor;		// フレームの塗潰し色
	COLORREF crTxtColor;		// 文字色
	long nFontSizeY;			// 文字サイズ
}T_APP_BOX_GUI_INF;

// GUI配置情報(テーブル)
static const T_APP_BOX_GUI_INF F_APP_BOX_GUI_INF[E_APP_BOX_TYPE_NUM] =
{
// 2の0乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"1"),		// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(128,128,128),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	64,					// 文字サイズ
// 2の1乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"2"),		// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 64, 64,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	64,					// 文字サイズ
// 2の2乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"4"),		// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192, 64, 64),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	64,					// 文字サイズ
// 2の3乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"8"),		// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192, 64,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	64,					// 文字サイズ
// 2の4乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"16"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 64,192, 64),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	48,					// 文字サイズ
// 2の5乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"32"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 64,192,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	48,					// 文字サイズ
// 2の6乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"64"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192,192, 64),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	48,					// 文字サイズ
// 2の7乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"128"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192,192,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	40,					// 文字サイズ
// 2の8乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"256"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192,192,255),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	40,					// 文字サイズ
// 2の9乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"512"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(255,192,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	40,					// 文字サイズ
// 2の10乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"1,024"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(255,192,255),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	32,					// 文字サイズ
// 2の11乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"2,048"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192,255,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	32,					// 文字サイズ
// 2の12乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"4,096"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(192,255,255),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	32,					// 文字サイズ
// 2の13乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"8,192"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(255,255,192),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	32,					// 文字サイズ
// 2の14乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"16,384"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(255,255,255),	// フレームの塗潰し色
	RGB(  0,  0,  0),	// 文字色
	28,					// 文字サイズ
// 2の15乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"32,768"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 32, 32, 32),	// フレームの塗潰し色
	RGB(255,255,255),	// 文字色
	28,					// 文字サイズ
// 2の16乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"65,536"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 32, 32,128),	// フレームの塗潰し色
	RGB(255,255,255),	// 文字色
	28,					// 文字サイズ
// 2の17乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"131,072"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(128, 32, 32),	// フレームの塗潰し色
	RGB(255,255,255),	// 文字色
	22,					// 文字サイズ
// 2の18乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"262,144"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB(128, 32,128),	// フレームの塗潰し色
	RGB(255,255,255),	// 文字色
	22,					// 文字サイズ
// 2の19乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"524,288"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 32,128, 32),	// フレームの塗潰し色
	RGB(255,255,255),	// 文字色
	22,					// 文字サイズ
// 2の20乗
	   0,   0,			// フレーム位置
	 100, 100,			// フレーム幅
	   0,   0,			// 文字位置
	 100, 100,			// 文字幅
	(LPWSTR)(L"1,048,576"),	// 文字列
	RGB( 64, 64, 64),	// フレームの線色
	RGB( 32,128,128),	// フレームの塗潰し色
	RGB(255,255,255),	// 文字色
	20,					// 文字サイズ
};

#define	E_APP_BOX_GUI_OFFSET_X	(  0)	// 箱オブジェクト全体のオフセット位置X
#define	E_APP_BOX_GUI_OFFSET_Y	(100)	// 箱オブジェクト全体のオフセット位置Y

/********************************************************************************
*	CRAppBox Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppBox														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRAppBox::CRAppBox(void)
{
	// 引数付きで生成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRAppBox														*
* Function    :	コンストラクタ													*
* Argument(s) :	nBoxTypeID=箱タイプ												*
********************************************************************************/
CRAppBox::CRAppBox(long nBoxTypeID, long nPosX, long nPosY)
{
	//-----------------------------------------------------------------------
	// 引数チェック
	if(!NUMOK(nBoxTypeID, E_APP_BOX_TYPE_NUM))
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT  ] = &CRAppBox::SeqInit;		// 初期状態
	m_lpSeq[E_SEQ_IDLE  ] = &CRAppBox::SeqIdle;		// 待機状態
	m_lpSeq[E_SEQ_SLIDE ] = &CRAppBox::SeqSlide;	// スライド移動状態
	m_lpSeq[E_SEQ_CHANGE] = &CRAppBox::SeqChange;	// 種類の変更状態
	m_lpSeq[E_SEQ_FALL  ] = &CRAppBox::SeqFall;		// 落下状態
	m_lpSeq[E_SEQ_SERVE ] = &CRAppBox::SeqServe;	// サーブ状態

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nBoxTypeID = nBoxTypeID;		// 箱タイプ設定
	m_nPosX = nPosX;				// 初期位置X
	m_nPosY = nPosY;				// 初期位置Y
	m_nStat = E_APP_BOX_STAT_IDLE;	// 初期箱状態=オフ
	m_nGuiRectID = -1;				// 初期GUIの矩形ID=-1
	m_nGuiTxtID = -1;				// 初期GUIのテキストID=-1
	m_tmSeq = 0;					// シーケンスタイマー=0
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

	Debuglog((LPWSTR)(L"CRAppBox"));
}

/********************************************************************************
* FunctionName:	~CRAppBox														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppBox::~CRAppBox(void)
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
		if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) != FALSE)
		{
			// 描画するテキストを登録削除する
			lpMidGui->ClrTxt(m_nGuiTxtID);
			m_nGuiTxtID = -1;
		}
	}

	Debuglog((LPWSTR)(L"~CRAppBox"));
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
CRAppBox::UpdateGui(void)
{
	CRMidGui* lpMidGui;					// GUIインスタンス
	const T_APP_BOX_GUI_INF *lpGuiInf;	// GUI配置情報
	long nRate;							// 変形レート
	long nPosX;							// 表示位置X
	long nPosY;							// 表示位置Y
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
	// GUI配置情報を特定する
	lpGuiInf = &F_APP_BOX_GUI_INF[m_nBoxTypeID];

	//-----------------------------------------------------------------------
	// シーケンス分岐(場合によっては位置移動させる)
	if(m_nSeq == E_SEQ_SLIDE)
	{
		// シーケンス=スライド移動状態 -> 位置を変形する
		nRate = m_tmSeq * m_tmSeq;
		nPosX = m_nPosX - (m_nPosX - m_nSlideFromX) * nRate / (E_TM_SLIDE * E_TM_SLIDE);
		nPosY = m_nPosY - (m_nPosY - m_nSlideFromY) * nRate / (E_TM_SLIDE * E_TM_SLIDE);
	}
	else if(m_nSeq == E_SEQ_FALL)
	{
		// シーケンス=落下移動状態 -> 位置を変形する
		nRate = (E_TM_FALL * E_TM_FALL) - (E_TM_FALL - m_tmSeq) * (E_TM_FALL - m_tmSeq);
		nPosX = m_nPosX - (m_nPosX - m_nFallFromX) * nRate / (E_TM_FALL * E_TM_FALL);
		nPosY = m_nPosY - (m_nPosY - m_nFallFromY) * nRate / (E_TM_FALL * E_TM_FALL);
	}
	else if(m_nSeq == E_SEQ_SERVE)
	{
		// シーケンス=サーブ移動状態 -> 位置を変形する
		nRate = m_tmSeq;
		nPosX = m_nPosX - (m_nPosX - m_nServeFromX) * nRate / E_TM_SERVE;
		nPosY = m_nPosY - (m_nPosY - m_nServeFromY) * nRate / E_TM_SERVE;
	}
	else{
		// シーケンス=その他 -> 変形なし
		nPosX = m_nPosX;
		nPosY = m_nPosY;
	}
	nPosX += E_APP_BOX_GUI_OFFSET_X;
	nPosY += E_APP_BOX_GUI_OFFSET_Y;

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
		m_nGuiRectID = lpMidGui->GetUnsetRectID(E_MIDGUI_RECT_BOX);
	}
	// GUIドライバに矩形の配置を指示する
	if(NUMOK(m_nGuiRectID, E_MIDGUI_RECT_NUM) != FALSE)
	{
		lpMidGui->SetRect(m_nGuiRectID,
		                 &rcPos,
		                 lpGuiInf->crFrmLineColor,
		                 lpGuiInf->crFrmFilColor,
		                 TRUE);
	}

	//-----------------------------------------------------------------------
	// 文字表示
	// 文字表示範囲の矩形の設定
	SetRect(&rcPos,
	        nPosX + lpGuiInf->nTxtPosX,
	        nPosY + lpGuiInf->nTxtPosY,
	        nPosX + lpGuiInf->nTxtPosX + lpGuiInf->nTxtWidX,
	        nPosY + lpGuiInf->nTxtPosY + lpGuiInf->nTxtWidY);
	// 文字列の設定
	wmsTxt.SetW(lpGuiInf->lpwsTxt);
	// GUIのテキストID未取得時は取得する
	if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) == FALSE)
	{
		m_nGuiTxtID = lpMidGui->GetUnsetTxtID(E_MIDGUI_TXT_BOX);
	}
	// GUIドライバに文字列の配置を指示する
	if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) != FALSE)
	{
		lpMidGui->SetTxt(m_nGuiTxtID,
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
CRAppBox::SeqInit(void)
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
CRAppBox::SeqIdle(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 箱状態がスライド移動の場合はスライド移動状態へ
	if(m_nStat == E_APP_BOX_STAT_SLIDE)
	{
		// 位置 <- スライド移動終了位置
		m_nPosX = m_nSlideToX;
		m_nPosY = m_nSlideToY;
		// シーケンス -> スライド移動状態
		m_nSeq = E_SEQ_SLIDE;
		m_tmSeq = E_TM_SLIDE;
	}

	//-----------------------------------------------------------------------
	// 箱状態が種類変更の場合は種類の変更状態へ
	if(m_nStat == E_APP_BOX_STAT_CHANGE)
	{
		// 変更後箱タイプ設定 <- 引数
		m_nBoxTypeID = m_nChangeToBoxTypeID;
		// シーケンス -> 種類の変更状態
		m_nSeq = E_SEQ_CHANGE;
		m_tmSeq = E_TM_CHANGE;
	}

	//-----------------------------------------------------------------------
	// 箱状態が落下の場合は落下状態へ
	if(m_nStat == E_APP_BOX_STAT_FALL)
	{
		// 位置 <- 落下移動終了位置
		m_nPosX = m_nFallToX;
		m_nPosY = m_nFallToY;
		// シーケンス -> 落下状態
		m_nSeq = E_SEQ_FALL;
		m_tmSeq = E_TM_FALL;
	}

	//-----------------------------------------------------------------------
	// 箱状態がサーブの場合はサーブ状態へ
	if(m_nStat == E_APP_BOX_STAT_SERVE)
	{
		// 位置 <- サーブ移動終了位置
		m_nPosX = m_nServeToX;
		m_nPosY = m_nServeToY;
		// シーケンス -> サーブ状態
		m_nSeq = E_SEQ_SERVE;
		m_tmSeq = E_TM_SERVE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqSlide														*
* Function    :	シーケンス処理:スライド移動状態									*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRAppBox::SeqSlide(void)
{
	long nRet;

	nRet = TRUE;

	//-----------------------------------------------------------------------
	// GUI表示の更新処理
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(スライド移動中)
	if(m_tmSeq == 0)
	{
		// タイマーカウント後
		return FALSE;
	}

	return nRet;
}

/********************************************************************************
* FunctionName:	SeqChange														*
* Function    :	シーケンス処理:種類の変更状態									*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRAppBox::SeqChange(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(種類の変更中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		// タイマーカウント後
		// 箱状態=アイドル
		m_nStat = E_APP_BOX_STAT_IDLE;
		// シーケンス -> 待機状態
		m_nSeq = E_SEQ_IDLE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqFall															*
* Function    :	シーケンス処理:落下状態											*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRAppBox::SeqFall(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(表示更新中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		// タイマーカウント後
		// 箱状態=アイドル
		m_nStat = E_APP_BOX_STAT_IDLE;
		// シーケンス -> 待機状態
		m_nSeq = E_SEQ_IDLE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqServe															*
* Function    :	シーケンス処理:サーブ状態											*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRAppBox::SeqServe(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
	if(UpdateGui() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(表示更新中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		// タイマーカウント後
		// 箱状態=アイドル
		m_nStat = E_APP_BOX_STAT_IDLE;
		// シーケンス -> 待機状態
		m_nSeq = E_SEQ_IDLE;
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
CRAppBox::Update(void)
{
	//-----------------------------------------------------------------------
	// 基底クラスの処理
	if(this->CRoutine::Update() == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマー
	Dec(m_tmSeq);

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
CRAppBox::SetPos(long nPosX, long nPosY)
{
	m_nPosX = nPosX;
	m_nPosY = nPosY;
}

/********************************************************************************
* FunctionName:	SetStatSlide													*
* Function    :	箱状態をスライド移動に設定										*
* Argument    :	スライド終了位置												*
* Return      :	none															*
********************************************************************************/
void
CRAppBox::SetStatSlide(long nSlideToX, long nSlideToY)
{
	// スライド移動開始位置 <- 現在位置
	m_nSlideFromX = m_nPosX;
	m_nSlideFromY = m_nPosY;
	// スライド移動終了位置 <- 引数
	m_nSlideToX = nSlideToX;
	m_nSlideToY = nSlideToY;

	m_nStat = E_APP_BOX_STAT_SLIDE;
}

/********************************************************************************
* FunctionName:	SetStatChange													*
* Function    :	箱状態を種類変更に設定											*
* Argument    :	変更後箱タイプ													*
* Return      :	none															*
********************************************************************************/
void
CRAppBox::SetStatChange(long nChangeToBoxTypeID)
{
	// 変更前箱タイプ <- 箱タイプ設定
	m_nChangeFromBoxTypeID = m_nBoxTypeID;
	// 変更後箱タイプ設定 <- 引数
	m_nChangeToBoxTypeID = nChangeToBoxTypeID;

	m_nStat = E_APP_BOX_STAT_CHANGE;
}

/********************************************************************************
* FunctionName:	SetStatFall														*
* Function    :	箱状態を落下に設定												*
* Argument    :	落下移動終了位置												*
* Return      :	none															*
********************************************************************************/
void
CRAppBox::SetStatFall(long nFallToX, long nFallToY)
{
	// 落下移動開始位置 <- 現在位置
	m_nFallFromX = m_nPosX;
	m_nFallFromY = m_nPosY;
	// 落下移動終了位置 <- 引数
	m_nFallToX = nFallToX;
	m_nFallToY = nFallToY;

	m_nStat = E_APP_BOX_STAT_FALL;
}

/********************************************************************************
* FunctionName:	SetStatServe													*
* Function    :	箱状態をサーブに設定											*
* Argument    :	サーブ移動終了位置												*
* Return      :	none															*
********************************************************************************/
void
CRAppBox::SetStatServe(long nServeToX, long nServeToY)
{
	// サーブ移動開始位置 <- 現在位置
	m_nServeFromX = m_nPosX;
	m_nServeFromY = m_nPosY;
	// サーブ移動終了位置 <- 引数
	m_nServeToX = nServeToX;
	m_nServeToY = nServeToY;

	m_nStat = E_APP_BOX_STAT_SERVE;
}

/********************************************************************************
* FunctionName:	GetStat															*
* Function    :	箱状態を取得													*
* Argument    :	none															*
* Return      :	箱状態															*
********************************************************************************/
long
CRAppBox::GetStat(void)
{
	return m_nStat;
};
