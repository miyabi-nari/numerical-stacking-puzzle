/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: applblset.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ラベルオブジェクト全体管理									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "applblset.h"
#include "applbl.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態

// 子プロセス
#define	E_RT_NUM	(E_APP_LBL_TYPE_NUM)//子プロセスの上限数

/********************************************************************************
*	CRAppLblSet Public Constructor(s)/Destructor								*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppLblSet														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppLblSet::CRAppLblSet(void)
{
	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT ] = &CRAppLblSet::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE ] = &CRAppLblSet::SeqIdle;	// 待機状態
	// 子プロセス
	if(SetChildNum(E_RT_NUM) == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// GUI系の子プロセスの生成
	if(CreateGuiChild() == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nSeq = E_SEQ_IDLE;

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRAppLblSet"));

	return;
}

/********************************************************************************
* FunctionName:	~CRAppLblSet													*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppLblSet::~CRAppLblSet(void)
{
	Debuglog((LPWSTR)(L"~CRAppLblSet"));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	CreateGuiChild													*
* Function    :	GUI系の子プロセスの生成											*
* Argument(s) :	none															*
* Return      :	FALSE=生成失敗													*
********************************************************************************/
bool
CRAppLblSet::CreateGuiChild(void)
{
	long nID;	// ラベルID

	//-----------------------------------------------------------------------
	// ラベル種類毎にラベルオブジェクトの生成
	for(nID = 0; nID < E_APP_LBL_TYPE_NUM; nID++)
	{
		NEW(m_lplpRtChild[nID], CRAppLbl(nID));
		if(m_lplpRtChild[nID] == nullptr)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	IsChildAlive													*
* Function    :	子プロセスの生存判定											*
* Argument(s) :	none															*
* Return      :	TRUE=すべて生存													*
********************************************************************************/
bool
CRAppLblSet::IsChildAlive(void)
{
	long nID;	// ラベルID

	//-----------------------------------------------------------------------
	// ラベルオブジェクト
	for(nID = 0; nID < E_APP_LBL_TYPE_NUM; nID++)
	{
		if(m_lplpRtChild[nID] == nullptr)
		{
			return FALSE;
		}
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
CRAppLblSet::SeqInit(void)
{
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
CRAppLblSet::SeqIdle(void)
{
	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
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
CRAppLblSet::Update(void)
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
* FunctionName:	SetScoreVal														*
* Function    :	スコア表示値を設定												*
* Argument    :	表示値															*
* Return      :	none															*
********************************************************************************/
void
CRAppLblSet::SetScoreVal(long nScoreVal)
{
	CRAppLbl* lpAppLbl;
	
	lpAppLbl = ((CRAppLbl*)m_lplpRtChild[E_APP_LBL_TYPE_SCORE_VAL]);
	
	if(lpAppLbl != nullptr)
	{
		lpAppLbl->SetVal(nScoreVal, 0);
	}
}

/********************************************************************************
* FunctionName:	SetElapsedTimeVal												*
* Function    :	経過時間表示値を設定											*
* Argument    :	表示値															*
* Return      :	none															*
********************************************************************************/
void
CRAppLblSet::SetElapsedTimeVal(long nTimeVal)
{
	CRAppLbl* lpAppLbl;
	
	lpAppLbl = ((CRAppLbl*)m_lplpRtChild[E_APP_LBL_TYPE_ELAPSED_TIME_VAL]);
	
	if(lpAppLbl != nullptr)
	{
		lpAppLbl->SetVal(nTimeVal, 0);
	}
}

/********************************************************************************
* FunctionName:	SetMergeTimeVal													*
* Function    :	マージ時間表示値を設定											*
* Argument    :	表示値															*
* Return      :	none															*
********************************************************************************/
void
CRAppLblSet::SetMergeTimeVal(long nIndex, long nTimeVal, long nPowVal)
{
	CRAppLbl* lpAppLbl;

	if(nIndex < 5)
	{
		lpAppLbl = ((CRAppLbl*)m_lplpRtChild[E_APP_LBL_TYPE_MERGED_TIME_VAL_1 + nIndex]);
		if(lpAppLbl != nullptr)
		{
			lpAppLbl->SetVal(nTimeVal, nPowVal);
		}
	}
}

