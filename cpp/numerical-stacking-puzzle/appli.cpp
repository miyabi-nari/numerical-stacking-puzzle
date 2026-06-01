/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appli.cpp														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: アプリケーション層全体制御									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include <time.h>
#include "define.h"
#include "appli.h"
#include "midgui.h"
#include "midkey.h"
#include "midsnd.h"
#include "rec.h"
#include "celldat.h"
#include "nextdat.h"
#include "appbox.h"
#include "appboxset.h"
#include "appnextboxset.h"
#include "appkeyindset.h"
#include "commdlg.h"
#include "titlelblset.h"
#include "applblset.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// シーケンス
#define	E_SEQ_INIT		(0)	// 初期状態
#define	E_SEQ_TITLE		(1)	// タイトル状態
#define	E_SEQ_SERVE		(2)	// サーブ状態
#define	E_SEQ_FALL_SLOW	(3)	// 落下(遅)状態
#define	E_SEQ_FALL_FAST	(4)	// 落下(速)状態
#define	E_SEQ_STACK		(5)	// 積上状態
#define	E_SEQ_MERGE		(6)	// 結合動作状態
#define	E_SEQ_LAND		(7)	// 着地動作状態
#define	E_SEQ_BACK		(8)	// タイトルバック状態
#define	E_SEQ_EXIT		(9)	// EXIT状態

// 子プロセス
#define	E_RT_TITLE_LBL_SET		( 0)	// タイトルラベルセット
#define	E_RT_CELL_DAT			( 1)	// セルのデータ管理
#define	E_RT_NEXT_DAT			( 2)	// 次落下セルのデータ管理
#define	E_RT_APP_KEY_IND_SET	( 3)	// キー表示オブジェクト全体管理
#define	E_RT_APP_LBL_SET		( 4)	// ラベルオブジェクト全体管理
#define	E_RT_APP_BOX_SET		( 5)	// 箱オブジェクト全体管理
#define	E_RT_APP_NEXT_BOX_SET	( 6)	// 次回箱オブジェクト全体管理
#define	E_RT_FALL_APP_BOX		( 7)	// 落下箱
#define	E_RT_NUM				( 8)//子プロセスの上限数

// タイマー
#define	E_TM_SERVE			(15)
#define	E_TM_FALL_SLOW_INI	(60)
#define	E_TM_FALL_FAST		( 5)
#define	E_TM_STACK			(10)
#define	E_TM_MERGE			(30)
#define	E_TM_LAND			(30)

/********************************************************************************
*	CRAppli Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRAppli															*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
* Attention(s): このコンストラクタは使用しない(引数付きで生成する事)			*
********************************************************************************/
CRAppli::CRAppli(void)
{
	// 引数付きで作成する事
	Assert(FALSE);
}

/********************************************************************************
* FunctionName:	CRAppli															*
* Function    :	コンストラクタ													*
* Argument(s) :	hWnd=ウィンドウハンドル											*
********************************************************************************/
CRAppli::CRAppli(HWND hWnd)
{
	//-----------------------------------------------------------------------
	// 引数チェック
	if(hWnd == nullptr)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// 初期化処理
	// シーケンス設定（関数ポインタ配列作成）
	m_lpSeq[E_SEQ_INIT ] = &CRAppli::SeqInit;	// 初期状態
	m_lpSeq[E_SEQ_TITLE] = &CRAppli::SeqTitle;	// タイトル状態
	m_lpSeq[E_SEQ_SERVE] = &CRAppli::SeqServe;	// サーブ状態
	m_lpSeq[E_SEQ_FALL_SLOW] = &CRAppli::SeqFallSlow;	// 落下(遅)状態
	m_lpSeq[E_SEQ_FALL_FAST] = &CRAppli::SeqFallFast;	// 落下(速)状態
	m_lpSeq[E_SEQ_STACK ] = &CRAppli::SeqStack;	// 積上状態
	m_lpSeq[E_SEQ_MERGE ] = &CRAppli::SeqMerge;	// 結合動作状態
	m_lpSeq[E_SEQ_LAND ] = &CRAppli::SeqLand;	// 着地動作状態
	m_lpSeq[E_SEQ_BACK ] = &CRAppli::SeqBack;	// タイトルバック状態
	m_lpSeq[E_SEQ_EXIT ] = &CRAppli::SeqExit;	// EXIT状態
	// 子プロセス
	if(SetChildNum(E_RT_NUM) == FALSE)
	{
		Assert(FALSE);
		return;
	}

	//-----------------------------------------------------------------------
	// バッファクリア
	memset(&(m_anMergeTime[0]), -1, sizeof(m_anMergeTime));
	memset(&(m_anMergeVal[0]), -1, sizeof(m_anMergeVal));

	//-----------------------------------------------------------------------
	// 初期値設定
	m_hAppWnd = hWnd;		// ウィンドウハンドル
	m_tmSeq = 0;			// シーケンスタイマー <- 0
	m_nElapsedTime = -1;	// 経過時間 <- -1
	m_bTitleBack = FALSE;	// タイトルバック指令
	m_bExit = FALSE;		// EXIT指令
	m_nSeq = E_SEQ_INIT;	// 初期シーケンス=初期状態

	// 初期化正常終了
	this->CRoutine::InitOK();

	Debuglog((LPWSTR)(L"CRAppli"));

	return;
}

/********************************************************************************
* FunctionName:	~CRAppli														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRAppli::~CRAppli(void)
{
	Debuglog((LPWSTR)(L"~CRAppli"));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	CreatePzlChild													*
* Function    :	パズル系の子プロセスの生成										*
* Argument(s) :	none															*
* Return      :	FALSE=生成失敗													*
********************************************************************************/
bool
CRAppli::CreatePzlChild(void)
{
	// セルデータ管理ルーチン起動
	NEW(m_lplpRtChild[E_RT_CELL_DAT], CRCellDat());
	if(m_lplpRtChild[E_RT_CELL_DAT] == nullptr)
	{
		return FALSE;
	}

	// 次落下セルデータ管理ルーチン起動
	NEW(m_lplpRtChild[E_RT_NEXT_DAT], CRNextDat());
	if(m_lplpRtChild[E_RT_NEXT_DAT] == nullptr)
	{
		return FALSE;
	}

	// 箱オブジェクト全体管理の生成
	NEW(m_lplpRtChild[E_RT_APP_BOX_SET], CRAppBoxSet());
	if(m_lplpRtChild[E_RT_APP_BOX_SET] == nullptr)
	{
		return FALSE;
	}

	// 次回箱オブジェクト全体管理の生成
	NEW(m_lplpRtChild[E_RT_APP_NEXT_BOX_SET], CRAppNextBoxSet());
	if(m_lplpRtChild[E_RT_APP_NEXT_BOX_SET] == nullptr)
	{
		return FALSE;
	}

	// キー表示オブジェクト全体管理の生成
	NEW(m_lplpRtChild[E_RT_APP_KEY_IND_SET], CRAppKeyIndSet());
	if(m_lplpRtChild[E_RT_APP_KEY_IND_SET] == nullptr)
	{
		return FALSE;
	}

	// ラベル全体オブジェクトの生成
	NEW(m_lplpRtChild[E_RT_APP_LBL_SET], CRAppLblSet());
	if(m_lplpRtChild[E_RT_APP_LBL_SET] == nullptr)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	IsPzlChildAlive													*
* Function    :	子プロセスの生存判定											*
* Argument(s) :	none															*
* Return      :	TRUE=すべて生存													*
********************************************************************************/
bool
CRAppli::IsPzlChildAlive(void)
{
	// セルデータ管理ルーチン
	if(m_lplpRtChild[E_RT_CELL_DAT] == nullptr)
	{
		return FALSE;
	}

	// 次落下セルデータ管理ルーチン
	if(m_lplpRtChild[E_RT_NEXT_DAT] == nullptr)
	{
		return FALSE;
	}

	// 箱オブジェクト全体管理
	if(m_lplpRtChild[E_RT_APP_BOX_SET] == nullptr)
	{
		return FALSE;
	}

	// 次回箱オブジェクト全体管理
	if(m_lplpRtChild[E_RT_APP_NEXT_BOX_SET] == nullptr)
	{
		return FALSE;
	}

	// キー表示オブジェクト全体管理
	if(m_lplpRtChild[E_RT_APP_KEY_IND_SET] == nullptr)
	{
		return FALSE;
	}

	// ラベル全体管理オブジェクト
	if(m_lplpRtChild[E_RT_APP_LBL_SET] == nullptr)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	InitPzlData														*
* Function    :	パズル系のデータの初期設定										*
* Argument(s) :	none															*
* Return      :	FALSE=設定失敗													*
********************************************************************************/
bool
CRAppli::InitPzlData(void)
{
	CRNextDat* lpNextDat;				// 次落下箱管理インスタンス
	CRAppLblSet* lpAppLblSet;			// ラベル全体オブジェクト
	long nNextVal;						// 次回箱表示値

	// 次落下箱管理インスタンスを取得する
	// (パズル系の子プロセスの生成時に生成される)
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	// ラベル全体オブジェクト
	// (パズル系の子プロセスの生成時に生成される)
	lpAppLblSet = ((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET]);
	if(lpAppLblSet == nullptr)
	{
		return FALSE;
	}

	// スコア表示のリセット
	lpAppLblSet->SetScoreVal(0);
	// 経過時間のリセット
	m_nElapsedTime = 0;
	// 落下開始位置の初期値を設定
	m_nFallBoxX = 0;
	m_nFallBoxY = 0;
	// 落下箱の値を設定
	nNextVal = lpNextDat->ExecPush();
	m_nFallBoxVal = nNextVal;
	// 次回箱の表示更新
	m_tmFall = E_TM_FALL_SLOW_INI;

	return TRUE;
}

/********************************************************************************
* FunctionName:	DeletePzlChild													*
* Function    :	パズル系の子プロセスの破棄										*
* Argument(s) :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppli::DeletePzlChild(void)
{
	// セルデータ管理ルーチン
	FREE(m_lplpRtChild[E_RT_CELL_DAT]);
	// 次落下セルデータ管理ルーチン
	FREE(m_lplpRtChild[E_RT_NEXT_DAT]);
	// 箱オブジェクト全体管理
	FREE(m_lplpRtChild[E_RT_APP_BOX_SET]);
	// 次回箱オブジェクト全体管理
	FREE(m_lplpRtChild[E_RT_APP_NEXT_BOX_SET]);
	// キー表示オブジェクト全体管理
	FREE(m_lplpRtChild[E_RT_APP_KEY_IND_SET]);
	// ラベルオブジェクト全体管理
	FREE(m_lplpRtChild[E_RT_APP_LBL_SET]);
	// 落下箱
	FREE(m_lplpRtChild[E_RT_FALL_APP_BOX]);

	return;
}

/********************************************************************************
* FunctionName:	UpdateElapsedTime												*
* Function    :	経過時間の更新処理												*
* Argument(s) :	none															*
* Return      :	FALSE=更新失敗													*
********************************************************************************/
bool
CRAppli::UpdateElapsedTime(void)
{
	if(m_nElapsedTime == 0)
	{
		if(m_nSeq < E_SEQ_FALL_SLOW)
		{
		}
		else if(m_nSeq == E_SEQ_FALL_SLOW)
		{
			if(m_nFallBoxY == 0)
			{
			}
			else
			{
				m_nElapsedTime = 1;
			}
		}
		else
		{
			m_nElapsedTime = 1;
		}
	}
	else
	{
		m_nElapsedTime++;
	}

	((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET])->SetElapsedTimeVal(m_nElapsedTime);

	return TRUE;
}

/********************************************************************************
* FunctionName:	UpdateMergeTime													*
* Function    :	マージ時間の更新処理											*
* Argument(s) :	none															*
* Return      :	FALSE=更新失敗													*
********************************************************************************/
bool
CRAppli::UpdateMergeTime(long nMaxMergeVal)
{
	long n;
	long nMaxMergeTime;

	if(nMaxMergeVal >= 0)
	{
		for(n = 0; n < 5; n++)
		{
			if(nMaxMergeVal == m_anMergeVal[n])
			{
				break;
			}
		}
		if(n >= 5)
		{
			// 同じ値は無い -> 更新を行う
			nMaxMergeTime = m_nElapsedTime;
			for(n = 0; n < 5; n++)
			{
				if(nMaxMergeVal < m_anMergeVal[n])
				{
				}
				else
				{
					Swap(nMaxMergeVal, m_anMergeVal[n]);
					Swap(nMaxMergeTime, m_anMergeTime[n]);
				}
			}
		}
	}

	for(n = 0; n < 5; n++)
	{
		((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET])->SetMergeTimeVal(n, m_anMergeTime[n], m_anMergeVal[n]);
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	OpenLoadFile													*
* Function    :	読出ファイルの選択処理(Dialog)									*
* Argument(s) :	none															*
* Return      :	FALSE=選択キャンセル											*
********************************************************************************/
bool
CRAppli::OpenLoadFile(CWMStr* lpFilename)
{
	bool ret;			// 戻り値
	CWMStr wmsFilename;	// 選択したファイル名
	OPENFILENAME OFN;	// ファイルオープン構造体

	ret = FALSE;

	wmsFilename.NewW(MAX_PATH*2);

	::ZeroMemory(&OFN,sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME); 
	OFN.hwndOwner = m_hAppWnd;
	OFN.lpstrFilter = L"テキストファイル(*.txt)\0*.txt\0CSVファイル(*.csv)\0*.csv\0すべてのファイル(*.*)\0*.*\0\0";
	OFN.lpstrFile = wmsFilename.PtW();
	OFN.nMaxFile = MAX_PATH*2;
	OFN.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if(::GetOpenFileName(&OFN) != FALSE)
	{
		lpFilename->SetW(wmsFilename.PtW());

		ret = TRUE;
	}

	return ret;
}

/********************************************************************************
* FunctionName:	SaveFileOp														*
* Function    :	ファイル保存オペレーション										*
* Argument(s) :	none															*
* Return      :	FALSE=保存失敗													*
********************************************************************************/
bool
CRAppli::SaveFileOp(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	CRNextDat* lpNextDat;	// 次落下箱管理インスタンス
	CRec rec;				// 記録データ読出保存ライブラリ
	CWMStr wmsKey;			// キー文字列
	CWMStr wmsFilename;		// 保存ファイルネーム
	long nDat;				// 一時データ
	time_t ltime;			// 時刻用変数
	struct tm today;		// 日付用変数

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// パズル系データの保存
	// 識別情報
	wmsKey.SetW((LPWSTR)E_APP_SAVE_ID_KEY);
	nDat = E_APP_SAVE_ID_NUM;
	rec.SetDat(&wmsKey, &nDat, 1);
	// セルデータ
	wmsKey.SetW((LPWSTR)L"CellData");
	rec.SetDat(&wmsKey,
	                 (long*)(lpCellDat->GetCellDatAdr()),
	                 (sizeof(T_LD_CELLDAT) / sizeof(long)) * E_CELL_X_NUM * E_CELL_Y_NUM);
	// スコア
	wmsKey.SetW((LPWSTR)L"Score");
	nDat = lpCellDat->GetScore();
	rec.SetDat(&wmsKey, &nDat, 1);

	// 次回箱データ
	wmsKey.SetW((LPWSTR)L"NextData");
	rec.SetDat(&wmsKey,
	                 (long*)(lpNextDat->GetNextBoxValAdr()),
	                 E_NEXT_BOX_NUM);
	// 次回箱生成数
	wmsKey.SetW((LPWSTR)L"NextBoxCnt");
	nDat = lpNextDat->GetNextBoxCnt();
	rec.SetDat(&wmsKey, &nDat, 1);
	// 箱の値ランダムタイマ
	wmsKey.SetW((LPWSTR)L"BoxValTime");
	nDat = lpNextDat->GetBoxValTm();
	rec.SetDat(&wmsKey, &nDat, 1);

	// 落下中の箱
	wmsKey.SetW((LPWSTR)L"FallX");
	rec.SetDat(&wmsKey, &m_nFallBoxX, 1);
	wmsKey.SetW((LPWSTR)L"FallY");
	rec.SetDat(&wmsKey, &m_nFallBoxY, 1);
	wmsKey.SetW((LPWSTR)L"FallVal");
	rec.SetDat(&wmsKey, &m_nFallBoxVal, 1);

	// 経過時間
	wmsKey.SetW((LPWSTR)L"ElapsedTime");
	rec.SetDat(&wmsKey, &m_nElapsedTime, 1);

	// マージ時間とマージ値
	wmsKey.SetW((LPWSTR)L"MergeTime");
	rec.SetDat(&wmsKey, &m_anMergeTime[0], sizeof(m_anMergeTime) / sizeof(long));
	wmsKey.SetW((LPWSTR)L"MergeVal");
	rec.SetDat(&wmsKey, &m_anMergeVal[0], sizeof(m_anMergeVal) / sizeof(long));

	//-----------------------------------------------------------------------
	// 現在時刻からログファイル名を決定
	// MSDNサンプルソースを拝借。
	wmsFilename.NewW(128+128);
	time(&ltime);
	(void)localtime_s(&today, &ltime);
	wcsftime(wmsFilename.PtW(), 128, L"save-%Y%m%d-%H%M%S.txt", &today);
	
	if(rec.SaveFile(&wmsFilename) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************
* FunctionName:	LoadFileOp														*
* Function    :	ファイル読出オペレーション										*
* Argument(s) :	none															*
* Return      :	FALSE=読出失敗													*
********************************************************************************/
bool
CRAppli::LoadFileOp(CWMStr* lpFilename)
{
	CRCellDat* lpCellDat;		// セルデータ管理インスタンス
	CRNextDat* lpNextDat;		// 次落下箱管理インスタンス
	CRAppLblSet* lpAppLblSet;	// ラベル全体オブジェクト
	CRec rec;					// 記録データ読出保存ライブラリ
	CWMStr wmsKey;				// キー文字列
	long nDat;					// 一時データ
	long n;						// 一時データ

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ラベル全体オブジェクト
	// (パズル系の子プロセスの生成時に生成される)
	lpAppLblSet = ((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET]);
	if(lpAppLblSet == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ファイル読み出し
	if(rec.LoadFile(lpFilename) == FALSE)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// パズル系データの読出
	// 識別情報
	wmsKey.SetW((LPWSTR)E_APP_SAVE_ID_KEY);
	rec.GetDat(&wmsKey, &nDat, 1);
	if(nDat != E_APP_SAVE_ID_NUM)
	{
		return FALSE;
	}
	// セルデータ
	wmsKey.SetW((LPWSTR)L"CellData");
	if(rec.GetDat(&wmsKey,
	                    (long*)(lpCellDat->GetCellDatAdr()),
	                    (sizeof(T_LD_CELLDAT) / sizeof(long)) * E_CELL_X_NUM * E_CELL_Y_NUM) == FALSE)
	{
		return FALSE;
	}
	// スコア
	wmsKey.SetW((LPWSTR)L"Score");
	if(rec.GetDat(&wmsKey, &nDat, 1) == FALSE)
	{
		return FALSE;
	}
	lpCellDat->SetScore(nDat);
	// 表示にも反映させる
	lpAppLblSet->SetScoreVal(nDat);

	// 次回箱データ
	wmsKey.SetW((LPWSTR)L"NextData");
	if(rec.GetDat(&wmsKey,
	                    (long*)(lpNextDat->GetNextBoxValAdr()),
	                    E_NEXT_BOX_NUM) == FALSE)
	{
		return FALSE;
	}
	// 次回箱生成数
	wmsKey.SetW((LPWSTR)L"NextBoxCnt");
	if(rec.GetDat(&wmsKey, &nDat, 1) == FALSE)
	{
		return FALSE;
	}
	lpNextDat->SetNextBoxCnt(nDat);
	// 箱の値ランダムタイマ
	wmsKey.SetW((LPWSTR)L"BoxValTime");
	if(rec.GetDat(&wmsKey, &nDat, 1) == FALSE)
	{
		return FALSE;
	}
	lpNextDat->SetBoxValTm(nDat);

	// 落下中の箱
	wmsKey.SetW((LPWSTR)L"FallX");
	if(rec.GetDat(&wmsKey, &m_nFallBoxX, 1) == FALSE)
	{
		return FALSE;
	}
	wmsKey.SetW((LPWSTR)L"FallY");
	if(rec.GetDat(&wmsKey, &m_nFallBoxY, 1) == FALSE)
	{
		return FALSE;
	}
	wmsKey.SetW((LPWSTR)L"FallVal");
	if(rec.GetDat(&wmsKey, &m_nFallBoxVal, 1) == FALSE)
	{
		return FALSE;
	}

	// 経過時間
	wmsKey.SetW((LPWSTR)L"ElapsedTime");
	if(rec.GetDat(&wmsKey, &m_nElapsedTime, 1) == FALSE)
	{
		return FALSE;
	}
	// 経過時間の更新処理
	// 表示にも反映させる
	lpAppLblSet->SetElapsedTimeVal(m_nElapsedTime);

	// マージ時間とマージ値
	wmsKey.SetW((LPWSTR)L"MergeTime");
	if(rec.GetDat(&wmsKey, &m_anMergeTime[0], sizeof(m_anMergeTime) / sizeof(long)) == FALSE)
	{
		return FALSE;
	}
	wmsKey.SetW((LPWSTR)L"MergeVal");
	if(rec.GetDat(&wmsKey, &m_anMergeVal[0], sizeof(m_anMergeVal) / sizeof(long)) == FALSE)
	{
		return FALSE;
	}
	// マージ時間を更新する
	for(n = 0; n < 5; n++)
	{
		lpAppLblSet->SetMergeTimeVal(n, m_anMergeTime[n], m_anMergeVal[n]);
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
CRAppli::SeqInit(void)
{
	// タイトル用ラベルオブジェクト全体管理の生成
	NEW(m_lplpRtChild[E_RT_TITLE_LBL_SET], CRTitleLblSet());
	if(m_lplpRtChild[E_RT_TITLE_LBL_SET] == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// EXIT指令ある場合は終了
	if(m_bExit != FALSE)
	{
		m_bExit = FALSE;
		m_nSeq = E_SEQ_EXIT;
		// 以降の処理なし
		return TRUE;
	}

	// シーケンス -> タイトル状態
	m_nSeq = E_SEQ_TITLE;

	return TRUE;
}

/********************************************************************************
* FunctionName:	SeqTitle														*
* Function    :	シーケンス処理:タイトル状態										*
* Argument(s) :	none															*
* Return      :	FALSE=処理終了（オブジェクト破棄）								*
********************************************************************************/
bool
CRAppli::SeqTitle(void)
{
	CRMidKey* lpMidKey;	// Key入力インスタンス
	CRMidSnd* lpMidSnd;	// 音出力インスタンス
	CWMStr wmsFilename;	// 選択したファイル名
	bool bStart;		// 開始FG
	bool bLoadStart;	// セーブデータ読出開始FG

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(m_lplpRtChild[E_RT_TITLE_LBL_SET] == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーは終了
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bExit = TRUE;
	}

	//-----------------------------------------------------------------------
	// EXIT指令ある場合は終了
	if(m_bExit != FALSE)
	{
		m_bExit = FALSE;
		m_nSeq = E_SEQ_EXIT;
		// 以降の処理なし
		return TRUE;
	}

	//-----------------------------------------------------------------------
	// キー入力を判定して開始する
	bStart = FALSE;
	bLoadStart = FALSE;
	if(lpMidKey->GetKeyDown()->qwerty[0])
	{
		// [Z]入力あり -> 開始(データ読出無し)
		bStart = TRUE;
	}
	else if(lpMidKey->GetKeyDown()->qwerty[1])
	{
		// [X]入力あり -> 開始(データ読出有り)
		if(OpenLoadFile(&wmsFilename) != FALSE)
		{
			bStart = TRUE;
			bLoadStart = TRUE;
		}
	}
	else if(lpMidKey->GetKeyDown()->qwerty[2])
	{
		// [C]入力あり -> 音声オフ
		lpMidSnd->DisablePlay(TRUE);
	}
	else if(lpMidKey->GetKeyDown()->qwerty[3])
	{
		// [V]入力あり -> 音声オン
		lpMidSnd->DisablePlay(FALSE);
		lpMidSnd->SetSnd(E_MIDSND_MERGE_3);
	}

	if(bStart != FALSE)
	{
		// タイトル用ラベルオブジェクト全体管理の破棄
		FREE(m_lplpRtChild[E_RT_TITLE_LBL_SET]);
		// パズル系の子プロセスの生成
		if(CreatePzlChild() == FALSE)
		{
			Assert(FALSE);
			return FALSE;
		}

		// パズル系のデータの初期設定
		if(InitPzlData() == FALSE)
		{
			Assert(FALSE);
			return FALSE;
		}

		if(bLoadStart != FALSE)
		{
			// ファイル読出オペレーション
			if(LoadFileOp(&wmsFilename) == FALSE)
			{
				bStart = FALSE;
			}
		}

		if(bStart != FALSE)
		{
			// 次回箱の表示更新
			((CRAppNextBoxSet*)m_lplpRtChild[E_RT_APP_NEXT_BOX_SET])->SetStatPush();
			// 箱生成
			NEW(m_lplpRtChild[E_RT_FALL_APP_BOX], CRAppBox(m_nFallBoxVal, 1200, 0));
			if(m_lplpRtChild[E_RT_FALL_APP_BOX] == nullptr)
			{
				Assert(FALSE);
				return FALSE;
			}
			((CRAppBox*)m_lplpRtChild[E_RT_FALL_APP_BOX])->SetStatServe(m_nFallBoxX * 100, m_nFallBoxY * 100);
	
			m_bTitleBack = FALSE;
			// シーケンス -> サーブ状態
			m_tmSeq = E_TM_SERVE;
			m_nSeq = E_SEQ_SERVE;
		}
		else
		{
			// パズル系の子プロセスの破棄
			DeletePzlChild();
			
			// タイトル用ラベルオブジェクト全体管理の生成
			NEW(m_lplpRtChild[E_RT_TITLE_LBL_SET], CRTitleLblSet());
			if(m_lplpRtChild[E_RT_TITLE_LBL_SET] == nullptr)
			{
				return FALSE;
			}
		
			// シーケンス -> タイトル状態
			m_nSeq = E_SEQ_TITLE;
		}
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqServe														*
* Purpose	: シーケンス＝サーブ状態											*
********************************************************************************/
bool
CRAppli::SeqServe(void)
{
	CRMidKey* lpMidKey;	// Key入力インスタンス

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 経過時間の更新処理
	if(UpdateElapsedTime() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーはタイトルバック
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bTitleBack = TRUE;
	}

	//-----------------------------------------------------------------------
	// タイトルバック操作処理
	if(m_bTitleBack != FALSE || m_bExit != FALSE)
	{
		m_bTitleBack = FALSE;
		// シーケンス -> タイトルバック状態
		m_nSeq = E_SEQ_BACK;
		// 以降の処理なし
		return TRUE;
	}
	
	//-----------------------------------------------------------------------
	// 一定時間経過後に落下開始
	if(m_tmSeq == 0)
	{
		m_tmSeq = E_TM_FALL_SLOW_INI;
		// シーケンス -> 落下(遅)状態
		m_nSeq = E_SEQ_FALL_SLOW;
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqFallSlow													*
* Purpose	: シーケンス＝落下(遅)状態											*
********************************************************************************/
bool
CRAppli::SeqFallSlow(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	CRMidKey* lpMidKey;		// Key入力インスタンス
	CRMidSnd* lpMidSnd;		// 音出力インスタンス
	long nCellX;			// セル位置
	long nPosX, nPosY;		// 箱位置X,Y

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 経過時間の更新処理
	if(UpdateElapsedTime() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーはタイトルバック
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bTitleBack = TRUE;
	}

	//-----------------------------------------------------------------------
	// タイトルバック操作処理
	if(m_bTitleBack != FALSE || m_bExit != FALSE)
	{
		m_bTitleBack = FALSE;
		// シーケンス -> タイトルバック状態
		m_nSeq = E_SEQ_BACK;
		// 以降の処理なし
		return TRUE;
	}
	
	//-----------------------------------------------------------------------
	// 箱の降下処理
	nPosX = m_nFallBoxX * 100;
	if(m_nFallBoxY == 0)
	{
		nPosY = 0;
	}
	else
	{
		nPosY = m_nFallBoxY * 100;
		nPosY -= m_tmSeq * 100 / m_tmFall;
	}
	((CRAppBox*)m_lplpRtChild[E_RT_FALL_APP_BOX])->SetPos(nPosX, nPosY);
	// 1セル降下毎にスタック判定
	if(m_tmSeq == 0)
	{
		if(lpCellDat->CheckEnStack(m_nFallBoxX, m_nFallBoxY, m_nFallBoxVal) != FALSE)
		{
			// スタック音を再生する
			lpMidSnd->SetSnd(E_MIDSND_STACK);
			// 箱全体処理状態を積上動作に設定
			((CRAppBoxSet*)m_lplpRtChild[E_RT_APP_BOX_SET])->SetStatStack();
			m_tmSeq = E_TM_STACK;
			// シーケンス -> 積上状態
			m_nSeq = E_SEQ_STACK;
		}
		else
		{
			// スタック不可
			if(m_nFallBoxY == 0 && lpCellDat->GetVal(m_nFallBoxX, m_nFallBoxY+1) >= 0)
			{
			}
			else
			{
				m_nFallBoxY++;
				m_tmSeq = m_tmFall;
			}
		}
	}

	//-----------------------------------------------------------------------
	// キー操作処理
	if(m_nSeq != E_SEQ_STACK)
	{
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			if(lpMidKey->GetKeyDown()->qwerty[nCellX])
			{
				// qwertyキー入力あり
				if(lpCellDat->GetVal(nCellX, m_nFallBoxY) < 0)
				{
					m_nFallBoxX = nCellX;
					m_tmSeq = (m_tmSeq * E_TM_FALL_FAST + m_tmFall / 2) / m_tmFall;
					// シーケンス -> 落下(速)状態
					m_nSeq = E_SEQ_FALL_FAST;
					break;
				}
			}
		}
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqFallFast													*
* Purpose	: シーケンス＝落下(速)状態											*
********************************************************************************/
bool
CRAppli::SeqFallFast(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	CRMidKey* lpMidKey;		// Key入力インスタンス
	CRMidSnd* lpMidSnd;		// 音出力インスタンス
	long nCellX;			// セル位置
	long nPosX, nPosY;		// 箱位置X,Y

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 経過時間の更新処理
	if(UpdateElapsedTime() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーはタイトルバック
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bTitleBack = TRUE;
	}

	//-----------------------------------------------------------------------
	// タイトルバック操作処理
	if(m_bTitleBack != FALSE || m_bExit != FALSE)
	{
		m_bTitleBack = FALSE;
		// シーケンス -> タイトルバック状態
		m_nSeq = E_SEQ_BACK;
		// 以降の処理なし
		return TRUE;
	}
	
	//-----------------------------------------------------------------------
	// 箱の降下処理
	nPosX = m_nFallBoxX * 100;
	if(m_nFallBoxY == 0)
	{
		nPosY = 0;
	}
	else
	{
		nPosY = m_nFallBoxY * 100;
		nPosY -= m_tmSeq * 100 / E_TM_FALL_FAST;
	}
	((CRAppBox*)m_lplpRtChild[E_RT_FALL_APP_BOX])->SetPos(nPosX, nPosY);
	// 1セル降下毎にスタック判定
	if(m_tmSeq == 0)
	{
		if(lpCellDat->CheckEnStack(m_nFallBoxX, m_nFallBoxY, m_nFallBoxVal) != FALSE)
		{
			// スタック音を再生する
			lpMidSnd->SetSnd(E_MIDSND_STACK);
			// 箱全体処理状態を積上動作に設定
			((CRAppBoxSet*)m_lplpRtChild[E_RT_APP_BOX_SET])->SetStatStack();
			m_tmSeq = E_TM_STACK;
			// シーケンス -> 積上状態
			m_nSeq = E_SEQ_STACK;
		}
		else
		{
			// スタック不可
			if(m_nFallBoxY == 0 && lpCellDat->GetVal(m_nFallBoxX, m_nFallBoxY+1) >= 0)
			{
			}
			else
			{
				m_nFallBoxY++;
				m_tmSeq = E_TM_FALL_FAST;
			}
		}
	}

	//-----------------------------------------------------------------------
	// キー操作処理
	if(m_nSeq != E_SEQ_STACK)
	{
		for(nCellX = 0; nCellX < E_CELL_X_NUM; nCellX++)
		{
			if(lpMidKey->GetKeyDown()->qwerty[nCellX])
			{
				// qwertyキー入力あり
				if(lpCellDat->GetVal(nCellX, m_nFallBoxY) < 0)
				{
					m_nFallBoxX = nCellX;
					break;
				}
			}
		}
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqStack														*
* Purpose	: シーケンス＝積上状態												*
********************************************************************************/
bool
CRAppli::SeqStack(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	CRNextDat* lpNextDat;	// 次落下箱管理インスタンス
	CRMidKey* lpMidKey;		// Key入力インスタンス
	CRMidSnd* lpMidSnd;		// 音出力インスタンス
	long nScore;			// スコア
	long nNextVal;			// 次回箱の値
	long nMaxMergeVal;		// マージ結果(値)
	long nMaxMergeDifVal;	// マージ結果(規模)

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 経過時間の更新処理
	if(UpdateElapsedTime() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーはタイトルバック
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bTitleBack = TRUE;
	}

	if(m_tmSeq == 0)
	{
		// 格納値の積み上げ
		lpCellDat->ExecStack();
		// 得点計算
		if(lpCellDat->CalcScore() != FALSE)
		{
			nScore = lpCellDat->GetScore();
			((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET])->SetScoreVal(nScore);
		}
		// 落下箱を破棄
		FREE(m_lplpRtChild[E_RT_FALL_APP_BOX]);
		// 格納値のマージ可否を検査
		if(lpCellDat->CheckMerge(m_nFallBoxX, m_nFallBoxY, &nMaxMergeVal, &nMaxMergeDifVal) != FALSE)
		{
			// マージ時間を更新する
			if(UpdateMergeTime(nMaxMergeVal) == FALSE)
			{
				return FALSE;
			}
			// マージ音を再生する
			if(nMaxMergeDifVal <= 1)
			{
				lpMidSnd->SetSnd(E_MIDSND_MERGE_1);
			}
			else if(nMaxMergeDifVal == 2)
			{
				lpMidSnd->SetSnd(E_MIDSND_MERGE_2);
			}
			else
			{
				lpMidSnd->SetSnd(E_MIDSND_MERGE_3);
			}
			// 箱全体処理状態を結合動作に設定
			((CRAppBoxSet*)m_lplpRtChild[E_RT_APP_BOX_SET])->SetStatMerge();
			m_tmSeq = E_TM_MERGE;
			// シーケンス -> 結合動作状態
			m_nSeq = E_SEQ_MERGE;
		}
		else
		{
			// 落下開始位置の初期値を設定
	//		m_nFallBoxX = 0;
			m_nFallBoxY = 0;
			nNextVal = lpNextDat->ExecPush();
			m_nFallBoxVal = nNextVal;
			((CRAppNextBoxSet*)m_lplpRtChild[E_RT_APP_NEXT_BOX_SET])->SetStatPush();
			// 箱生成
			NEW(m_lplpRtChild[E_RT_FALL_APP_BOX], CRAppBox(m_nFallBoxVal, 1200, 0));
			((CRAppBox*)m_lplpRtChild[E_RT_FALL_APP_BOX])->SetStatServe(m_nFallBoxX * 100, m_nFallBoxY * 100);

			// シーケンス -> サーブ状態
			m_tmSeq = E_TM_SERVE;
			m_nSeq = E_SEQ_SERVE;
		}
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqMerge														*
* Purpose	: シーケンス＝結合動作状態											*
********************************************************************************/
bool
CRAppli::SeqMerge(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	CRNextDat* lpNextDat;	// 次落下箱管理インスタンス
	CRMidKey* lpMidKey;		// Key入力インスタンス
	CRMidSnd* lpMidSnd;		// 音出力インスタンス
	long nScore;			// スコア
	long nNextVal;			// 次回箱の値
	long nMaxMergeVal;		// マージ結果(値)
	long nMaxMergeDifVal;	// マージ結果(規模)

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 経過時間の更新処理
	if(UpdateElapsedTime() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーはタイトルバック
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bTitleBack = TRUE;
	}

	if(m_tmSeq == 0)
	{
		// 格納値のマージ
		lpCellDat->ExecMerge();
		// 得点計算
		if(lpCellDat->CalcScore() != FALSE)
		{
			nScore = lpCellDat->GetScore();
			((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET])->SetScoreVal(nScore);
		}
		// 格納値の着地可否を検査
		if(lpCellDat->CheckEnLand() != FALSE)
		{
			// 箱全体処理状態を着地動作に設定
			((CRAppBoxSet*)m_lplpRtChild[E_RT_APP_BOX_SET])->SetStatLand();
			m_tmSeq = E_TM_LAND;
			// シーケンス -> 結合動作状態
			m_nSeq = E_SEQ_LAND;
		}
		else
		{
			// 格納値のマージ可否を検査
			if(lpCellDat->CheckMerge(m_nFallBoxX, m_nFallBoxY, &nMaxMergeVal, &nMaxMergeDifVal) != FALSE)
			{
				// マージ時間を更新する
				if(UpdateMergeTime(nMaxMergeVal) == FALSE)
				{
					return FALSE;
				}
				// マージ音を再生する
				if(nMaxMergeDifVal <= 1)
				{
					lpMidSnd->SetSnd(E_MIDSND_MERGE_1);
				}
				else if(nMaxMergeDifVal == 2)
				{
					lpMidSnd->SetSnd(E_MIDSND_MERGE_2);
				}
				else
				{
					lpMidSnd->SetSnd(E_MIDSND_MERGE_3);
				}
				// 箱全体処理状態を結合動作に設定
				((CRAppBoxSet*)m_lplpRtChild[E_RT_APP_BOX_SET])->SetStatMerge();
				m_tmSeq = E_TM_MERGE;
				// シーケンス -> 結合動作状態
				m_nSeq = E_SEQ_MERGE;
			}
			else
			{
				// 落下開始位置の初期値を設定
		//		m_nFallBoxX = 0;
				m_nFallBoxY = 0;
				nNextVal = lpNextDat->ExecPush();
				m_nFallBoxVal = nNextVal;
				((CRAppNextBoxSet*)m_lplpRtChild[E_RT_APP_NEXT_BOX_SET])->SetStatPush();
				// 箱生成
				NEW(m_lplpRtChild[E_RT_FALL_APP_BOX], CRAppBox(m_nFallBoxVal, 1200, 0));
				((CRAppBox*)m_lplpRtChild[E_RT_FALL_APP_BOX])->SetStatServe(m_nFallBoxX * 100, m_nFallBoxY * 100);

				// シーケンス -> サーブ状態
				m_tmSeq = E_TM_SERVE;
				m_nSeq = E_SEQ_SERVE;
			}
		}
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqLand														*
* Purpose	: シーケンス＝着地動作状態											*
********************************************************************************/
bool
CRAppli::SeqLand(void)
{
	CRCellDat* lpCellDat;	// セルデータ管理インスタンス
	CRNextDat* lpNextDat;	// 次落下箱管理インスタンス
	CRMidKey* lpMidKey;		// Key入力インスタンス
	CRMidSnd* lpMidSnd;		// 音出力インスタンス
	long nScore;			// スコア
	long nNextVal;			// 次回箱の値
	long nMaxMergeVal;		// マージ結果(値)
	long nMaxMergeDifVal;	// マージ結果(規模)

	//-----------------------------------------------------------------------
	// セルデータ管理インスタンスを取得する
	lpCellDat = CRCellDat::GetInstPt();
	if(lpCellDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 次落下箱管理インスタンスを取得する
	lpNextDat = CRNextDat::GetInstPt();
	if(lpNextDat == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 音出力インスタンスを取得する
	lpMidSnd = CRMidSnd::GetInstPt();
	if(lpMidSnd == nullptr)
	{
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// 経過時間の更新処理
	if(UpdateElapsedTime() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// ESCキーはタイトルバック
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bTitleBack = TRUE;
	}

	if(m_tmSeq == 0)
	{
		// 格納値の着地
		lpCellDat->ExecLand();
		// 得点計算
		if(lpCellDat->CalcScore() != FALSE)
		{
			nScore = lpCellDat->GetScore();
			((CRAppLblSet*)m_lplpRtChild[E_RT_APP_LBL_SET])->SetScoreVal(nScore);
		}
		// 格納値のマージ可否を検査
		if(lpCellDat->CheckMerge(m_nFallBoxX, m_nFallBoxY, &nMaxMergeVal, &nMaxMergeDifVal) != FALSE)
		{
			// マージ時間を更新する
			if(UpdateMergeTime(nMaxMergeVal) == FALSE)
			{
				return FALSE;
			}
			// マージ音を再生する
			if(nMaxMergeDifVal <= 1)
			{
				lpMidSnd->SetSnd(E_MIDSND_MERGE_1);
			}
			else if(nMaxMergeDifVal == 2)
			{
				lpMidSnd->SetSnd(E_MIDSND_MERGE_2);
			}
			else
			{
				lpMidSnd->SetSnd(E_MIDSND_MERGE_3);
			}
			// 箱全体処理状態を結合動作に設定
			((CRAppBoxSet*)m_lplpRtChild[E_RT_APP_BOX_SET])->SetStatMerge();
			m_tmSeq = E_TM_MERGE;
			// シーケンス -> 結合動作状態
			m_nSeq = E_SEQ_MERGE;
		}
		else
		{
			// 落下開始位置の初期値を設定
	//		m_nFallBoxX = 0;
			m_nFallBoxY = 0;
			nNextVal = lpNextDat->ExecPush();
			m_nFallBoxVal = nNextVal;
			((CRAppNextBoxSet*)m_lplpRtChild[E_RT_APP_NEXT_BOX_SET])->SetStatPush();
			// 箱生成
			NEW(m_lplpRtChild[E_RT_FALL_APP_BOX], CRAppBox(m_nFallBoxVal, 1200, 0));
			((CRAppBox*)m_lplpRtChild[E_RT_FALL_APP_BOX])->SetStatServe(m_nFallBoxX * 100, m_nFallBoxY * 100);

			// シーケンス -> サーブ状態
			m_tmSeq = E_TM_SERVE;
			m_nSeq = E_SEQ_SERVE;
		}
	}

	return TRUE;
}

/********************************************************************************
* Function Name	: SeqBack														*
* Purpose	: シーケンス＝タイトルバック状態									*
********************************************************************************/
bool
CRAppli::SeqBack(void)
{
	CRMidKey* lpMidKey;		// Key入力インスタンス

	//-----------------------------------------------------------------------
	// 枝葉ルーチン生存していない -> 強制終了
	if(IsPzlChildAlive() == FALSE)
	{
		Assert(FALSE);
		return FALSE;
	}

	//-----------------------------------------------------------------------
	// Key入力インスタンスを取得する
	lpMidKey = CRMidKey::GetInstPt();
	if(lpMidKey == nullptr)
	{
		return FALSE;
	}

	// ファイル保存オペレーション
	if(SaveFileOp() == FALSE)
	{
		// ファイル保存失敗はあり得るのでスルー
	}

	// パズル系の子プロセスの破棄
	DeletePzlChild();
	
	//-----------------------------------------------------------------------
	// ESCキーは終了
	if(lpMidKey->GetKeyDown()->esc)
	{
		m_bExit = TRUE;
	}

	//-----------------------------------------------------------------------
	// EXIT指令ある場合は終了
	if(m_bExit != FALSE)
	{
		m_bExit = FALSE;
		m_nSeq = E_SEQ_EXIT;
		// 以降の処理なし
		return TRUE;
	}

	//-----------------------------------------------------------------------
	// タイトル用ラベルオブジェクト全体管理の生成
	NEW(m_lplpRtChild[E_RT_TITLE_LBL_SET], CRTitleLblSet());
	if(m_lplpRtChild[E_RT_TITLE_LBL_SET] == nullptr)
	{
		return FALSE;
	}

	// シーケンス -> タイトル状態
	m_nSeq = E_SEQ_TITLE;
	
	return TRUE;
}

/********************************************************************************
* Function Name	: SeqExit														*
* Purpose	: シーケンス＝EXIT状態												*
********************************************************************************/
bool
CRAppli::SeqExit(void)
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
CRAppli::Update(void)
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
* FunctionName:	SetTitleBack													*
* Function    :	タイトルバック指令												*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppli::SetTitleBack(void)
{
	m_bTitleBack = TRUE;
}

/********************************************************************************
* FunctionName:	SetExit															*
* Function    :	EXIT指令														*
* Argument    :	none															*
* Return      :	none															*
********************************************************************************/
void
CRAppli::SetExit(void)
{
	m_bExit = TRUE;
}

/********************************************************************************
* FunctionName:	IsExitSeq														*
* Function    :	シーケンス状態がEXIT状態か否か取得								*
* Argument    :	none															*
* Return      :	シーケンス状態(EXIT状態=TRUE)									*
********************************************************************************/
bool
CRAppli::IsExitSeq(void)
{
	bool ret;
	ret = FALSE;
	
	if(m_nSeq == E_SEQ_EXIT)
	{
		ret = TRUE;
	}

	return ret;
}

