/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: midgui.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: GUI描画のミドルウェア											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"
#include "wmstr.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// 矩形ID
#define	E_MIDGUI_RECT_KEY_IND	(  0)	// 矩形ID:キー表示用の先頭ID
#define	E_MIDGUI_RECT_BOX		(100)	// 矩形ID:箱用の先頭ID
#define	E_MIDGUI_RECT_NUM		(250)	// 矩形ID:総数
// テキストID
#define	E_MIDGUI_TXT_KEY_IND	(  0)	// テキストID:キー表示用の先頭ID
#define	E_MIDGUI_TXT_LBL		(100)	// テキストID:ラベル用の先頭ID
#define	E_MIDGUI_TXT_BOX		(150)	// テキストID:箱用の先頭ID
#define	E_MIDGUI_TXT_DBG		(299)	// テキストID:デバッグ用のID
#define	E_MIDGUI_TXT_NUM		(300)	// テキストID:総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRMidGui : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRMidGui();
			CRMidGui(HWND hWnd);
	virtual	~CRMidGui();
private:
	//----- static変数
	static	CRMidGui* m_lpThis;
	//----- 変数
	HWND	m_hAppWnd;			// ウィンドウハンドル
	bool	m_bDeDraw;			// 描画禁止FG
	long	m_nFPS;				// FPS設定
	long	m_tmFPS;			// FPS用タイマ
	// 変数(矩形描画情報)
	bool		m_abRectSet[E_MIDGUI_RECT_NUM];
	RECT		m_arcRectPos[E_MIDGUI_RECT_NUM];
	COLORREF	m_acrRectFrmCol[E_MIDGUI_RECT_NUM];
	COLORREF	m_acrRectFilCol[E_MIDGUI_RECT_NUM];
	// 変数(文字列描画情報)
	bool		m_abTxtSet[E_MIDGUI_TXT_NUM];
	RECT		m_arcTxtPos[E_MIDGUI_TXT_NUM];
	long		m_anTxtPt[E_MIDGUI_TXT_NUM];
	bool		m_abTxtLeftFit[E_MIDGUI_TXT_NUM];
	CWMStr		m_awmsTxtStr[E_MIDGUI_TXT_NUM];
	COLORREF	m_acrTxtColor[E_MIDGUI_TXT_NUM];
	//----- 関数
	// 矩形の描画処理
	void	DrawRect(HDC hDC, RECT* lpRcClient);
	// テキストの描画処理
	void	DrawTxt(HDC hDC, RECT* lpRcClient);
	// シーケンス
	long	m_nSeq;		// 処理シーケンスNo.
	bool	(CRMidGui::*m_lpSeq[10])(void);
	bool	SeqInit();	// 初期状態
	bool	SeqIdle();	// 待機状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update();
	// 描画禁止FGの更新
	void	DisableDraw(bool bDeDraw){m_bDeDraw = bDeDraw;};
	// FPSの設定
	bool	SetFPS(long nFPS);
	// 未使用の矩形IDを取得する
	long	GetUnsetRectID(long nStartRectID);
	// 描画する矩形を登録する
	void	SetRect(long nRectID, RECT* lpPos, COLORREF crFrm, COLORREF crFil, bool bSet);
	// 描画する矩形を登録削除する
	void	ClrRect(long nRectID);
	// 未使用のテキストIDを取得する
	long	GetUnsetTxtID(long nStartTxtID);
	// 描画するテキストを登録する
	void	SetTxt(long nTxtID, RECT* lpPos, COLORREF crTxtColor, CWMStr* lpStr, long nPt, bool bLeftFit, bool bSet);
	// 描画するテキストを登録削除する
	void	ClrTxt(long nTxtID);

	static	CRMidGui*	GetInstPt(void){return m_lpThis;};
};
