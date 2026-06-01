/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: celldat.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: セルのデータ管理												*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "celldat.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT	(0)	// 初期状態
#define	E_SEQ_IDLE	(1)	// 待機状態

/********************************************************************************
*	Define Static Instance														*
********************************************************************************/
CRCellDat*	CRCellDat::m_lpThis;

/********************************************************************************
*	CRCellDat Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRCellDat														*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRCellDat::CRCellDat(void)
{
	//-----------------------------------------------------------------------
	// インスタンスポインタ保存
	m_lpThis = this;

	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT] = &CRCellDat::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_IDLE] = &CRCellDat::SeqIdle;	// 待機状態

	//-----------------------------------------------------------------------
	// バッファクリア
	memset(&(m_atatCellDat[0][0]), -1, sizeof(m_atatCellDat));

	//-----------------------------------------------------------------------
	// 初期値設定
	m_nScore = 0;			// 得点 <- 0
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRCellDat"));
}

/********************************************************************************
* FunctionName:	~CRCellDat														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRCellDat::~CRCellDat(void)
{
	// インスタンスポインタ破棄
	m_lpThis = nullptr;

	Debuglog((LPWSTR)(L"~CRCellDat"));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

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
CRCellDat::SeqInit(void)
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
CRCellDat::SeqIdle(void)
{
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
CRCellDat::Update()
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
* Function    :	セル格納値を設定												*
* Argument    :	格納位置X,Y														*
*				格納値															*
* Return      :	none															*
********************************************************************************/
void
CRCellDat::SetVal(long nX, long nY, long nVal)
{
	m_atatCellDat[nX][nY].nVal = nVal;
}

/********************************************************************************
* FunctionName:	GetVal															*
* Function    :	セル格納値を取得												*
* Argument    :	格納位置X,Y														*
* Return      :	セル格納値														*
********************************************************************************/
long
CRCellDat::GetVal(long nX, long nY)
{
	return m_atatCellDat[nX][nY].nVal;
}

/********************************************************************************
* FunctionName:	GetStackVal														*
* Function    :	スタック情報を取得												*
* Argument    :	格納位置X,Y														*
* Return      :	スタック情報													*
********************************************************************************/
long
CRCellDat::GetStackVal(long nX, long nY)
{
	return m_atatCellDat[nX][nY].nStackVal;
}

/********************************************************************************
* FunctionName:	GetMergeVal														*
* Function    :	マージするセルの値を取得										*
* Argument    :	格納位置X,Y														*
* Return      :	マージするセルの値												*
********************************************************************************/
long
CRCellDat::GetMergeVal(long nX, long nY)
{
	return m_atatCellDat[nX][nY].nMergeVal;
}

/********************************************************************************
* FunctionName:	GetMergeX														*
* Function    :	マージ位置を取得												*
* Argument    :	格納位置X,Y														*
* Return      :	マージ位置														*
********************************************************************************/
long
CRCellDat::GetMergeX(long nX, long nY)
{
	return m_atatCellDat[nX][nY].nMergeX;
}

/********************************************************************************
* FunctionName:	GetMergeY														*
* Function    :	マージ位置を取得												*
* Argument    :	格納位置X,Y														*
* Return      :	マージ位置														*
********************************************************************************/
long
CRCellDat::GetMergeY(long nX, long nY)
{
	return m_atatCellDat[nX][nY].nMergeY;
}

/********************************************************************************
* FunctionName:	GetLandY														*
* Function    :	落下位置を取得													*
* Argument    :	格納位置X,Y														*
* Return      :	落下位置														*
********************************************************************************/
long
CRCellDat::GetLandY(long nX, long nY)
{
	return m_atatCellDat[nX][nY].nLandY;
}

/********************************************************************************
* FunctionName:	CheckEnStack													*
* Function    :	セル格納値の積み上げ可否を検査									*
* Argument    :	格納位置X,Y														*
*				格納値															*
* Return      :	積み上げ可否													*
********************************************************************************/
bool
CRCellDat::CheckEnStack(long nX, long nY, long nVal)
{
	long ret;				// 戻り値
	long nCntX, nCntY;		// 処理カウンタX,Y
	long nCellVal;			// セル側の格納値

	ret = FALSE;

	// セルデータのスタック情報を全消去
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			m_atatCellDat[nCntX][nCntY].nStackVal = -1;
		}
	}

	if(nY >= E_CELL_Y_NUM - 1)
	{
		m_atatCellDat[nX][nY].nStackVal = nVal;
		ret = TRUE;
	}
	else
	{
		nCellVal = m_atatCellDat[nX][nY+1].nVal;
		if(nCellVal >= 0)
		{
			if(nY >= 1)
			{
				m_atatCellDat[nX][nY].nStackVal = nVal;
				ret = TRUE;
			}
			else
			{
				if(nVal == nCellVal)
				{
					m_atatCellDat[nX][nY].nStackVal = nVal;
					ret = TRUE;
				}
			}
		}
	}

	return ret;
}

/********************************************************************************
* FunctionName:	ExecStack														*
* Function    :	セルデータの積み上げを実行										*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRCellDat::ExecStack(void)
{
	long nCntX, nCntY;		// 処理カウンタX,Y

	// セルデータサイズ分繰り返す
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			if(m_atatCellDat[nCntX][nCntY].nStackVal >= 0)
			{
				m_atatCellDat[nCntX][nCntY].nVal = m_atatCellDat[nCntX][nCntY].nStackVal;
				m_atatCellDat[nCntX][nCntY].nStackVal = -1;
			}
		}
	}
}

/********************************************************************************
* FunctionName:	CheckMerge														*
* Function    :	セル格納値のマージ可否を検査									*
* Argument    :	格納位置X,Y														*
*				格納値															*
* Return      :	マージ可否														*
*				最大マージ値(ポインタ引数返し)									*
*				マージ規模(ポインタ引数返し)									*
********************************************************************************/
bool
CRCellDat::CheckMerge(long nX, long nY, long* lpMaxMergeVal, long* lpMaxMergeDifVal)
{
	bool ret;				// 戻り値
	long nCntX, nCntY;		// 処理カウンタX,Y
	long nMaxMergeVal;		// 最大マージ値
	long nMaxMergeDifVal;	// マージ規模

	ret = FALSE;

	// セルデータのマージ情報を全消去
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			m_atatCellDat[nCntX][nCntY].nMergeX = -1;
			m_atatCellDat[nCntX][nCntY].nMergeY = -1;
			m_atatCellDat[nCntX][nCntY].nMergeVal = -1;
		}
	}

	// セルデータサイズ分繰り返す(マージ可否検査)
	for(nCntX = nX; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			// 同値セルを再帰的に探索し、連結数をカウントする
			if(CheckMergeSub(nCntX, nCntY, nCntX, nCntY) > 1)
			{
				ret = TRUE;
			}
		}
	}
	for(nCntX = nX-1; nCntX >= 0; nCntX--)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			// 同値セルを再帰的に探索し、連結数をカウントする
			if(CheckMergeSub(nCntX, nCntY, nCntX, nCntY) > 1)
			{
				ret = TRUE;
			}
		}
	}

	// マージ可否検査結果をセット
	if(lpMaxMergeVal != nullptr)
	{
		nMaxMergeVal = -1;
		nMaxMergeDifVal = -1;
		if(ret != FALSE)
		{
			// セルデータサイズ分繰り返す
			for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
			{
				for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
				{
					if(m_atatCellDat[nCntX][nCntY].nVal < m_atatCellDat[nCntX][nCntY].nMergeVal)
					{
						nMaxMergeVal = Max(nMaxMergeVal, m_atatCellDat[nCntX][nCntY].nMergeVal);
						nMaxMergeDifVal = Max(nMaxMergeDifVal, m_atatCellDat[nCntX][nCntY].nMergeVal - m_atatCellDat[nCntX][nCntY].nVal);
					}
				}
			}
		}
		*lpMaxMergeVal = nMaxMergeVal;
		*lpMaxMergeDifVal = nMaxMergeDifVal;
	}
	
	return ret;
}

// ※privateでも良い
long
CRCellDat::CheckMergeSub(long nX, long nY, long nBaseX, long nBaseY)
{
	const long F_XY[4][2] = { {-1, 0},{ 1, 0},{ 0,-1},{ 0, 1} };
	long ret;
	long n;
	long nCellX;
	long nCellY;

	ret = 0;
	if(m_atatCellDat[nX][nY].nVal != -1 &&
	   m_atatCellDat[nX][nY].nMergeX == -1 &&
	   m_atatCellDat[nX][nY].nMergeY == -1)
	{
		if(m_atatCellDat[nX][nY].nVal == m_atatCellDat[nBaseX][nBaseY].nVal)
		{
			ret = 1;
			m_atatCellDat[nX][nY].nMergeX = nBaseX;
			m_atatCellDat[nX][nY].nMergeY = nBaseY;
			for(n = 0; n < 4; n++)
			{
				nCellX = nX + F_XY[n][0];
				nCellY = nY + F_XY[n][1];
				if(NUMOK(nCellX, E_CELL_X_NUM) &&
				   NUMOK(nCellY, E_CELL_Y_NUM))
				{
					ret += CheckMergeSub(nCellX, nCellY, nBaseX, nBaseY);
				}
			}

			if(nX == nBaseX &&
			   nY == nBaseY)
			{
				m_atatCellDat[nX][nY].nMergeVal = m_atatCellDat[nX][nY].nVal + ret - 1;
			}
		}
	}

	return ret;
}

/********************************************************************************
* FunctionName:	ExecMerge														*
* Function    :	セル格納値のマージを実行										*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRCellDat::ExecMerge(void)
{
	long nCntX;	// 処理カウンタX
	long nCntY;	// 処理カウンタY

	// セルデータサイズ分繰り返す
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			m_atatCellDat[nCntX][nCntY].nVal = m_atatCellDat[nCntX][nCntY].nMergeVal;
			m_atatCellDat[nCntX][nCntY].nMergeX = -1;
			m_atatCellDat[nCntX][nCntY].nMergeY = -1;
			m_atatCellDat[nCntX][nCntY].nMergeVal = -1;
		}
	}
}

/********************************************************************************
* FunctionName:	CheckEnLand														*
* Function    :	セル格納値の着地可否を検査										*
* Argument    :	none															*
* Return      :	着地可否														*
********************************************************************************/
bool
CRCellDat::CheckEnLand(void)
{
	bool ret;				// 戻り値
	long nCntX, nCntY;		// 処理カウンタX,Y

	ret = FALSE;

	// セルデータの落下情報を全消去
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			m_atatCellDat[nCntX][nCntY].nLandY = -1;
		}
	}

	// セルデータサイズ分繰り返す
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = E_CELL_Y_NUM - 1; nCntY >= 0; nCntY--)
		{
			// 落下可能位置を計算
			if(CheckEnLandSub(nCntX, nCntY) != FALSE)
			{
				ret = TRUE;
			}
		}
	}

	return ret;
}

// ※privateでも良い
bool
CRCellDat::CheckEnLandSub(long nX, long nY)
{
	bool ret;
	long nCellY;

	ret = FALSE;
	if(m_atatCellDat[nX][nY].nVal != -1)
	{
		if(nY < E_CELL_Y_NUM - 1)
		{
			for(nCellY = nY + 1; nCellY < E_CELL_Y_NUM; nCellY++)
			{
				if(m_atatCellDat[nX][nCellY].nVal == -1)
				{
					m_atatCellDat[nX][nY].nLandY = nCellY;
					ret = TRUE;
				}
				else
				{
					if(m_atatCellDat[nX][nCellY].nLandY >= 0)
					{
						m_atatCellDat[nX][nY].nLandY = m_atatCellDat[nX][nCellY].nLandY - 1;
						ret = TRUE;
					}
					break;
				}
			}
		}
	}

	return ret;
}

/********************************************************************************
* FunctionName:	ExecLand														*
* Function    :	セル格納値の着地を実行											*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRCellDat::ExecLand(void)
{
	long nCntX, nCntY;		// 処理カウンタX,Y
	long nLandY;			// 落下位置Y

	// セルデータサイズ分繰り返す
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = E_CELL_Y_NUM-1; nCntY >= 0; nCntY--)
		{
			nLandY = m_atatCellDat[nCntX][nCntY].nLandY;
			if(nLandY != -1)
			{
				m_atatCellDat[nCntX][nLandY].nVal = m_atatCellDat[nCntX][nCntY].nVal;
				m_atatCellDat[nCntX][nCntY].nVal = -1;
			}
			m_atatCellDat[nCntX][nCntY].nLandY = -1;
		}
	}
}

/********************************************************************************
* FunctionName:	CalcScore														*
* Function    :	得点計算														*
* Argument    :	none															*
* Return      :	得点増加の有無													*
********************************************************************************/
bool
CRCellDat::CalcScore(void)
{
	bool ret;				// 戻り値
	long nCntX, nCntY;		// 処理カウンタX,Y
	long nScore;			// スコア
	long nVal;				// セル格納値

	ret = FALSE;

	nScore = 0;

	// セルデータの数値を合算する
	for(nCntX = 0; nCntX < E_CELL_X_NUM; nCntX++)
	{
		for(nCntY = 0; nCntY < E_CELL_Y_NUM; nCntY++)
		{
			nVal = m_atatCellDat[nCntX][nCntY].nVal;
			if(nVal >= 0 && nVal <= 30)
			{
				nScore += (1<<nVal);
			}
			if(nScore < 0)
			{
				// オーバーフロー
				break;
			}
		}
		if(nScore < 0)
		{
			// オーバーフロー
			break;
		}
	}

	if(nScore >= 0)
	{
		m_nScore = nScore;
		ret = TRUE;
	}
	
	return ret;
}

/********************************************************************************
* FunctionName:	GetScore														*
* Function    :	得点を取得														*
* Argument    :	none															*
* Return      :	得点															*
********************************************************************************/
long
CRCellDat::GetScore(void)
{
	return m_nScore;
}

/********************************************************************************
* FunctionName:	SetScore														*
* Function    :	得点を設定														*
* Argument    :	得点															*
* Return      :	none															*
********************************************************************************/
void
CRCellDat::SetScore(long nScore)
{
	m_nScore = nScore;
}
