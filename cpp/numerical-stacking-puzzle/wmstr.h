/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: wmstr.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ワイド/マルチ文字列操作										*
********************************************************************************/
#ifndef	WMSTR_H
#define	WMSTR_H

/********************************************************************************
*	Define(s)								*
********************************************************************************/

/********************************************************************************
*	Global Function(s)							*
********************************************************************************/

/********************************************************************************
*	Define Class								*
********************************************************************************/
class CWMStr
{
public:
	// Constructor(s)/Destructor
			CWMStr();
	virtual	~CWMStr();
private:
	LPWSTR	m_lpWStr;	// ヒープメモリへのポインタ
	char*	m_lpMStr;	// ヒープメモリへのポインタ
	size_t	m_nWSize;	// ヒープメモリのサイズ
	size_t	m_nMSize;	// ヒープメモリのサイズ
	size_t	m_nWSeek;	// ヒープメモリのシーク位置
	size_t	m_nMSeek;	// ヒープメモリのシーク位置
public:
	//----- 関数
	LPWSTR	NewW(size_t nWSize);
	char*	NewM(size_t nMSize);
	LPWSTR	PtW(void);
	char*	PtM(void);
	size_t	SizeW(void);
	size_t	SizeM(void);

	LPWSTR	MirrorMtoW(void);
	char*	MirrorWtoM(void);
	LPWSTR	SetW(LPWSTR lpWSrc);
	char*	SetM(char* lpMSrc);
	LPWSTR	AddW(LPWSTR lpWSrc);
	char*	AddM(char* lpMSrc);
	LPWSTR	AddW(LPWSTR lpWSrc, size_t nSrcLen);
	char*	AddM(char* lpMSrc, size_t nSrcLen);
	LPWSTR	AddChrW(WCHAR wcChr);
	char*	AddChrM(char cChr);
	LPWSTR	AddDigW(long nDigVal, long nDigLen);
	char*	AddDigM(long nDigVal, long nDigLen);
	LPWSTR	AddHexW(u_long ulHexVal, long nHexLen);
	char*	AddHexM(u_long ulHexVal, long nHexLen);
	LPWSTR	GetW(LPWSTR lpWDst);
	char*	GetM(char* lpMDst);
	WCHAR	ChrW(size_t nOffset);
	char	ChrM(size_t nOffset);
	WCHAR	ChrW(size_t nOffset, size_t nLen);
	char	ChrM(size_t nOffset, size_t nLen);
	size_t	LenW(void);
	size_t	LenM(void);
	float	WidW(void);
	float	WidM(void);
	long	CmpW(LPWSTR lpWSrc);
	long	CmpM(char* lpMSrc);
	long	CmpW(LPWSTR lpWSrc, size_t nLen);
	long	CmpM(char* lpMSrc, size_t nLen);
	void	SeekClrW(void);
	LPWSTR	SeekFindW(LPWSTR lpWSrc);
	long	SeekScanDigW(long* lpDig);
	long	SeekScanStrW(LPWSTR lpWDst, long nStrLen);
	void	SeekMoveW(size_t nSeek);
};

#endif// WMSTR_H
