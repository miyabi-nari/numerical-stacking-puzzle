/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: midsnd.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 音再生のミドルウェア											*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include <MMSystem.h>
#include "define.h"
#include "midsnd.h"

extern const u_char F_MIDSND_BUF_STACK[];
extern const u_char F_MIDSND_BUF_MERGE_1[];
extern const u_char F_MIDSND_BUF_MERGE_2[];
extern const u_char F_MIDSND_BUF_MERGE_3[];

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス番号
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態

/********************************************************************************
*	Define Static Instance														*
********************************************************************************/
CRMidSnd*	CRMidSnd::m_lpThis;

/********************************************************************************
*	CRMidSnd Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRMidSnd														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRMidSnd::CRMidSnd(void)
{
	// 引数付きで作成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRMidSnd														*
* Function    :	コンストラクタ													*
* Argument(s) :	hInst=インスタンスハンドル										*
********************************************************************************/
CRMidSnd::CRMidSnd(HINSTANCE hInst)
{
	//-----------------------------------------------------------------------
	// インスタンスポインタ保存
	m_lpThis = this;

	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT] = &CRMidSnd::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE] = &CRMidSnd::SeqIdle;	// 待機状態

	//-----------------------------------------------------------------------
	// バッファクリア
	memset(&(m_abSndSet[0]), 0, sizeof(m_abSndSet));

	//-----------------------------------------------------------------------
	// 初期値設定
	m_hAppInst = hInst;		// インスタンスハンドル
	m_bDePlay = FALSE;		// 再生禁止FG <- OFF
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRMidSnd"));
}

/********************************************************************************
* FunctionName:	~CRMidSnd														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRMidSnd::~CRMidSnd(void)
{
	// インスタンスポインタ破棄
	m_lpThis = nullptr;

	Debuglog((LPWSTR)(L"~CRMidSnd."));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	PlaySnd															*
* Function    :	音の再生処理													*
* Argument(s) :	none															*
* Return      :	none															*
********************************************************************************/
void
CRMidSnd::PlaySnd(void)
{
	static const u_char* const F_SNDNAME[E_MIDSND_NUM] =
	{
		F_MIDSND_BUF_STACK,
		F_MIDSND_BUF_MERGE_1,
		F_MIDSND_BUF_MERGE_2,
		F_MIDSND_BUF_MERGE_3,
	};

	long nSndID;

	// 音ID毎に再生処理する
	for(nSndID = 0; nSndID < E_MIDSND_NUM; nSndID++)
	{
		// 有効な音IDのみ処理する
		if(m_abSndSet[nSndID])
		{
			// 再生許可時のみ実行
			if(m_bDePlay == FALSE)
			{
				// メモリ上の音データを非同期で再生
				(void)::PlaySound((LPCWSTR)F_SNDNAME[nSndID], nullptr, SND_MEMORY | SND_ASYNC);
			}
			m_abSndSet[nSndID] = FALSE;
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
CRMidSnd::SeqInit(void)
{
	Debuglog((LPWSTR)(L"MidSndInit."));

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
CRMidSnd::SeqIdle(void)
{
	PlaySnd();

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
CRMidSnd::Update()
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
* FunctionName:	SetSnd															*
* Function    :	再生する音声を登録する											*
* Argument(s) :	nSndID=音ID														*
* Return      :	none															*
********************************************************************************/
void
CRMidSnd::SetSnd(long nSndID)
{
	m_abSndSet[nSndID] = TRUE;
}

