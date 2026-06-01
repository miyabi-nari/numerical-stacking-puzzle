/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: fileio.cpp													*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ファイル入出力のアクセスライブラリ							*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "framework.h"	/* 必須 */
#include "define.h"
#include "fileio.h"
#include "wmstr.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
static const WCHAR F_FOPEN_MODE[3][3] = {L"rb", L"wb", L"ab"};

/********************************************************************************
*	Define Static Instance														*
********************************************************************************/
WCHAR	CFileIO::m_lpDefaultPath[3][MAX_PATH];

/********************************************************************************
*	CFileIO Public Constructor(s)/Destructor									*
********************************************************************************/

/********************************************************************************
* FunctionName:	CFileIO															*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CFileIO::CFileIO(void)
{
	// バッファ初期化
	m_hFile = NULL;
	m_nOpenMode = -1;
	m_lpPath[0] = WCHAR_NULL;
}

/********************************************************************************
* FunctionName:	CFileIO															*
* Function    :	コンストラクタ													*
* Argument(s) :	filename=ファイル名												*
*				nOpenMode=ファイルオープンモード								*
********************************************************************************/
CFileIO::CFileIO(LPWSTR filename, long nOpenMode)
{
	// 初期化＆ファイルオープン
	m_hFile = NULL;
	m_nOpenMode = -1;
	m_lpPath[0] = WCHAR_NULL;
	Open(filename, nOpenMode);
}

/********************************************************************************
* FunctionName:	~CFileIO														*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CFileIO::~CFileIO(void)
{
	Close();
}

/********************************************************************************
*	Public Methods																*
********************************************************************************/
bool
CFileIO::AllSetUp(LPWSTR lpFirstPath, LPWSTR lpSecondPath, LPWSTR lpThirdPath)
{
	CWMStr awmsPath[3];

	m_lpDefaultPath[0][0] = m_lpDefaultPath[1][0] = m_lpDefaultPath[2][0] = WCHAR_NULL;

	// ファイル検索パス設定
	awmsPath[0].SetW(lpFirstPath);
	awmsPath[1].SetW(lpSecondPath);
	awmsPath[2].SetW(lpThirdPath);

	long i;
	for(i = 0; i < 3; i++)
	{
		if(awmsPath[i].LenW() > 0)
		{
			// 末尾に￥が付いていない場合は付ける
			if(awmsPath[i].ChrW(awmsPath[i].LenW()-1) != L'\\')
			{
				awmsPath[i].AddW((LPWSTR)(L"\\"));
			}
			awmsPath[i].GetW(m_lpDefaultPath[i]);
		}
	}

	// ファイルクリーンアップ
	AllCleanup();

	Debuglog((LPWSTR)(L"FirstDir."));
	Debuglog(m_lpDefaultPath[0]);
	Debuglog((LPWSTR)(L"SecondDir."));
	Debuglog(m_lpDefaultPath[1]);
	Debuglog((LPWSTR)(L"ThirdDir."));
	Debuglog(m_lpDefaultPath[2]);

	return TRUE;
}

void
CFileIO::AllCleanup()
{
	// 現状実装なし
}

/********************************************************************************
*	Private Methods																*
********************************************************************************/
FILE*
CFileIO::FOpen(LPWSTR lpFilename, long nOpenMode)
{
	CWMStr wmsPath;

	// 引数チェック
	if(lpFilename == nullptr || !NUMOK(nOpenMode, 3))
	{
		return nullptr;
	}

	// ファイル検索パスを付加してファイルオープン
	FILE* file;
	for(long i = 0; i <= 2; i++)
	{
		// ファイルパス検索
		if(*m_lpDefaultPath[i] != WCHAR_NULL)
		{
			// デフォルトパス有り
			wmsPath.SetW(m_lpDefaultPath[i]);
			if(*m_lpPath != WCHAR_NULL)
			{
				// パス追加
				wmsPath.AddW(m_lpPath);
			}
			wmsPath.AddW(lpFilename);
			if(_wfopen_s(&file, wmsPath.PtW(), F_FOPEN_MODE[nOpenMode]) == 0)
			{
				// ファイル有り -> ファイルオープン
				Debuglog((LPWSTR)(L"FileOpen"));
				Debuglog(wmsPath.PtW());
				return file;
			}
		}
	}
	// ファイルオープンに失敗した場合 -> デフォルトパス無しオープン
	wmsPath.SetW((LPWSTR)(L""));
	if(*m_lpPath != WCHAR_NULL)
	{
		// パス追加
		wmsPath.AddW(m_lpPath);
	}
	wmsPath.AddW(lpFilename);
	if(_wfopen_s(&file, wmsPath.PtW(), F_FOPEN_MODE[nOpenMode]) == 0)
	{
		// ファイル有り -> ファイルオープン
		Debuglog((LPWSTR)(L"FileOpen"));
		Debuglog(wmsPath.PtW());
		return file;
	}

	return NULL;
}

/********************************************************************************
*	Public Methods																*
********************************************************************************/
bool
CFileIO::Open(LPWSTR lpFilename, long nOpenMode)
{
	// 引数チェック
	if(lpFilename == nullptr || !NUMOK(nOpenMode, 3))
	{
		return FALSE;
	}

	// 直前まで使用していたファイルは解放
	Close();
	m_nOpenMode = nOpenMode;

	// -> ファイルオープン
	m_hFile = FOpen(lpFilename, nOpenMode);
	return m_hFile != NULL;
}

void
CFileIO::Close()
{
	if(m_hFile)
	{
		fclose(m_hFile);
		m_hFile = nullptr;
	}
}

bool
CFileIO::SetPath(LPWSTR lpPath)
{
	CWMStr wmsPath;

	m_lpPath[0] = WCHAR_NULL;

	// パス設定
	if(lpPath)
	{
		// 検索パス設定あり
		wmsPath.SetW(lpPath);
		// 末尾に￥が付いていない場合は付ける
		if(wmsPath.ChrW(wmsPath.LenW()-1) != L'\\')
		{
			wmsPath.AddW((LPWSTR)(L"\\"));
		}
		wmsPath.GetW(m_lpPath);
	}

	return TRUE;
}

long
CFileIO::Size()
{
	if(!m_hFile)
	{
		return 0;
	}

	long i, j = ftell(m_hFile);

	fseek(m_hFile, 0, SEEK_END);
	i = ftell(m_hFile);
	fseek(m_hFile, j, SEEK_SET);

	return i;
}

long
CFileIO::Tell()
{
	if(!m_hFile)
	{
		return 0;
	}

	return ftell(m_hFile);
}

bool
CFileIO::Seek(long offset, int origin)
{
	if(!m_hFile)
	{
		return FALSE;
	}

	return fseek(m_hFile, offset, origin) == 0;
}

bool
CFileIO::Read(void *buffer, long count)
{
	// 読み込みモード以外は失敗
	if(m_nOpenMode != FOPEN_READ)
	{
		return FALSE;
	}

	// 通常モード
	if(!m_hFile)
	{
		return FALSE;
	}

	return ((u_long)count == fread(buffer, sizeof(char), count, m_hFile)/* || feof(m_hFile)*/);
}

bool
CFileIO::Write(const void *buffer, long count)
{
	// 書き込みモード以外は失敗
	if(!m_hFile || m_nOpenMode == FOPEN_READ)
	{
		return FALSE;
	}

	return (u_long)count == fwrite(buffer, sizeof(char), count, m_hFile);
}

bool
CFileIO::WriteLn(const LPWSTR buffer)
{
	CWMStr wmsBuffer;

	// 書き込みモード以外は失敗
	if(!m_hFile || m_nOpenMode == FOPEN_READ)
	{
		return FALSE;
	}

	wmsBuffer.SetW(buffer);
	wmsBuffer.AddW((LPWSTR)(L"\x0D\x0A"));
	wmsBuffer.MirrorWtoM();

	return wmsBuffer.LenM() == fwrite(wmsBuffer.PtM(), sizeof(char), wmsBuffer.LenM(), m_hFile);
}

bool
CFileIO::WriteLn(const LPWSTR buffer, long n)
{
	CWMStr wmsBuffer;

	// 書き込みモード以外は失敗
	if(!m_hFile || m_nOpenMode == FOPEN_READ)
	{
		return FALSE;
	}

	wmsBuffer.SetW(buffer);
	wmsBuffer.AddDigW(n, -1);
	wmsBuffer.AddW((LPWSTR)(L"\x0D\x0A"));
	wmsBuffer.MirrorWtoM();

	return wmsBuffer.LenM() == fwrite(wmsBuffer.PtM(), sizeof(char), wmsBuffer.LenM(), m_hFile);
}
