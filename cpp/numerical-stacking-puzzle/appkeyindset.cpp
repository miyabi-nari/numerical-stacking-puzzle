/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appkeyindset.cpp												*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: キー表示オブジェクト全体管理									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "appkeyindset.h"
#include "appkeyind.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態

// 子プロセス
#define	E_RT_NUM	(E_CELL_X_NUM*2)//子プロセスの上限数

/********************************************************************************
*	CRAppKeyIndSet Public Constructor(s)/Destructor								*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppKeyIndSet													*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppKeyIndSet::CRAppKeyIndSet(void)
{
	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT ] = &CRAppKeyIndSet::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE ] = &CRAppKeyIndSet::SeqIdle;	// 待機状態
	// 子プロセス
	if(SetChildNum(E_RT_NUM) == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRAppKeyIndSet"));

	return;
}

/********************************************************************************
* FunctionName:	~CRAppKeyIndSet													*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppKeyIndSet::~CRAppKeyIndSet(void)
{
	Debuglog((LPWSTR)(L"~CRAppKeyIndSet"));
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
CRAppKeyIndSet::CreateGuiChild(void)
{
	long nCellX;			// セルデータ位置X
	long nCellY;			// セルデータ位置Y
	long nRt;				// 子プロセスNo.

	nRt = 0;

	//-----------------------------------------------------------------------
	// セルデータ毎にキー表示オブジェクトの生成
	for(nCellY = 0; nCellY < 2; nCellY++)
	{
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			NEW(m_lplpRtChild[nRt], CRAppKeyInd(nCellX, nCellX*100, nCellY*800));
			if(m_lplpRtChild[nRt] == nullptr)
			{
				return FALSE;
			}
			nRt++;
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
CRAppKeyIndSet::IsChildAlive(void)
{
	long nCellX;			// セルデータ位置X
	long nCellY;			// セルデータ位置Y
	long nRt;				// 子プロセスNo.

	nRt = 0;

	//-----------------------------------------------------------------------
	// キー表示オブジェクト
	for(nCellY = 0; nCellY < 2; nCellY++)
	{
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			if(m_lplpRtChild[nRt] == nullptr)
			{
				return FALSE;
			}
			nRt++;
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
CRAppKeyIndSet::SeqInit(void)
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
CRAppKeyIndSet::SeqIdle(void)
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
CRAppKeyIndSet::Update(void)
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
