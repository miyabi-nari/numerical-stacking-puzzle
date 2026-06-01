/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: rec.cpp														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 記録データ読出保存ライブラリ									*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "rec.h"
#include "wmstr.h"
#include "fileio.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/

/********************************************************************************
*	CRec Public Constructor(s)/Destructor										*
********************************************************************************/

/********************************************************************************
* FunctionName:	CRec															*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRec::CRec(void)
{
	//-----------------------------------------------------------------------
	// バッファクリア

	//-----------------------------------------------------------------------
	// 初期値設定

	Debuglog((LPWSTR)(L"CRec"));
}

/********************************************************************************
* FunctionName:	~CRec															*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CRec::~CRec(void)
{
	Debuglog((LPWSTR)(L"~CRec"));
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	SaveFileOp														*
* Function    :	ファイル保存オペレーション										*
* Argument(s) :	none															*
* Return      :	FALSE=保存失敗													*
********************************************************************************/
bool
CRec::SaveFileOp(CWMStr* lpFilename)
{
	bool ret;				// 戻り値
	CFileIO fioSaveFile;	// 保存ファイルインスタンス
	CWMStr wmsSaveDat;		// 保存データ文字列
	long nCnt;				// 処理カウンタ

	ret = FALSE;

	// 保存ファイルオープン
	if(fioSaveFile.Open(lpFilename->PtW(), FOPEN_WRITE))
	{
		// 保存データ文字列バッファ確保
		wmsSaveDat.NewW(E_REC_DAT_NUM * 20);
		// データを結合する。データサイズ分繰り返す
		for(nCnt = 0; nCnt < E_REC_DAT_NUM; nCnt++)
		{
			if(m_atRecDat[nCnt].wmsKey.CmpW(nullptr) != 0)
			{
				wmsSaveDat.AddW((LPWSTR)L"key:");
				wmsSaveDat.AddW(m_atRecDat[nCnt].wmsKey.PtW());
				wmsSaveDat.AddChrW('\n');
				wmsSaveDat.AddW((LPWSTR)L"size:");
				wmsSaveDat.AddDigW(m_atRecDat[nCnt].nDatSize, -1);
				wmsSaveDat.AddChrW('\n');
				wmsSaveDat.AddW(m_atRecDat[nCnt].wmsDat.PtW());
				wmsSaveDat.AddChrW('\n');
			}
		}
		wmsSaveDat.AddW((LPWSTR)L"end:\n");
		// マルチバイト文字に変換する
		wmsSaveDat.MirrorWtoM();
		// 保存ファイル書き込み、クローズ
		if(fioSaveFile.Write(wmsSaveDat.PtM(), (long)wmsSaveDat.LenM()) != FALSE)
		{
			ret = TRUE;
		}
		fioSaveFile.Close();
	}

	return ret;
}

/********************************************************************************
* FunctionName:	LoadFileOp														*
* Function    :	ファイル読出オペレーション										*
* Argument(s) :	none															*
* Return      :	FALSE=読出失敗													*
********************************************************************************/
bool
CRec::LoadFileOp(CWMStr* lpFilename)
{
	bool ret;				// 戻り値
	CFileIO fioLoadFile;	// 読出ファイルインスタンス
	CWMStr wmsLoadDat;		// 読出データ文字列
	long nPos;				// 処理カウンタ
	long nSize;				// データサイズ
	long nCnt;				// 読出カウンタ
	long nDat;				// 読出データ

	ret = FALSE;

	// 読出ファイルオープン
	if(fioLoadFile.Open(lpFilename->PtW(), FOPEN_READ))
	{
		// 読出データ文字列バッファ確保
		wmsLoadDat.NewM(fioLoadFile.Size() * 2);
		// 読出ファイル読み出し、クローズ
		if(fioLoadFile.Read(wmsLoadDat.PtM(), fioLoadFile.Size()) != FALSE)
		{
			// ワイドバイト文字に変換する
			wmsLoadDat.MirrorMtoW();
			wmsLoadDat.SeekClrW();
			// データを抽出する。データサイズ分繰り返す
			for(nPos = 0; nPos < E_REC_DAT_NUM; nPos++)
			{
				// カンマに続く数値を抽出してデータにセット
				wmsLoadDat.SeekFindW((LPWSTR)L"key:");
				wmsLoadDat.SeekMoveW(4);
				m_atRecDat[nPos].wmsKey.NewW(256);
				wmsLoadDat.SeekScanStrW(m_atRecDat[nPos].wmsKey.PtW(), 255);

				wmsLoadDat.SeekFindW((LPWSTR)L"size:");
				wmsLoadDat.SeekMoveW(5);
				if(wmsLoadDat.SeekScanDigW(&nSize) != 0)
				{
					m_atRecDat[nPos].nDatSize = nSize;
					m_atRecDat[nPos].wmsDat.NewW(0);
					for(nCnt = 0; nCnt < nSize; nCnt++)
					{
						if(wmsLoadDat.SeekScanDigW(&nDat) != 0)
						{
							if(nDat >= 0)
							{
								m_atRecDat[nPos].wmsDat.AddDigW(nDat, -1);
							}
							else
							{
								m_atRecDat[nPos].wmsDat.AddChrW('-');
								m_atRecDat[nPos].wmsDat.AddDigW(-nDat, -1);
							}
							m_atRecDat[nPos].wmsDat.AddChrW(',');
							if((nCnt & 0x07) == 0x07)
							{
								m_atRecDat[nPos].wmsDat.AddChrW('\n');
							}
						}
						wmsLoadDat.SeekMoveW(1);
					}
				}
				wmsLoadDat.SeekMoveW(1);
			}
			ret = TRUE;
		}
		fioLoadFile.Close();
	}

	return ret;
}

/********************************************************************************
*	Public Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	SaveFile														*
* Function    :	保存データの書き込み											*
* Argument    :	保存ファイル名													*
* Return      :	保存処理の結果(OK/NG)											*
********************************************************************************/
bool
CRec::SaveFile(CWMStr* lpFilename)
{
	return SaveFileOp(lpFilename);
}

/********************************************************************************
* FunctionName:	LoadFile														*
* Function    :	保存データの読み出し											*
* Argument    :	読出ファイル名													*
* Return      :	読出処理の結果(OK/NG)											*
********************************************************************************/
bool
CRec::LoadFile(CWMStr* lpFilename)
{
	return LoadFileOp(lpFilename);
}

/********************************************************************************
* FunctionName:	SetDat															*
* Function    :	保存データをセット												*
* Argument    :	情報キー、情報データ、情報サイズ								*
* Return      :	保存データのセット処理の結果(OK/NG)								*
********************************************************************************/
bool
CRec::SetDat(CWMStr* lpKey, long* lpDat, long nSize)
{
	bool ret;	// 戻り値
	long nPos;	// 保存データ参照位置
	long nCnt;	// 処理カウンタ

	ret = FALSE;

	// 同じ名前のKEYが登録済み領域を探す
	for(nPos = 0; nPos < E_REC_DAT_NUM; nPos++)
	{
		if(lpKey->CmpW(m_atRecDat[nPos].wmsKey.PtW()) == 0)
		{
			break;
		}
	}
	// 同じ名前のKEY登録が無ければ空き領域を探す
	if(nPos >= E_REC_DAT_NUM)
	{
		for(nPos = 0; nPos < E_REC_DAT_NUM; nPos++)
		{
			if(m_atRecDat[nPos].wmsKey.CmpW(nullptr) == 0)
			{
				break;
			}
		}
	}

	// 領域がある場合データを文字列化する
	if(nPos < E_REC_DAT_NUM)
	{
		m_atRecDat[nPos].wmsKey.SetW(lpKey->PtW());

		m_atRecDat[nPos].nDatSize = nSize;

		m_atRecDat[nPos].wmsDat.NewW(nSize*12);
		for(nCnt = 0; nCnt < nSize; nCnt++)
		{
			if(lpDat[nCnt] >= 0)
			{
				m_atRecDat[nPos].wmsDat.AddDigW(lpDat[nCnt], -1);
			}
			else
			{
				m_atRecDat[nPos].wmsDat.AddChrW('-');
				m_atRecDat[nPos].wmsDat.AddDigW(-lpDat[nCnt], -1);
			}
			m_atRecDat[nPos].wmsDat.AddChrW(',');
			if((nCnt & 0x07) == 0x07)
			{
				m_atRecDat[nPos].wmsDat.AddChrW('\n');
			}
		}
		ret = TRUE;
	}

	return ret;
}

/********************************************************************************
* FunctionName:	GetDat															*
* Function    :	保存データを取得												*
* Argument    :	情報キー、情報サイズ											*
* Return      :	保存データの取得処理の結果(OK/NG)								*
*               情報データ(ポインタ引数返し)									*
********************************************************************************/
bool
CRec::GetDat(CWMStr* lpKey, long* lpDat, long nSize)
{
	bool ret;	// 戻り値
	long nPos;	// 保存データ参照位置
	long nCnt;	// 処理カウンタ

	ret = FALSE;

	// 同じ名前のKEYが登録済みの領域を探す
	for(nPos = 0; nPos < E_REC_DAT_NUM; nPos++)
	{
		if(lpKey->CmpW(m_atRecDat[nPos].wmsKey.PtW()) == 0)
		{
			break;
		}
	}

	// 領域がある場合データを数値化する
	if(nPos < E_REC_DAT_NUM)
	{
		if(m_atRecDat[nPos].nDatSize == nSize)
		{
			m_atRecDat[nPos].wmsDat.SeekClrW();
			for(nCnt = 0; nCnt < nSize; nCnt++)
			{
				if(m_atRecDat[nPos].wmsDat.SeekScanDigW(&(lpDat[nCnt])) == 0)
				{
					break;
				}
				m_atRecDat[nPos].wmsDat.SeekMoveW(1);
			}
			if(nCnt >= nSize)
			{
				ret = TRUE;
			}
		}
	}

	return ret;
}
