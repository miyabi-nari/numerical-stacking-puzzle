/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appboxset.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 箱オブジェクト全体管理										*
********************************************************************************/

/********************************************************************************
*	Include files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "appboxset.h"
#include "celldat.h"
#include "appbox.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態
#define	E_SEQ_STACK	(2)	// 積上動作状態
#define	E_SEQ_MERGE	(3)	// 結合動作状態
#define	E_SEQ_LAND	(4)	// 着地動作状態

// タイマー
#define	E_TM_STACK	( 0)	// 積上動作状態の処理時間
#define	E_TM_MERGE	(15)	// 結合動作状態の処理時間
#define	E_TM_LAND	( 0)	// 着地動作状態の処理時間

// 子プロセス
#define	E_RT_APP_BOX			(  0)	// 箱オブジェクト先頭
#define	E_RT_NUM				(160)//子プロセスの上限数

/********************************************************************************
*	CRAppBoxSet Public Constructor(s)/Destructor								*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppBoxSet														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppBoxSet::CRAppBoxSet(void)
{
	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT ] = &CRAppBoxSet::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE ] = &CRAppBoxSet::SeqIdle;	// 待機状態
	m_lpSeq[E_SEQ_STACK] = &CRAppBoxSet::SeqStack;	// 積上動作状態
	m_lpSeq[E_SEQ_MERGE] = &CRAppBoxSet::SeqMerge;	// 結合動作状態
	m_lpSeq[E_SEQ_LAND ] = &CRAppBoxSet::SeqLand;	// 着地動作状態
	// 子プロセス
	if(SetChildNum(E_RT_NUM) == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// バッファクリア
	memset(&(m_anBoxRt[0][0]), -1, sizeof(m_anBoxRt));

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nStat = E_APP_BOX_SET_STAT_IDLE;
	m_tmSeq = 0;			// シーケンスタイマー <- 0
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRAppBoxSet"));

	return;
}

/********************************************************************************
* FunctionName:	~CRAppBoxSet													*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppBoxSet::~CRAppBoxSet(void)
{
	Debuglog((LPWSTR)(L"~CRAppBoxSet"));
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
CRAppBoxSet::CreateGuiChild(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	long nCellX;			// セルデータ位置X
	long nCellY;			// セルデータ位置Y
	long val;				// 箱表示値
	long nRt;				// 子プロセスNo.

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	nRt = 0;

	//-----------------------------------------------------------------------
	// セルデータ毎に箱オブジェクトの生成
	for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
	{
		for(nCellY = 0; nCellY < E_CELL_Y_NUM; nCellY++)
		{
			val = lpCellDat->GetVal(nCellX, nCellY);
			if(val >= 0)
			{
				while(nRt < E_RT_NUM)
				{
					if(m_lplpRtChild[nRt] == nullptr)
					{
						NEW(m_lplpRtChild[nRt], CRAppBox(val, nCellX*100, nCellY*100));
						if(m_lplpRtChild[nRt] != nullptr)
						{
							m_anBoxRt[nCellX][nCellY] = nRt;
							break;
						}
						else
						{
							return FALSE;
						}
					}
					nRt++;
				}
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
CRAppBoxSet::IsChildAlive(void)
{
	long nCellX;			// セルデータ位置X
	long nCellY;			// セルデータ位置Y
	long nRt;				// 子プロセスNo.

	//-----------------------------------------------------------------------
	// 箱オブジェクト
	for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
	{
		for(nCellY = 0; nCellY < E_CELL_Y_NUM; nCellY++)
		{
			nRt = m_anBoxRt[nCellX][nCellY];
			if(NUMOK(nRt, E_RT_NUM) != FALSE)
			{
				if(m_lplpRtChild[nRt] == nullptr)
				{
					return FALSE;
				}
			}
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
CRAppBoxSet::SeqInit(void)
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
CRAppBoxSet::SeqIdle(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	long nCellX;			// セルデータ位置X
	long nCellY;			// セルデータ位置Y
	long nRt;				// 子プロセスNo.
	long val;				// 箱表示値
	CRAppBox* lpAppBox;		// 箱オブジェクト
	long nMergeX;
	long nMergeY;
	long nLandY;

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	nRt = 0;

	//-----------------------------------------------------------------------
	// 箱全体処理状態が積上動作の場合は積上動作状態へ
	if(m_nStat == E_APP_BOX_SET_STAT_STACK)
	{
		// 箱に対応する箱オブジェクトが無い場合は生成する
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			for(nCellY = 0; nCellY < E_CELL_Y_NUM; nCellY++)
			{
				val = lpCellDat->GetVal(nCellX, nCellY);
				if(val < 0)
				{
					val = lpCellDat->GetStackVal(nCellX, nCellY);
				}
				if(val >= 0)
				{
					if(m_anBoxRt[nCellX][nCellY] < 0)
					{
						while(nRt < E_RT_NUM)
						{
							if(m_lplpRtChild[nRt] == nullptr)
							{
								NEW(m_lplpRtChild[nRt], CRAppBox(val, nCellX*100, nCellY*100));
								if(m_lplpRtChild[nRt] != nullptr)
								{
									m_anBoxRt[nCellX][nCellY] = nRt;
									break;
								}
								else
								{
									break;
								}
							}
							nRt++;
						}
					}
				}
			}
		}
		// シーケンス -> 結合動作状態
		m_nSeq = E_SEQ_STACK;
		m_tmSeq = E_TM_STACK;
	}

	//-----------------------------------------------------------------------
	// 箱全体処理状態が結合動作の場合は結合動作状態へ
	if(m_nStat == E_APP_BOX_SET_STAT_MERGE)
	{
		// 結合する箱を「スライド移動」「種類変更」の状態に更新
		// （結合位置にある箱＝種類変更、結合位置にない箱＝結合位置までスライド移動）
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			for(nCellY = 0; nCellY < E_CELL_Y_NUM; nCellY++)
			{
				nRt = m_anBoxRt[nCellX][nCellY];
				if(nRt >= 0)
				{
					nMergeX = lpCellDat->GetMergeX(nCellX, nCellY);
					nMergeY = lpCellDat->GetMergeY(nCellX, nCellY);
					if(nMergeX >= 0 &&
					   nMergeY >= 0)
					{
						lpAppBox = (CRAppBox*)m_lplpRtChild[nRt];
						if(nMergeX == nCellX &&
						   nMergeY == nCellY)
						{
							val = lpCellDat->GetMergeVal(nCellX, nCellY);
							if(val > lpAppBox->GetBoxTypeID())
							{
								lpAppBox->SetStatChange(val);
							}
						}
						else
						{
							lpAppBox->SetStatSlide(nMergeX*100, nMergeY*100);
							m_anBoxRt[nCellX][nCellY] = -1;
						}
					}
				}
			}
		}

		// シーケンス -> 結合動作状態
		m_nSeq = E_SEQ_MERGE;
		m_tmSeq = E_TM_MERGE;
	}

	//-----------------------------------------------------------------------
	// 箱全体処理状態が着地動作の場合は着地動作状態へ
	if(m_nStat == E_APP_BOX_SET_STAT_LAND)
	{
		// 着地する箱＝落下状態に設定
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			for(nCellY = E_CELL_Y_NUM-1; nCellY >= 0; nCellY--)
			{
				nRt = m_anBoxRt[nCellX][nCellY];
				if(nRt >= 0)
				{
					nLandY = lpCellDat->GetLandY(nCellX, nCellY);
					if(nLandY >= 0)
					{
						lpAppBox = (CRAppBox*)m_lplpRtChild[nRt];
						lpAppBox->SetStatFall(nCellX*100, nLandY*100);
						m_anBoxRt[nCellX][nCellY] = -1;
						m_anBoxRt[nCellX][nLandY] = nRt;
					}
				}
			}
		}

		// シーケンス -> 着地動作状態
		m_nSeq = E_SEQ_LAND;
		m_tmSeq = E_TM_LAND;
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqStack														*
* Purpose	: シーケンス＝積上状態												*
********************************************************************************/
bool
CRAppBoxSet::SeqStack(void)
{
	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(積上中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		// 箱全体処理状態=待機状態
		m_nStat = E_APP_BOX_SET_STAT_IDLE;
		// シーケンス -> 待機状態
		m_nSeq = E_SEQ_IDLE;
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqMerge														*
* Purpose	: シーケンス＝結合状態												*
********************************************************************************/
bool
CRAppBoxSet::SeqMerge(void)
{
	CRAppBox* lpAppBox;		// 箱オブジェクト
	long nRt;				// 子プロセスNo.

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(結合中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		for(nRt = 0; nRt < E_RT_NUM; nRt++)
		{
			if(m_lplpRtChild[nRt] != nullptr)
			{
				lpAppBox = (CRAppBox*)m_lplpRtChild[nRt];
				if(lpAppBox->GetStat() == E_APP_BOX_STAT_SLIDE)
				{
					FREE(m_lplpRtChild[nRt]);
				}
			}
		}
		// 箱全体処理状態=待機状態
		m_nStat = E_APP_BOX_SET_STAT_IDLE;
		// シーケンス -> 待機状態
		m_nSeq = E_SEQ_IDLE;
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqLand														*
* Purpose	: シーケンス＝着地状態												*
********************************************************************************/
bool
CRAppBoxSet::SeqLand(void)
{
	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// タイマーカウント中は待機(着地中)
	// タイマーカウント後は待機状態へ
	if(m_tmSeq == 0)
	{
		// タイマーカウント後
		// 箱全体処理状態=待機状態
		m_nStat = E_APP_BOX_SET_STAT_IDLE;
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
CRAppBoxSet::Update(void)
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
* FunctionName:	SetStatStack													*
* Function    :	箱全体処理状態を積上動作に設定									*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppBoxSet::SetStatStack(void)
{
	m_nStat = E_APP_BOX_SET_STAT_STACK;
}

/********************************************************************************
* FunctionName:	SetStatMerge													*
* Function    :	箱全体処理状態を結合動作に設定									*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppBoxSet::SetStatMerge(void)
{
	m_nStat = E_APP_BOX_SET_STAT_MERGE;
}

/********************************************************************************
* FunctionName:	SetStatLand														*
* Function    :	箱全体処理状態を着地動作に設定									*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppBoxSet::SetStatLand(void)
{
	m_nStat = E_APP_BOX_SET_STAT_LAND;
}

