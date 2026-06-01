/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appnextboxset.cpp												*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 次回箱オブジェクト全体管理									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "appnextboxset.h"
#include "nextdat.h"
#include "appbox.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態
#define	E_SEQ_PUSH	(2)	// PUSH動作状態

// タイマー
#define	E_TM_PUSH	( 0)	// PUSH動作状態の処理時間

// 子プロセス
#define	E_RT_NUM	(E_NEXT_BOX_NUM)//子プロセスの上限数

/********************************************************************************
*	CRAppNextBoxSet Public Constructor(s)/Destructor							*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppNextBoxSet													*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppNextBoxSet::CRAppNextBoxSet(void)
{
	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT ] = &CRAppNextBoxSet::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE ] = &CRAppNextBoxSet::SeqIdle;	// 待機状態
	m_lpSeq[E_SEQ_PUSH]  = &CRAppNextBoxSet::SeqPush;	// PUSH動作状態
	// 子プロセス
	if(SetChildNum(E_RT_NUM) == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nStat = E_APP_NEXT_BOX_SET_STAT_IDLE;
	m_tmSeq = 0;			// シーケンスタイマー <- 0
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRAppNextBoxSet"));

	return;
}

/********************************************************************************
* FunctionName:	~CRAppNextBoxSet												*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppNextBoxSet::~CRAppNextBoxSet(void)
{
	Debuglog((LPWSTR)(L"~CRAppNextBoxSet"));
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
CRAppNextBoxSet::CreateGuiChild(void)
{
	CRNextDat* lpNextDat;	// 次落下箱管理インスタンス
	long nCellX;			// セルデータ位置X
	long val;				// 次回箱表示値

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次回箱データ毎に次回箱オブジェクトの生成
	for(nCellX = 0; nCellX < E_NEXT_BOX_NUM; nCellX++)
	{
		val = lpNextDat->GetVal(nCellX);
		if(val >= 0)
		{
			NEW(m_lplpRtChild[nCellX], CRAppBox(val, nCellX*100+1200, 0));
			if(m_lplpRtChild[nCellX] == nullptr)
			{
				return FALSE;
			}
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
CRAppNextBoxSet::IsChildAlive(void)
{
	long nCellX;			// セルデータ位置X

	//-----------------------------------------------------------------------
	// 次回箱オブジェクト
	for(nCellX = 0; nCellX < E_NEXT_BOX_NUM; nCellX++)
	{
		if(m_lplpRtChild[nCellX] == nullptr)
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
CRAppNextBoxSet::SeqInit(void)
{
	//-----------------------------------------------------------------------
	// GUI系の子プロセスの生成
	if(CreateGuiChild() == FALSE)
	{
		Assert(FALSE);
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
CRAppNextBoxSet::SeqIdle(void)
{
	CRNextDat* lpNextDat;	// 次落下箱管理インスタンス
	long nCellX;			// セルデータ位置X
	long val;				// 次回箱表示値
	CRAppBox* lpAppNextBox;	// 次回箱オブジェクト

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次回箱全体処理状態がPUSH動作の場合はPUSH動作状態へ
	if(m_nStat == E_APP_NEXT_BOX_SET_STAT_PUSH)
	{
		for(nCellX = 0; nCellX < E_NEXT_BOX_NUM; nCellX++)
		{
			val = lpNextDat->GetVal(nCellX);
			if(val >= 0)
			{
				NEW(m_lplpRtChild[nCellX], CRAppBox(val, nCellX*100+1300, 0));
				if(m_lplpRtChild[nCellX] != nullptr)
				{
					lpAppNextBox = (CRAppBox*)m_lplpRtChild[nCellX];
					lpAppNextBox->SetStatServe(nCellX*100 + 1200, 0);
				}
				else
				{
					return FALSE;
				}
			}
		}

		// シーケンス -> PUSH動作状態
		m_nSeq = E_SEQ_PUSH;
		m_tmSeq = E_TM_PUSH;
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqPush														*
* Purpose	: シーケンス＝PUSH状態												*
********************************************************************************/
bool
CRAppNextBoxSet::SeqPush(void)
{
	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(PUSH中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		// タイマーカウント後
		// 次回箱全体処理状態=待機状態
		m_nStat = E_APP_NEXT_BOX_SET_STAT_IDLE;
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
CRAppNextBoxSet::Update(void)
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
* FunctionName:	SetStatPush														*
* Function    :	次回箱全体処理状態をPUSH動作に設定								*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppNextBoxSet::SetStatPush(void)
{
	m_nStat = E_APP_NEXT_BOX_SET_STAT_PUSH;
}
