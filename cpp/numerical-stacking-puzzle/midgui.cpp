/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: midgui.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: GUI描画のミドルウェア											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "midgui.h"
#include "wmstr.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス番号
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態

/********************************************************************************
*	Define Static Instance														*
********************************************************************************/
CRMidGui*	CRMidGui::m_lpThis;

/********************************************************************************
*	CRMidGui Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRMidGui														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRMidGui::CRMidGui(void)
{
	// 引数付きで作成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRMidGui														*
* Function    :	コンストラクタ													*
* Argument(s) :	hWnd=ウィンドウハンドル											*
********************************************************************************/
CRMidGui::CRMidGui(HWND hWnd)
{
	//-----------------------------------------------------------------------
	// インスタンスポインタ保存
	m_lpThis = this;

	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT] = &CRMidGui::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE] = &CRMidGui::SeqIdle;	// 待機状態

	//-----------------------------------------------------------------------
	// バッファクリア
	memset(&(m_abRectSet[0]), 0, sizeof(m_abRectSet));
	memset(&(m_arcRectPos[0]), 0, sizeof(m_arcRectPos));
	memset(&(m_acrRectFrmCol[0]), 0, sizeof(m_acrRectFrmCol));
	memset(&(m_acrRectFilCol[0]), 0, sizeof(m_acrRectFilCol));

	memset(&(m_abTxtSet[0]), 0, sizeof(m_abTxtSet));
	memset(&(m_arcTxtPos[0]), 0, sizeof(m_arcTxtPos));
	memset(&(m_anTxtPt[0]), 0, sizeof(m_anTxtPt));
	memset(&(m_abTxtLeftFit[0]), 0, sizeof(m_abTxtLeftFit));
	memset(&(m_acrTxtColor[0]), 0, sizeof(m_acrTxtColor));

	//-----------------------------------------------------------------------
	// 初期値設定
	m_hAppWnd = hWnd;		// ウィンドウハンドル
	m_bDeDraw = FALSE;		// 描画禁止FG <- OFF
	m_nFPS = 1;				// FPS設定 <- 1
	m_tmFPS = m_nFPS;		// FPS用タイマ <- リセット
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRMidGui"));
}

/********************************************************************************
* FunctionName:	~CRMidGui														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRMidGui::~CRMidGui(void)
{
	// インスタンスポインタ破棄
	m_lpThis = nullptr;

	Debuglog((LPWSTR)(L"~CRMidGui."));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	DrawRect														*
* Function    :	矩形の描画処理													*
* Argument(s) :	デバイスコンテキストハンドル,クライアント領域情報				*
* Return      :	none															*
********************************************************************************/
void
CRMidGui::DrawRect(HDC hDC, RECT* lpRcClient)
{
	long nRectID;

	// 矩形ID毎に描画処理する
	for(nRectID = 0; nRectID < E_MIDGUI_RECT_NUM; nRectID++)
	{
		// 有効な矩形IDのみ処理する
		if(m_abRectSet[nRectID])
		{
			// 矩形サイズ決定(クライアント領域に合わせて伸縮)
			RECT rc;
			rc.left = m_arcRectPos[nRectID].left * (lpRcClient->right - lpRcClient->left) / E_WID_X + lpRcClient->left;
			rc.top = m_arcRectPos[nRectID].top * (lpRcClient->bottom - lpRcClient->top) / E_WID_Y + lpRcClient->top;
			rc.right = m_arcRectPos[nRectID].right * (lpRcClient->right - lpRcClient->left) / E_WID_X + lpRcClient->left;
			rc.bottom = m_arcRectPos[nRectID].bottom * (lpRcClient->bottom - lpRcClient->top) / E_WID_Y + lpRcClient->top;
			// 塗り潰し用ブラシハンドル生成 -> FillRect -> 破棄
			HBRUSH hgdi = CreateSolidBrush(m_acrRectFilCol[nRectID]);
			{
				(void)::FillRect(hDC, &rc, hgdi);
			}
			DeleteObject(hgdi);
			// 枠線用ブラシハンドル生成 -> FrameRect -> 破棄
			hgdi = CreateSolidBrush(m_acrRectFrmCol[nRectID]);
			{
				(void)::FrameRect(hDC, &rc, hgdi);
			}
			DeleteObject(hgdi);
		}
	}
}

/********************************************************************************
* FunctionName:	DrawTxt															*
* Function    :	テキストの描画処理												*
* Argument(s) :	デバイスコンテキストハンドル,クライアント領域情報				*
* Return      :	none															*
********************************************************************************/
void
CRMidGui::DrawTxt(HDC hDC, RECT* lpRcClient)
{
	long nTxtID;

	// フォントの作成
	HFONT hFont = NULL;		// フォントハンドル
	HFONT hOldFont = NULL;	// 以前のフォントハンドル
	COLORREF crOldTxtColor = RGB(0x00,0x00,0x00);

	TEXTMETRIC tm;			// フォント情報
	// "ＭＳ Ｐゴシック"のフォントにする
	// ※フォントサイズはTxtID毎に個別設定するので、ここでは設定しない
	LOGFONT logfont;
//	logfont.lfHeight         = 32;							// ポイント
//	logfont.lfWidth          = 0;							// 幅（０：だと自動設定）
	logfont.lfEscapement     = 0;							// テキスト行の描画角度
	logfont.lfOrientation    = 0;							// ベースラインの角度
	logfont.lfWeight         = 400;							// 文字の太さ（400:標準 700:太字）
	logfont.lfItalic         = FALSE;						// 斜体指定
	logfont.lfUnderline      = FALSE;						// 下線付き指定
	logfont.lfStrikeOut      = FALSE;						// 打ち消し線指定
	logfont.lfCharSet        = SHIFTJIS_CHARSET;			// 文字セット指定
	logfont.lfOutPrecision   = OUT_DEFAULT_PRECIS;			// フォントの出力精度指定
	logfont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;			// クリッピング精度指定 
	logfont.lfQuality        = DEFAULT_QUALITY;				// フォントの出力品質指定
	logfont.lfPitchAndFamily = DEFAULT_PITCH;				// フォントのピッチ指定
	// フォントの書体名指定
	wcscpy_s(logfont.lfFaceName, LF_FACESIZE, (LPWSTR)(L"ＭＳ Ｐゴシック"));

	// テキストID毎に描画処理する
	for(nTxtID = 0; nTxtID < E_MIDGUI_TXT_NUM; nTxtID++)
	{
		// 有効なテキストIDのみ処理する
		if(m_abTxtSet[nTxtID])
		{
			// 矩形サイズ決定(クライアント領域に合わせて伸縮)
			RECT rc;
			rc.left = m_arcTxtPos[nTxtID].left * (lpRcClient->right - lpRcClient->left) / E_WID_X + lpRcClient->left;
			rc.top = m_arcTxtPos[nTxtID].top * (lpRcClient->bottom - lpRcClient->top) / E_WID_Y + lpRcClient->top;
			rc.right = m_arcTxtPos[nTxtID].right * (lpRcClient->right - lpRcClient->left) / E_WID_X + lpRcClient->left;
			rc.bottom = m_arcTxtPos[nTxtID].bottom * (lpRcClient->bottom - lpRcClient->top) / E_WID_Y + lpRcClient->top;
			// フォントサイズ設定
			logfont.lfHeight = m_anTxtPt[nTxtID] * (lpRcClient->bottom - lpRcClient->top) / E_WID_Y;			// ポイント
			logfont.lfWidth  = m_anTxtPt[nTxtID] * (lpRcClient->right - lpRcClient->left) / E_WID_X / 2;		// ポイント
			// フォントフォーマット設定
			UINT fFormat = 0;
			if(m_abTxtLeftFit[nTxtID] != FALSE)
			{
				fFormat = (DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			}
			else
			{
				fFormat = (DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			// フォントハンドル生成 -> DrawText -> 破棄
			hFont = CreateFontIndirect(&logfont);
			{
				// フォントを入れ替え -> DrawText -> フォントを入れ替え(元に戻す)
				hOldFont = (HFONT)SelectObject(hDC, hFont);
				{
					crOldTxtColor = SetTextColor(hDC, m_acrTxtColor[nTxtID]);
					{
						// 現在のフォント情報を取得
						GetTextMetrics(hDC, &tm);
						SetBkMode(hDC, TRANSPARENT);
						
						::DrawText(hDC,
						           m_awmsTxtStr[nTxtID].PtW(),
						           (long)m_awmsTxtStr[nTxtID].LenW(),
						           &rc,
						           fFormat);
						SetBkMode(hDC, OPAQUE);
					}
					(void)SetTextColor(hDC, crOldTxtColor);
				}
				(void)SelectObject(hDC,hOldFont);
			}
			DeleteObject(hFont);
		}
	}
}

/********************************************************************************
*	Private Methods (Sequence)													*
********************************************************************************/

/********************************************************************************
* FunctionName:	SeqInit															*
* Function    :	シーケンス処理:初期化											*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRMidGui::SeqInit(void)
{
	Debuglog((LPWSTR)(L"MidGuiInit."));

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
CRMidGui::SeqIdle(void)
{
	HDC hDC;
	HDC hMemDC;
	RECT rcClient;
	HBITMAP hBitmap;

	// FPSタイミングのみ実行
	if(m_tmFPS == 0)
	{
		// 描画許可時のみ実行
		if(m_bDeDraw == FALSE)
		{
			// WindowハンドルからDC取得 -> 描画 -> 解放
			hDC = GetDC(m_hAppWnd);
			{
				// クライアント領域取得
				GetClientRect(m_hAppWnd, &rcClient);
				// DCからメモリDC取得 -> 描画 -> 廃棄
				hMemDC = CreateCompatibleDC(hDC);
				{
					// DCから互換ビットマップハンドル生成 -> 描画 -> 廃棄
					hBitmap = CreateCompatibleBitmap(hDC, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
					{
						// MemDCに互換ビットマップハンドル選択 -> 描画 -> MemDCに元のGDIオブジェクトを選択(元に戻す)
						HGDIOBJ hOldGdi = SelectObject(hMemDC, hBitmap);
						{
							FillRect(hMemDC, &rcClient, (HBRUSH) (COLOR_WINDOW+1));
							DrawRect(hMemDC, &rcClient);
							DrawTxt(hMemDC, &rcClient);
							BitBlt(hDC, rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hMemDC, 0, 0, SRCCOPY);
						}
						SelectObject(hMemDC, hOldGdi);
					}
					DeleteObject(hBitmap);
				}
				DeleteDC(hMemDC);
			}
			ReleaseDC(m_hAppWnd, hDC);
		}
		m_tmFPS = m_nFPS;
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
CRMidGui::Update()
{
	//-----------------------------------------------------------------------
	// 基底クラスの処理
	if(this->CRoutine::Update() == FALSE)
	{
		return FALSE;
	}

	Dec(m_tmFPS);

	//-----------------------------------------------------------------------
	// シーケンス処理
	if((this->*m_lpSeq[m_nSeq])() == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	SetFPS															*
* Function    :	FPSの設定														*
* Argument(s) :	FPSの設定値														*
* Return      :	FPSの設定可否													*
********************************************************************************/
bool
CRMidGui::SetFPS(long nFPS)
{
	if(nFPS <= 0 || nFPS >= 60)
	{
		return FALSE;
	}
	m_nFPS = nFPS;
	return TRUE;
}

/********************************************************************************
* FunctionName:	GetUnsetRectID													*
* Function    :	未使用の矩形IDを取得する										*
* Argument(s) :	nStartRectID=空きを探す先頭の矩形ID								*
* Return      :	未使用の矩形ID ただし発見できなかった場合は-1を返す				*
********************************************************************************/
long
CRMidGui::GetUnsetRectID(long nStartRectID)
{
	long nRet = -1;

	for(; nStartRectID < E_MIDGUI_RECT_NUM; nStartRectID++)
	{
		if(m_abRectSet[nStartRectID] == FALSE)
		{
			nRet = nStartRectID;
			break;
		}
	}

	return nRet;
}

/********************************************************************************
* FunctionName:	SetRect															*
* Function    :	描画する矩形を登録する											*
* Argument(s) :	nRectID=矩形ID, lpPos=矩形位置									*
*				crFrm=枠線色, crFil=塗潰し色									*
*				bSet=登録(=TRUE)/削除(=FALSE)									*
* Return      :	none															*
********************************************************************************/
void
CRMidGui::SetRect(long nRectID, RECT* lpPos, COLORREF crFrm, COLORREF crFil, bool bSet)
{
	m_arcRectPos[nRectID] = *lpPos;
	m_acrRectFrmCol[nRectID] = crFrm;
	m_acrRectFilCol[nRectID] = crFil;

	m_abRectSet[nRectID] = bSet;
}

/********************************************************************************
* FunctionName:	ClrRect															*
* Function    :	描画する矩形を登録削除する										*
* Argument(s) :	nRectID=矩形ID													*
* Return      :	none															*
********************************************************************************/
void
CRMidGui::ClrRect(long nRectID)
{
	m_abRectSet[nRectID] = FALSE;
}

/********************************************************************************
* FunctionName:	GetUnsetTxtID													*
* Function    :	未使用のテキストIDを取得する									*
* Argument(s) :	nStartTxtID=空きを探す先頭のテキストID							*
* Return      :	未使用のテキストID ただし発見できなかった場合は-1を返す			*
********************************************************************************/
long
CRMidGui::GetUnsetTxtID(long nStartTxtID)
{
	long nRet = -1;

	for(; nStartTxtID < E_MIDGUI_TXT_NUM; nStartTxtID++)
	{
		if(m_abTxtSet[nStartTxtID] == FALSE)
		{
			nRet = nStartTxtID;
			break;
		}
	}

	return nRet;
}

/********************************************************************************
* FunctionName:	SetTxt															*
* Function    :	描画するテキストを登録する										*
* Argument(s) :	nTxtID=テキストID, lpPos=テキスト位置							*
*				crTxtColor=テキスト色, nPt=文字サイズ,							*
*				bLeftFit=TRUEを設定すると左端揃え描画							*
*				bSet=登録(=TRUE)/削除(=FALSE)									*
* Return      :	none															*
********************************************************************************/
void
CRMidGui::SetTxt(long nTxtID, RECT* lpPos, COLORREF crTxtColor, CWMStr* lpStr, long nPt, bool bLeftFit, bool bSet)
{
	m_arcTxtPos[nTxtID] = *lpPos;
	m_anTxtPt[nTxtID] = nPt;
	m_abTxtLeftFit[nTxtID] = bLeftFit;
	m_awmsTxtStr[nTxtID].SetW(lpStr->PtW());
	m_acrTxtColor[nTxtID] = crTxtColor;

	m_abTxtSet[nTxtID] = bSet;
}

/********************************************************************************
* FunctionName:	ClrTxt															*
* Function    :	描画するテキストを登録削除する									*
* Argument(s) :	nTxtID=テキストID												*
* Return      :	none															*
********************************************************************************/
void
CRMidGui::ClrTxt(long nTxtID)
{
	m_abTxtSet[nTxtID] = FALSE;
}

