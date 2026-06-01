/********************************************************************************
* Copyright 2026 Miyabi															*
*																				*
* Project Name	: Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)	*
* File Name		: appbox.h														*
* Author		: Miyabi														*
* Create		: 2026/06/01 													*
* Purpose		: 箱オブジェクト												*
********************************************************************************/

/********************************************************************************
*	Include Files																*
********************************************************************************/
#include "routine.h"

/********************************************************************************
*	Defines																		*
********************************************************************************/
// 箱種類
#define	E_APP_BOX_TYPE_POW00	( 0)	// 2の0乗
#define	E_APP_BOX_TYPE_POW01	( 1)	// 2の1乗
#define	E_APP_BOX_TYPE_POW02	( 2)	// 2の2乗
#define	E_APP_BOX_TYPE_POW03	( 3)	// 2の3乗
#define	E_APP_BOX_TYPE_POW04	( 4)	// 2の4乗
#define	E_APP_BOX_TYPE_POW05	( 5)	// 2の5乗
#define	E_APP_BOX_TYPE_POW06	( 6)	// 2の6乗
#define	E_APP_BOX_TYPE_POW07	( 7)	// 2の7乗
#define	E_APP_BOX_TYPE_POW08	( 8)	// 2の8乗
#define	E_APP_BOX_TYPE_POW09	( 9)	// 2の9乗
#define	E_APP_BOX_TYPE_POW10	(10)	// 2の10乗
#define	E_APP_BOX_TYPE_POW11	(11)	// 2の11乗
#define	E_APP_BOX_TYPE_POW12	(12)	// 2の12乗
#define	E_APP_BOX_TYPE_POW13	(13)	// 2の13乗
#define	E_APP_BOX_TYPE_POW14	(14)	// 2の14乗
#define	E_APP_BOX_TYPE_POW15	(15)	// 2の15乗
#define	E_APP_BOX_TYPE_POW16	(16)	// 2の16乗
#define	E_APP_BOX_TYPE_POW17	(17)	// 2の17乗
#define	E_APP_BOX_TYPE_POW18	(18)	// 2の18乗
#define	E_APP_BOX_TYPE_POW19	(19)	// 2の19乗
#define	E_APP_BOX_TYPE_POW20	(20)	// 2の20乗
#define	E_APP_BOX_TYPE_NUM		(21)	// 箱種類の総数

// 箱状態
#define	E_APP_BOX_STAT_IDLE		( 0)	// 箱状態：アイドル
#define	E_APP_BOX_STAT_SLIDE	( 1)	// 箱状態：スライド移動
#define	E_APP_BOX_STAT_CHANGE	( 2)	// 箱状態：種類の変更
#define	E_APP_BOX_STAT_FALL		( 3)	// 箱状態：落下
#define	E_APP_BOX_STAT_SERVE	( 4)	// 箱状態：サーブ
#define	E_APP_BOX_STAT_NUM		( 5)	// 箱状態の総数

/********************************************************************************
*	Define Class																*
********************************************************************************/
class CRAppBox : public CRoutine
{
public:
	//----- Constructor(s)/Destructor
			CRAppBox(void);
			CRAppBox(long nBoxTypeID, long nPosX, long nPosY);
	virtual	~CRAppBox(void);
private:
	//----- 変数
	long	m_nBoxTypeID;	// 箱種類
	long	m_nChangeFromBoxTypeID;	// 変更前箱種類
	long	m_nChangeToBoxTypeID;	// 変更後箱種類
	long	m_nPosX;		// 位置X
	long	m_nPosY;		// 位置Y
	long	m_nStat;		// 箱状態
	long	m_nSlideFromX;	// スライド移動開始位置X
	long	m_nSlideFromY;	// スライド移動開始位置Y
	long	m_nSlideToX;	// スライド移動終了位置X
	long	m_nSlideToY;	// スライド移動終了位置Y
	long	m_nFallFromX;	// 落下移動開始位置X
	long	m_nFallFromY;	// 落下移動開始位置Y
	long	m_nFallToX;		// 落下移動終了位置X
	long	m_nFallToY;		// 落下移動終了位置Y
	long	m_nServeFromX;	// サーブ移動開始位置X
	long	m_nServeFromY;	// サーブ移動開始位置Y
	long	m_nServeToX;	// サーブ移動終了位置X
	long	m_nServeToY;	// サーブ移動終了位置Y
	long	m_nGuiRectID;	// GUIの矩形ID
	long	m_nGuiTxtID;	// GUIのテキストID
	long	m_tmSeq;		// シーケンスタイマー
	//----- 関数
	// GUI表示の更新処理
	bool	UpdateGui(void);
	// シーケンス
	bool	(CRAppBox::*m_lpSeq[20])(void);
	long	m_nSeq;				// 処理シーケンスNo.
	bool	SeqInit(void);		// 初期状態
	bool	SeqIdle(void);		// 待機状態
	bool	SeqSlide(void);		// スライド移動状態
	bool	SeqChange(void);	// 種類の変更状態
	bool	SeqFall(void);		// 落下状態
	bool	SeqServe(void);		// サーブ状態
public:
	//----- 関数
	// 定期更新処理
	bool	Update(void);
	// 位置を設定
	void	SetPos(long nPosX, long nPosY);
	// 箱種類を取得
	long	GetBoxTypeID(void){return m_nBoxTypeID;};
	// 箱状態をスライド移動に設定
	void	SetStatSlide(long nSlideToX, long nSlideToY);
	// 箱状態を種類変更に設定
	void	SetStatChange(long nChangeToBoxTypeID);
	// 箱状態を落下に設定
	void	SetStatFall(long nFallToX, long nFallToY);
	// 箱状態をサーブに設定
	void	SetStatServe(long nServeToX, long nServeToY);
	// 箱状態を取得
	long	GetStat(void);
};
