/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: rec.h															*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 記録データ読出保存ライブラリ									*
********************************************************************************/
#ifndef REC_H
#define REC_H

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"
#include "wmstr.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
#define	E_REC_DAT_NUM		(100)	// データ保存数の上限

typedef struct T_REC_DAT
{
	CWMStr wmsKey;	// 情報キー
	long nDatSize;	// 情報サイズ
	CWMStr wmsDat;	// 情報本体
}T_REC_DAT;

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRec
{
public:
	//----- Constructor(s)/Destructor
			CRec(void);
	virtual	~CRec(void);
private:
	//----- 変数
	T_REC_DAT	m_atRecDat[E_REC_DAT_NUM];
	//----- 関数
	// ファイル保存オペレーション
	bool	SaveFileOp(CWMStr* lpFilename);
	// ファイル読出オペレーション
	bool	LoadFileOp(CWMStr* lpFilename);
public:
	//----- 関数
	// 保存データの書き込み
	bool	SaveFile(CWMStr* lpFilename);
	// 保存データの読み出し
	bool	LoadFile(CWMStr* lpFilename);
	// 保存データをセット
	bool	SetDat(CWMStr* lpKey, long* lpDat, long nSize);
	// 保存データを取得
	bool	GetDat(CWMStr* lpKey, long* lpDat, long nSize);
};

#endif// REC_H
