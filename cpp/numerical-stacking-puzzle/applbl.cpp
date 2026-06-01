/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: applbl.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ラベルオブジェクト											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "applbl.h"
#include "midgui.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT		( 0)	// 初期状態
#define	E_SEQ_IDLE		( 1)	// 待機状態

// GUI配置情報(構造体定義)
typedef struct T_APP_LBL_GUI_INF
{
	long nTxtPosX, nTxtPosY;	// 文字位置
	long nTxtWidX, nTxtWidY;	// 文字幅
	LPWSTR lpwsTxt;				// 文字列
	COLORREF crTxtColor;		// 文字色
	long nFontSizeY;			// 文字サイズ
}T_APP_LBL_GUI_INF;

// GUI配置情報(テーブル)
static const T_APP_LBL_GUI_INF F_APP_LBL_GUI_INF[E_APP_LBL_TYPE_NUM] =
{
// 次落下箱案内 ラベル
	1200,  40,			// 文字位置
	 400,  60,			// 文字幅
	(LPWSTR)(L"Next Block"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// スコア項目 ラベル
	1200, 220,			// 文字位置
	 400,  60,			// 文字幅
	(LPWSTR)(L"Score"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// 経過時間項目 ラベル
	1200, 400,			// 文字位置
	 400,  60,			// 文字幅
	(LPWSTR)(L"Elapsed-Time"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// マージ時間項目 ラベル
	1200, 580,			// 文字位置
	 400,  60,			// 文字幅
	(LPWSTR)(L"Merged-Time"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// スコア値 ラベル
	1220, 280,			// 文字位置
	 380,  60,			// 文字幅
	(LPWSTR)(L"0"),		// 文字列
	RGB(128,128,128),	// 文字色
	50,
// 経過時間値 ラベル
	1220, 460,			// 文字位置
	 380,  60,			// 文字幅
	(LPWSTR)(L"--:--:--.--"),	// 文字列
	RGB(128,128,128),	// 文字色
	50,
// マージ時間値1 ラベル
	1220, 650,			// 文字位置
	 380,  40,			// 文字幅
	(LPWSTR)(L"-"),		// 文字列
	RGB(128,128,128),	// 文字色
	30,
// マージ時間値2 ラベル
	1220, 690,			// 文字位置
	 380,  40,			// 文字幅
	(LPWSTR)(L"-"),		// 文字列
	RGB(128,128,128),	// 文字色
	30,
// マージ時間値3 ラベル
	1220, 730,			// 文字位置
	 380,  40,			// 文字幅
	(LPWSTR)(L"-"),		// 文字列
	RGB(128,128,128),	// 文字色
	30,
// マージ時間値4 ラベル
	1220, 770,			// 文字位置
	 380,  40,			// 文字幅
	(LPWSTR)(L"-"),		// 文字列
	RGB(128,128,128),	// 文字色
	30,
// マージ時間値5 ラベル
	1220, 810,			// 文字位置
	 380,  40,			// 文字幅
	(LPWSTR)(L"-"),		// 文字列
	RGB(128,128,128),	// 文字色
	30,
};

static const LPWSTR F_APP_LBL_GUI_INF_MERGED_TIME_VAL[21] =
{
// 2の0乗
	(LPWSTR)(L"1..."),		// 文字列
// 2の1乗
	(LPWSTR)(L"2..."),		// 文字列
// 2の2乗
	(LPWSTR)(L"4..."),		// 文字列
// 2の3乗
	(LPWSTR)(L"8..."),		// 文字列
// 2の4乗
	(LPWSTR)(L"16..."),		// 文字列
// 2の5乗
	(LPWSTR)(L"32..."),		// 文字列
// 2の6乗
	(LPWSTR)(L"64..."),		// 文字列
// 2の7乗
	(LPWSTR)(L"128..."),	// 文字列
// 2の8乗
	(LPWSTR)(L"256..."),	// 文字列
// 2の9乗
	(LPWSTR)(L"512..."),	// 文字列
// 2の10乗
	(LPWSTR)(L"1,024..."),	// 文字列
// 2の11乗
	(LPWSTR)(L"2,048..."),	// 文字列
// 2の12乗
	(LPWSTR)(L"4,096..."),	// 文字列
// 2の13乗
	(LPWSTR)(L"8,192..."),	// 文字列
// 2の14乗
	(LPWSTR)(L"16,384..."),	// 文字列
// 2の15乗
	(LPWSTR)(L"32,768..."),	// 文字列
// 2の16乗
	(LPWSTR)(L"65,536..."),	// 文字列
// 2の17乗
	(LPWSTR)(L"131,072..."),	// 文字列
// 2の18乗
	(LPWSTR)(L"262,144..."),	// 文字列
// 2の19乗
	(LPWSTR)(L"524,288..."),	// 文字列
// 2の20乗
	(LPWSTR)(L"1,048,576..."),	// 文字列
};

/********************************************************************************
*	CRAppLbl Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppLbl														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRAppLbl::CRAppLbl(void)
{
	// 引数付きで生成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRAppLbl														*
* Function    :	コンストラクタ													*
* Argument(s) :	nLblTypeID=ラベルタイプ											*
********************************************************************************/
CRAppLbl::CRAppLbl(long nLblTypeID)
{
	//-----------------------------------------------------------------------
	// 引数チェック
	if(!NUMOK(nLblTypeID, E_APP_LBL_TYPE_NUM))
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT  ] = &CRAppLbl::SeqInit;		// 初期状態
	m_lpSeq[E_SEQ_IDLE  ] = &CRAppLbl::SeqIdle;		// 待機状態

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nLblTypeID = nLblTypeID;		// ラベルタイプ設定
	m_nVal1 = -1;					// 表示値=-1
	m_nVal2 = -1;					// 表示値=-1
	m_nValOld = -1;
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

	Debuglog((LPWSTR)(L"CRAppLbl"));
}

/********************************************************************************
* FunctionName:	~CRAppLbl														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppLbl::~CRAppLbl(void)
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

	Debuglog((LPWSTR)(L"~CRAppLbl"));
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
CRAppLbl::UpdateGui(void)
{
	CRMidGui* lpMidGui;					// GUIインスタンス
	const T_APP_LBL_GUI_INF *lpGuiInf;	// GUI配置情報
	RECT rcPos;							// 文字表示範囲矩形
	CWMStr wmsTxt;						// 文字列
	long nVal;

	//-----------------------------------------------------------------------
	// GUIインスタンスを取得する
	lpMidGui = CRMidGui::GetInstPt();
	if(lpMidGui == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// GUI配置情報を特定する
	lpGuiInf = &F_APP_LBL_GUI_INF[m_nLblTypeID];

	//-----------------------------------------------------------------------
	// 文字表示
	// 文字表示範囲の矩形の設定
	SetRect(&rcPos,
	        lpGuiInf->nTxtPosX,
	        lpGuiInf->nTxtPosY,
	        lpGuiInf->nTxtPosX + lpGuiInf->nTxtWidX,
	        lpGuiInf->nTxtPosY + lpGuiInf->nTxtWidY);
	// 文字列の設定
	wmsTxt.SetW(lpGuiInf->lpwsTxt);
	// 文字列に表示値を追記する
	if(m_nLblTypeID == E_APP_LBL_TYPE_SCORE_VAL)
	{
		// ラベル種類: スコア値
		if(m_nVal1 >= 0)
		{
			if(m_nValOld < m_nVal1)
			{
				if(m_nValOld >= 0)
				{
					m_nValOld += ((m_nVal1 - m_nValOld) + 99) / 100;
				}
				else
				{
					m_nValOld = 0;
				}
			}
			else if(m_nValOld > m_nVal1)
			{
				m_nValOld -= ((m_nValOld - m_nVal1) + 99) / 100;
			}
			nVal = m_nValOld;
			wmsTxt.SetW((LPWSTR)L"");
			if(nVal >= 1000*1000*1000)
			{
				wmsTxt.AddDigW((nVal)/(1000*1000*1000), -1);
				wmsTxt.AddW((LPWSTR)L",");
				nVal %= (1000 * 1000 * 1000);
				wmsTxt.AddDigW((nVal)/(1000*1000), 3);
				wmsTxt.AddW((LPWSTR)L",");
				nVal %= (1000 * 1000);
				wmsTxt.AddDigW((nVal)/1000, 3);
				wmsTxt.AddW((LPWSTR)L",");
				nVal %= 1000;
				wmsTxt.AddDigW((nVal)/1, 3);
			}
			else if(nVal >= 1000*1000)
			{
				wmsTxt.AddDigW((nVal)/(1000 * 1000), -1);
				wmsTxt.AddW((LPWSTR)L",");
				nVal %= (1000 * 1000);
				wmsTxt.AddDigW((nVal)/1000, 3);
				wmsTxt.AddW((LPWSTR)L",");
				nVal %= 1000;
				wmsTxt.AddDigW((nVal)/1, 3);
			}
			else if(nVal >= 1000)
			{
				wmsTxt.AddDigW((nVal)/1000, -1);
				wmsTxt.AddW((LPWSTR)L",");
				nVal %= 1000;
				wmsTxt.AddDigW((nVal)/1, 3);
			}
			else if(nVal >= 0)
			{
				wmsTxt.AddDigW((nVal)/1, -1);
			}
			else
			{
				wmsTxt.AddW((LPWSTR)L"-");
			}
		}
		else
		{
			wmsTxt.SetW((LPWSTR)L"-");
		}
	}
	else if(m_nLblTypeID == E_APP_LBL_TYPE_ELAPSED_TIME_VAL)
	{
		// ラベル種類: 経過時間値
		nVal = m_nVal1;
		if(nVal >= 0)
		{
			wmsTxt.SetW((LPWSTR)L"");
			wmsTxt.AddDigW((nVal)/(60*60*60), -1);
			wmsTxt.AddW((LPWSTR)L":");
			nVal %= (60 * 60 * 60);
			wmsTxt.AddDigW((nVal)/(60*60), 2);
			wmsTxt.AddW((LPWSTR)L":");
			nVal %= (60 * 60);
			wmsTxt.AddDigW((nVal)/60, 2);
			wmsTxt.AddW((LPWSTR)L".");
			nVal %= 60;
			wmsTxt.AddDigW((nVal)*100/60, 2);
		}
		else
		{
			wmsTxt.SetW((LPWSTR)L"--:--:--.--");
		}
	}
	else if(m_nLblTypeID >= E_APP_LBL_TYPE_MERGED_TIME_VAL_1 &&
	        m_nLblTypeID <= E_APP_LBL_TYPE_MERGED_TIME_VAL_5)
	{
		// ラベル種類: マージ時間値
		nVal = m_nVal2;
		if(nVal > 0)
		{
			wmsTxt.SetW(F_APP_LBL_GUI_INF_MERGED_TIME_VAL[nVal]);
			nVal = m_nVal1;
			wmsTxt.AddDigW((nVal)/(60*60*60), -1);
			wmsTxt.AddW((LPWSTR)L":");
			nVal %= (60 * 60 * 60);
			wmsTxt.AddDigW((nVal)/(60*60), 2);
			wmsTxt.AddW((LPWSTR)L":");
			nVal %= (60 * 60);
			wmsTxt.AddDigW((nVal)/60, 2);
			wmsTxt.AddW((LPWSTR)L".");
			nVal %= 60;
			wmsTxt.AddDigW((nVal)*100/60, 2);
		}
		else
		{
			wmsTxt.SetW((LPWSTR)L"-");
		}
	}
	else
	{
	}
	// GUIのテキストID未取得時は取得する
	if(NUMOK(m_nGuiTxtID, E_MIDGUI_TXT_NUM) == FALSE)
	{
		m_nGuiTxtID = lpMidGui->GetUnsetTxtID(E_MIDGUI_TXT_LBL);
	}
	// GUIドライバに文字列の配置を指示する
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
CRAppLbl::SeqInit(void)
{
	//-----------------------------------------------------------------------
	// GUI表示の更新処理
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
CRAppLbl::SeqIdle(void)
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
CRAppLbl::Update(void)
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
* FunctionName:	SetVal															*
* Function    :	ラベル表示値を設定												*
* Argument    :	表示値															*
* Return      :	none															*
********************************************************************************/
void
CRAppLbl::SetVal(long nVal1, long nVal2)
{
	m_nVal1 = nVal1;
	m_nVal2 = nVal2;
}
