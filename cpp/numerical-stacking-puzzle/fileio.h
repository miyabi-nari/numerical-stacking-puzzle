/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: fileio.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ファイル入出力のアクセスライブラリ							*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "stdio.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
#define	FOPEN_READ		(0)	// 読み取り
#define	FOPEN_WRITE		(1)	// 書き込み
#define	FOPEN_APPEND	(2)	// 追記

#define	FSEEK_CUR	SEEK_CUR
#define	FSEEK_END	SEEK_END
#define	FSEEK_SET	SEEK_SET
#define	FSEEK_BEGIN	SEEK_SET

#define CFILE_MAXFILES	250	// キャッシュファイル上限数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CFileIO
{
public:
	//----- Constructor(s)/Destructor
			CFileIO(void);
			CFileIO(LPWSTR filename, long nOpenMode);
	virtual	~CFileIO(void);
private:
	//----- Static変数
	static	WCHAR	m_lpDefaultPath[3][MAX_PATH];
	//----- 変数
	long	m_nOpenMode;	// ファイルオープンモード
	WCHAR	m_lpPath[MAX_PATH];	// ファイルパス
	//----- 関数
	FILE*	FOpen(LPWSTR lpFilename, long nOpenMode);
public:
	//----- 変数
	FILE*	m_hFile;	// ファイルハンドル
	//----- static関数
	static	bool	AllSetUp(LPWSTR lpFirstPath, LPWSTR lpSecondPath, LPWSTR lpThirdPath);
	static	void	AllCleanup();
	//----- 関数
	bool	Open(LPWSTR lpFilename, long nOpenMode);
	void	Close();
	bool	SetPath(LPWSTR lpPath);
	long	Size();
	long	Tell();
	bool	Seek(long offset, int origin = FSEEK_CUR);
	bool	Read(void* buffer, long count);
	bool	Write(const void* buffer, long count);
	bool	WriteLn(const LPWSTR buffer);
	bool	WriteLn(const LPWSTR buffer, long n);
};
