/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: wmstr.cpp														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: ワイド/マルチ文字列操作										*
********************************************************************************/

/********************************************************************************
*	Include File(s)																*
********************************************************************************/
#include "framework.h"
#include "define.h"
#include "wmstr.h"

/********************************************************************************
*	Define(s)																	*
********************************************************************************/

/********************************************************************************
* CWMStrクラス																	*
* ワイド文字列(unicode)操作とヒープメモリ確保・解放クラス						*
********************************************************************************/

/********************************************************************************
*	CWMStr Public Constructor(s)/Destructor										*
********************************************************************************/

/********************************************************************************
* FunctionName:	CWMStr															*
* Function    :	コンストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CWMStr::CWMStr(void)
{
	m_lpWStr = nullptr;
	m_lpMStr = nullptr;
	m_nWSize = 0;
	m_nMSize = 0;
	m_nMSeek = 0;
	m_nWSeek = 0;
}

/********************************************************************************
* FunctionName:	~CWMStr															*
* Function    :	デストラクタ													*
* Argument(s) :	none															*
********************************************************************************/
CWMStr::~CWMStr(void)
{
	FREE(m_lpWStr);
	FREE(m_lpMStr);
}

/********************************************************************************
*	Public Methods																*
********************************************************************************/

/********************************************************************************
* FunctionName:	NewW/NewM														*
* Function    :	文字列バッファ確保												*
* Argument(s) :	バッファサイズ													*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::NewW(size_t nWSize)
{
	// 4バイト境界にアラインメント
	nWSize = (nWSize+3) & ~3;

	FREE(m_lpWStr);
	m_nWSize = 0;

	if(nWSize > 0)
	{
		NEW(m_lpWStr, WCHAR[nWSize]);
		if(m_lpWStr)
		{
			memset(m_lpWStr, WCHAR_NULL, sizeof(WCHAR)*nWSize);
			m_nWSize = nWSize;
		}
	}

	return m_lpWStr;
}

char*
CWMStr::NewM(size_t nMSize)
{
	// 4バイト境界にアラインメント
	nMSize = (nMSize+3) & ~3;

	FREE(m_lpMStr);
	m_nMSize = 0;

	if(nMSize > 0)
	{
		NEW(m_lpMStr, char[nMSize]);
		if(m_lpMStr)
		{
			memset(m_lpMStr, NULL, sizeof(char)*nMSize);
			m_nMSize = nMSize;
		}
	}

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	PtW/PtM															*
* Function    :	文字列バッファへのポインタを返す								*
* Argument(s) :	none															*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::PtW(void)
{
	return m_lpWStr;
}

char*
CWMStr::PtM(void)
{
	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	SizeW/SizeM														*
* Function    :	文字列バッファの確保サイズを返す								*
* Argument(s) :	none															*
* Return      :	バッファの確保サイズ											*
********************************************************************************/
size_t
CWMStr::SizeW(void)
{
	return m_nWSize;
}

size_t
CWMStr::SizeM(void)
{
	return m_nMSize;
}

/********************************************************************************
* FunctionName:	MirrorMtoW/MirrorWtoM											*
* Function    :	マルチバイト文字列とワイド文字列の相互変換						*
* Argument(s) :	none															*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::MirrorMtoW(void)
{
	size_t nLen;
	
	if(m_lpWStr != nullptr)
	{
		m_lpWStr[0] = WCHAR_NULL;
	}

	if(m_lpMStr != nullptr)
	{
		nLen = strlen(m_lpMStr);
		if(nLen >= 0)
		{
			if(nLen+1 > m_nWSize)
			{
				if(NewW(nLen+1) != nullptr)
				{
					mbstowcs_s(nullptr, m_lpWStr, nLen+1, m_lpMStr, _TRUNCATE);
				}
			}
			else
			{
				if(m_lpWStr != nullptr)
				{
					mbstowcs_s(nullptr, m_lpWStr, nLen+1, m_lpMStr, _TRUNCATE);
				}
			}
		}
	}
	return m_lpWStr;
}

char*
CWMStr::MirrorWtoM(void)
{
	size_t nLen;
	
	if(m_lpMStr != nullptr)
	{
		m_lpMStr[0] = NULL;
	}

	if(m_lpWStr != nullptr)
	{
		nLen = wcslen(m_lpWStr);
		if(nLen >= 0)
		{
			if(nLen+nLen+1 > m_nMSize)
			{
				if(NewM(nLen+nLen+1) != nullptr)
				{
					wcstombs_s(nullptr, m_lpMStr, nLen+nLen+1, m_lpWStr, _TRUNCATE);
				}
			}
			else
			{
				if(m_lpMStr != nullptr)
				{
					wcstombs_s(nullptr, m_lpMStr, nLen+nLen+1, m_lpWStr, _TRUNCATE);
				}
			}
		}
	}
	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	SetW/SetM														*
* Function    :	文字列バッファに文字列を設定									*
* Argument(s) :	文字列															*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::SetW(LPWSTR lpWSrc)
{
	size_t nLen;
	
	if(lpWSrc)
	{
		nLen = wcslen(lpWSrc);
		if(nLen+1 > m_nWSize)
		{
			if(NewW(nLen+1) != nullptr)
			{
				wcscpy_s(m_lpWStr, nLen+1, lpWSrc);
			}
		}
		else
		{
			if(m_lpWStr != nullptr)
			{
				wcscpy_s(m_lpWStr, nLen+1, lpWSrc);
			}
		}
	}
	else
	{
		if(m_nWSize > 0)
		{
			m_lpWStr[0] = WCHAR_NULL;
		}
	}

	return m_lpWStr;
}

char*
CWMStr::SetM(char* lpMSrc)
{
	size_t nLen;
	
	if(lpMSrc)
	{
		nLen = strlen(lpMSrc);
		if(nLen+1 > m_nMSize)
		{
			if(NewM(nLen+1) != nullptr)
			{
				strcpy_s(m_lpMStr, nLen+1, lpMSrc);
			}
		}
		else
		{
			if(m_lpMStr != nullptr)
			{
				strcpy_s(m_lpMStr, nLen+1, lpMSrc);
			}
		}
	}
	else
	{
		FREE(m_lpMStr);
		m_nMSize = 0;
	}

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	AddW/AddM														*
* Function    :	文字列バッファに文字列を追加									*
* Argument(s) :	文字列 or 文字列と文字列長										*
*				(文字列長の指定時は、指定長まで追加)							*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::AddW(LPWSTR lpWSrc)
{
	size_t nLen;
	CWMStr cDst;
	
	if(lpWSrc)
	{
		nLen = wcslen(lpWSrc);
		if(m_lpWStr)
		{
			nLen += wcslen(m_lpWStr);
		}
		if(nLen+1 > m_nWSize)
		{
			if(m_lpWStr == nullptr ||
			   cDst.SetW(m_lpWStr) != nullptr)
			{
				if(NewW(nLen+1) != nullptr)
				{
					if(SetW(cDst.PtW()) != nullptr)
					{
						wcscat_s(m_lpWStr, nLen+1, lpWSrc);
					}
				}
			}
		}
		else
		{
			if(m_lpWStr != nullptr)
			{
				wcscat_s(m_lpWStr, nLen+1, lpWSrc);
			}
		}
	}

	return m_lpWStr;
}

char*
CWMStr::AddM(char* lpMSrc)
{
	size_t nLen;
	CWMStr cDst;
	
	if(lpMSrc)
	{
		nLen = strlen(lpMSrc);
		if(m_lpMStr)
		{
			nLen += strlen(m_lpMStr);
		}
		if(nLen+1 > m_nMSize)
		{
			if(m_lpMStr == nullptr ||
			   cDst.SetM(m_lpMStr) != nullptr)
			{
				if(NewM(nLen+1) != nullptr)
				{
					if(SetM(cDst.PtM()) != nullptr)
					{
						strcat_s(m_lpMStr, nLen+1, lpMSrc);
					}
				}
			}
		}
		else
		{
			if(m_lpMStr != nullptr)
			{
				strcat_s(m_lpMStr, nLen+1, lpMSrc);
			}
		}
	}

	return m_lpMStr;
}

LPWSTR
CWMStr::AddW(LPWSTR lpWSrc, size_t nSrcLen)
{
	size_t nDstLen;
	CWMStr cDst;
	
	if(lpWSrc)
	{
		if(nSrcLen > wcslen(lpWSrc))
		{
			nSrcLen = wcslen(lpWSrc);
		}
		nDstLen = nSrcLen;
		if(m_lpWStr)
		{
			nDstLen += wcslen(m_lpWStr);
		}
		if(nDstLen+1 > m_nWSize)
		{
			if(m_lpWStr == nullptr ||
			   cDst.SetW(m_lpWStr) != nullptr)
			{
				if(NewW(nDstLen+1) != nullptr)
				{
					if(SetW(cDst.PtW()) != nullptr)
					{
						wcsncat_s(m_lpWStr, nDstLen+1, lpWSrc, nSrcLen);
					}
				}
			}
		}
		else
		{
			if(m_lpWStr != nullptr)
			{
				wcsncat_s(m_lpWStr, nDstLen+1, lpWSrc, nSrcLen);
			}
		}
	}

	return m_lpWStr;
}

char*
CWMStr::AddM(char* lpMSrc, size_t nSrcLen)
{
	size_t nDstLen;
	CWMStr cDst;
	
	if(lpMSrc)
	{
		if(nSrcLen > strlen(lpMSrc))
		{
			nSrcLen = strlen(lpMSrc);
		}
		nDstLen = nSrcLen;
		if(m_lpMStr)
		{
			nDstLen += strlen(m_lpMStr);
		}
		if(nDstLen+1 > m_nMSize)
		{
			if(m_lpMStr == nullptr ||
			   cDst.SetM(m_lpMStr) != nullptr)
			{
				if(NewM(nDstLen+1) != nullptr)
				{
					if(SetM(cDst.PtM()) != nullptr)
					{
						strcat_s(m_lpMStr, nDstLen+1, lpMSrc);
					}
				}
			}
		}
		else
		{
			if(m_lpMStr != nullptr)
			{
				strcat_s(m_lpMStr, nDstLen+1, lpMSrc);
			}
		}
	}

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	AddChrW/AddChrM													*
* Function    :	文字列バッファに１キャラクタ追加								*
* Argument(s) :	文字															*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::AddChrW(WCHAR wcChr)
{
	WCHAR sChrW[2];

	sChrW[0] = wcChr;
	sChrW[1] = WCHAR_NULL;
	(void)this->AddW(sChrW);

	return m_lpWStr;
}

char*
CWMStr::AddChrM(char cChr)
{
	char sChrM[2];

	sChrM[0] = cChr;
	sChrM[1] = NULL;
	(void)this->AddM(sChrM);

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	AddDigW/AddDigM													*
* Function    :	文字列バッファに10進数の値を文字列として追加					*
* Argument(s) :	数値、桁数														*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::AddDigW(long nDigVal, long nDigLen)
{
	long long nVal;
	long nLen;

	nVal = 0;
	nLen = 0;
	while(nDigLen != 0)
	{
		nVal += (long long)nDigVal % 10;
		nLen++;
		nDigLen--;
		if(nDigLen == 0)
		{
			break;
		}
		nDigVal /= 10;
		if(nDigLen < 0 && nDigVal == 0)
		{
			break;
		}
		nVal *= 10;
	}

	while(nLen > 0)
	{
		(void)this->AddChrW(L'0' + (nVal % 10));
		nVal /= 10;
		nLen--;
	}

	return m_lpWStr;
}

char*
CWMStr::AddDigM(long nDigVal, long nDigLen)
{
	long nVal;
	long nLen;

	nVal = 0;
	nLen = 0;
	while(nDigLen != 0)
	{
		nVal += nDigVal % 10;
		nLen++;
		nDigLen--;
		if(nDigLen == 0)
		{
			break;
		}
		nDigVal /= 10;
		if(nDigLen < 0 && nDigVal == 0)
		{
			break;
		}
		nVal *= 10;
	}

	while(nLen > 0)
	{
		(void)this->AddChrM('0' + (nVal % 10));
		nVal /= 10;
		nLen--;
	}

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	AddHexW/AddHexM													*
* Function    :	文字列バッファに文字列化した16進数の数値を追加					*
* Argument(s) :	数値、桁数														*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::AddHexW(u_long ulHexVal, long nHexLen)
{
	u_long ulVal;
	long nLen;

	ulVal = 0;
	nLen = 0;
	while(nHexLen != 0)
	{
		ulVal += (ulHexVal & 0x0000000F);
		nLen++;
		nHexLen--;
		if(nHexLen == 0)
		{
			break;
		}
		ulHexVal >>= 4;
		if(nHexLen < 0 && ulHexVal == 0)
		{
			break;
		}
		ulVal <<= 4;
	}

	while(nLen > 0)
	{
		if((ulVal & 0x0000000F) <= 0x00000009)
		{
			(void)this->AddChrW(L'0' + (ulVal & 0x0000000F));
		}
		else
		{
			(void)this->AddChrW(L'A' + (ulVal & 0x0000000F) - 0x0000000A);
		}
		ulVal >>= 4;
		nLen--;
	}

	return m_lpWStr;
}

char*
CWMStr::AddHexM(u_long ulHexVal, long nHexLen)
{
	u_long ulVal;
	long nLen;

	ulVal = 0;
	nLen = 0;
	while(nHexLen != 0)
	{
		ulVal += (ulHexVal & 0x0000000F);
		nLen++;
		nHexLen--;
		if(nHexLen == 0)
		{
			break;
		}
		ulHexVal >>= 4;
		if(nHexLen < 0 && ulHexVal == 0)
		{
			break;
		}
		ulVal <<= 4;
	}

	while(nLen > 0)
	{
		if((ulVal & 0x0000000F) <= 0x00000009)
		{
			(void)this->AddChrM('0' + (ulVal & 0x0000000F));
		}
		else
		{
			(void)this->AddChrM('A' + (ulVal & 0x0000000F) - 0x0000000A);
		}
		ulVal >>= 4;
		nLen--;
	}

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	GetW/GetM														*
* Function    :	文字列バッファから文字列の取得									*
* Argument(s) :	文字列転送先のポインタ											*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::GetW(LPWSTR lpWDst)
{
	size_t nLen;
	
	if(lpWDst)
	{
		if(m_lpWStr)
		{
			nLen = wcslen(m_lpWStr);
			if(nLen >= 0)
			{
				wcscpy_s(lpWDst, nLen+1, m_lpWStr);
			}
		}
	}

	return m_lpWStr;
}

char*
CWMStr::GetM(char* lpMDst)
{
	size_t nLen;
	
	if(lpMDst)
	{
		if(m_lpMStr)
		{
			nLen = strlen(m_lpMStr);
			if(nLen >= 0)
			{
				strcpy_s(lpMDst, nLen+1, m_lpMStr);
			}
		}
	}

	return m_lpMStr;
}

/********************************************************************************
* FunctionName:	ChrW/ChrM														*
* Function    :	文字列バッファから1キャラクタ取得								*
* Argument(s) :	オフセット位置 or オフセット位置と文字列長						*
*				(文字列長の指定時は、指定長までの範囲に限定)					*
* Return      :	オフセット位置の1キャラクタデータ								*
********************************************************************************/
WCHAR
CWMStr::ChrW(size_t nOffset)
{
	size_t nLen;
	WCHAR wRet;

	wRet = WCHAR_NULL;

	if(m_lpWStr)
	{
		nLen = wcslen(m_lpWStr);
		if(nOffset < nLen)
		{
			wRet = m_lpWStr[nOffset];
		}
	}

	return wRet;
}

char
CWMStr::ChrM(size_t nOffset)
{
	size_t nLen;
	char cRet;

	cRet = NULL;

	if(m_lpMStr)
	{
		nLen = strlen(m_lpMStr);
		if(nOffset < nLen)
		{
			cRet = m_lpMStr[nOffset];
		}
	}

	return cRet;
}

WCHAR
CWMStr::ChrW(size_t nOffset, size_t nLen)
{
	WCHAR wRet;

	wRet = WCHAR_NULL;

	if(m_lpWStr)
	{
		if(nOffset < nLen)
		{
			if(nOffset < m_nWSize)
			{
				wRet = m_lpWStr[nOffset];
			}
		}
	}

	return wRet;
}

char
CWMStr::ChrM(size_t nOffset, size_t nLen)
{
	char cRet;

	cRet = NULL;

	if(m_lpMStr)
	{
		if(nOffset < nLen)
		{
			if(nOffset < m_nMSize)
			{
				cRet = m_lpMStr[nOffset];
			}
		}
	}

	return cRet;
}

/********************************************************************************
* FunctionName:	LenW/LenM														*
* Function    :	文字列バッファに格納されている文字列の長さを返す				*
* Argument(s) :	none															*
* Return      :	文字列の長さ													*
* Attention(s):	文字列の長さはバッファの確保サイズとは異なるので注意			*
********************************************************************************/
size_t
CWMStr::LenW(void)
{
	if(m_lpWStr)
	{
		return wcslen(m_lpWStr);
	}
	else
	{
		return 0;
	}
}

size_t
CWMStr::LenM(void)
{
	if(m_lpMStr)
	{
		return strlen(m_lpMStr);
	}
	else
	{
		return 0;
	}
}

/********************************************************************************
* FunctionName:	WidW/WidM														*
* Function    :	文字列全体の幅を返す											*
* Argument(s) :	none															*
* Return      :	文字列全体の幅(全角1文字=1.0/半角1文字=0.5で表現)				*
********************************************************************************/
float
CWMStr::WidW(void)
{
	size_t n;
	float wid;

	wid = 0.0f;

	if(m_lpWStr)
	{
		for(n = 0; n < m_nWSize; n++)
		{
			if(m_lpWStr[n] & 0xFF80)
			{
				// 全角文字の場合
				wid += 1.0f;
			}
			else
			{
				// 半角文字の場合
				wid += 0.5f;
			}
		}
	}

	return wid;
}

float
CWMStr::WidM(void)
{
	float wid;

	wid = 0.0f;

	if(m_lpMStr)
	{
		wid = strlen(m_lpMStr)*0.5f;
	}

	return wid;
}

/********************************************************************************
* FunctionName:	CmpW/CmpM														*
* Function    :	文字列の比較													*
* Argument(s) :	比較対象の文字列 or 比較対象の文字列と文字列長					*
*				(文字列長の指定時は、指定長までの範囲に限定)					*
* Return      :	比較結果(0=同一)												*
********************************************************************************/
long
CWMStr::CmpW(LPWSTR lpWSrc)
{
	long nRet;

	nRet = 0;

	if(lpWSrc)
	{
		if(m_lpWStr)
		{
			nRet = wcscmp(lpWSrc, m_lpWStr);
		}
		else
		{
			if(lpWSrc[0] != WCHAR_NULL)
			{
				nRet = 1;
			}
			else
			{
			}
		}
	}
	else
	{
		if(m_lpWStr)
		{
			if(m_lpWStr[0] != WCHAR_NULL)
			{
				nRet = -1;
			}
			else
			{
			}
		}
		else
		{
		}
	}

	return nRet;
}

long
CWMStr::CmpM(char* lpMSrc)
{
	long nRet;

	nRet = 0;

	if(lpMSrc)
	{
		if(m_lpMStr)
		{
			nRet = strcmp(lpMSrc, m_lpMStr);
		}
		else
		{
			if(lpMSrc[0] != NULL)
			{
				nRet = 1;
			}
			else
			{
			}
		}
	}
	else
	{
		if(m_lpMStr)
		{
			if(m_lpMStr[0] != NULL)
			{
				nRet = -1;
			}
			else
			{
			}
		}
		else
		{
		}
	}

	return nRet;
}

long
CWMStr::CmpW(LPWSTR lpWSrc, size_t nLen)
{
	long nRet;

	nRet = 0;

	if(lpWSrc)
	{
		if(m_lpWStr)
		{
			nRet = wcsncmp(lpWSrc, m_lpWStr, nLen);
		}
		else
		{
			if(lpWSrc[0] != WCHAR_NULL)
			{
				nRet = 1;
			}
			else
			{
			}
		}
	}
	else
	{
		if(m_lpWStr)
		{
			if(m_lpWStr[0] != WCHAR_NULL)
			{
				nRet = -1;
			}
			else
			{
			}
		}
		else
		{
		}
	}

	return nRet;
}

long
CWMStr::CmpM(char* lpMSrc, size_t nLen)
{
	long nRet;

	nRet = 0;

	if(lpMSrc)
	{
		if(m_lpMStr)
		{
			nRet = strncmp(lpMSrc, m_lpMStr, nLen);
		}
		else
		{
			if(lpMSrc[0] != NULL)
			{
				nRet = 1;
			}
			else
			{
			}
		}
	}
	else
	{
		if(m_lpMStr)
		{
			if(m_lpMStr[0] != NULL)
			{
				nRet = -1;
			}
			else
			{
			}
		}
		else
		{
		}
	}

	return nRet;
}

/********************************************************************************
* FunctionName:	SeekClrW														*
* Function    :	文字列走査位置のクリア											*
* Argument(s) :	none															*
* Return      :	none															*
********************************************************************************/
void
CWMStr::SeekClrW(void)
{
	m_nWSeek = 0;
}

/********************************************************************************
* FunctionName:	SeekFindW														*
* Function    :	文字列内から文字列を検索										*
* Argument(s) :	検索文字列														*
* Return      :	バッファへのポインタ											*
********************************************************************************/
LPWSTR
CWMStr::SeekFindW(LPWSTR lpWSrc)
{
	LPWSTR lp;

	lp = nullptr;
	if(lpWSrc)
	{
		if(m_nWSeek < m_nWSize)
		{
			lp = wcsstr(m_lpWStr + m_nWSeek, lpWSrc);
			if(lp != nullptr)
			{
				m_nWSeek = lp - m_lpWStr;
			}
			else
			{
				m_nWSeek = m_nWSize;
			}
		}
	}

	return lp;
}

/********************************************************************************
* FunctionName:	SeekScanDigW													*
* Function    :	文字列を10進数数値と見なして数字の整数値を取得する				*
* Argument(s) :	10進数数値の格納先のポインタ									*
* Return      :	10進数数値														*
********************************************************************************/
long
CWMStr::SeekScanDigW(long* lpDig)
{
	long dig;
	long sign;

	dig = -1;
	sign = 1;
	while(m_nWSeek < m_nWSize)
	{
		if(m_lpWStr[m_nWSeek] == L' ' ||
		   m_lpWStr[m_nWSeek] == L'\t' ||
		   m_lpWStr[m_nWSeek] == L'\n' ||
		   m_lpWStr[m_nWSeek] == L'\r')
		{
			m_nWSeek++;
		}
		else
		{
			break;
		}
	}

	while(m_nWSeek < m_nWSize)
	{
		if(m_lpWStr[m_nWSeek] >= L'0' &&
		   m_lpWStr[m_nWSeek] <= L'9')
		{
			if(dig < 0)
			{
				dig = 0;
			}
			else
			{
				dig *= 10;
			}
			dig += (m_lpWStr[m_nWSeek] - L'0');
			m_nWSeek++;
		}
		else if(m_lpWStr[m_nWSeek] == L'-')
		{
			if(dig < 0)
			{
				if(sign != -1)
				{
					sign = -1;
					m_nWSeek++;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	if(dig >= 0)
	{
		*lpDig = (dig * sign);
		return 1;
	}
	else
	{
		return 0;
	}
}

/********************************************************************************
* FunctionName:	SeekScanStrW													*
* Function    :	文字列内の文字列を抽出する										*
* Argument(s) :	文字列の格納先のポインタと文字列長								*
* Return      :	抽出した文字列長												*
********************************************************************************/
long
CWMStr::SeekScanStrW(LPWSTR lpWDst, long nStrLen)
{
	long len;

	len = 0;
	while(m_nWSeek < m_nWSize)
	{
		if(m_lpWStr[m_nWSeek] == L' ' ||
		   m_lpWStr[m_nWSeek] == L'\t' ||
		   m_lpWStr[m_nWSeek] == L'\n' ||
		   m_lpWStr[m_nWSeek] == L'\r')
		{
			m_nWSeek++;
		}
		else
		{
			break;
		}
	}

	while(m_nWSeek < m_nWSize)
	{
		if(m_lpWStr[m_nWSeek] == L',' ||
		   m_lpWStr[m_nWSeek] == L' ' ||
		   m_lpWStr[m_nWSeek] == L'\t' ||
		   m_lpWStr[m_nWSeek] == L'\n' ||
		   m_lpWStr[m_nWSeek] == L'\r')
		{
			break;
		}
		else
		{
			if(len < nStrLen)
			{
				*lpWDst = m_lpWStr[m_nWSeek];
				lpWDst++;
				m_nWSeek++;
				len++;
			}
			else
			{
				break;
			}
		}
	}

	return len;
}

/********************************************************************************
* FunctionName:	SeekMoveW														*
* Function    :	文字列走査位置の移動											*
* Argument(s) :	移動後の走査位置												*
* Return      :	none															*
********************************************************************************/
void
CWMStr::SeekMoveW(size_t nSeek)
{
	m_nWSeek += nSeek;
	if(m_nWSeek > m_nWSize)
	{
		m_nWSeek = m_nWSize;
	}
}
