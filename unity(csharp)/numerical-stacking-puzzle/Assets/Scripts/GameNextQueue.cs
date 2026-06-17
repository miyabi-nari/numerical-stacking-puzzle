//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// NEXTキュー管理クラス
// 次に出現するブロックを固定長キューで管理する
// C++版相当クラス: CRNextDat
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui
//  +- CRMidSnd
//  +- CRAppli
//      +- CRCellDat
//      +- CRNextDat === GameNextQueue
//      +- CRec
//      +- CRTitleLblSet
//      |   +- CRTitleLbl
//      +- CRAppBoxSet
//      |   +- CRAppBox
//      +- CRAppNextBoxSet
//      |   +- CRAppBox
//      +- CRAppKeyIndSet
//      |   +- CRAppKeyInd
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using System;

//----------------------------------------------------------------------
// NEXTキュー管理クラス（C++版 CRNextDat 相当）
// 次に出現するブロックを固定長キューで管理する
//----------------------------------------------------------------------
public class GameNextQueue
{
    public const int SIZE = 4;

    private int[] nextQueue = new int[SIZE];

    // 次に生成される値のカウンタ
    private int nextBoxCount = 0;

    // 値循環用タイマ（周期6）
    private int boxValTimer = 6;

    public GameNextQueue()
    {
        ClearAll();
    }

    //--------------------------------------------------------------
    // 初期化
    // 0,1,2,3 を順に並べる（C++版と同じ初期状態）
    //--------------------------------------------------------------
    public void ClearAll()
    {
        nextBoxCount = 0;
        boxValTimer = 6;

        // C++版と同じく最初は 0,1,2,3
        for (int i = 0; i < nextQueue.Length; i++)
        {
            nextQueue[i] = nextBoxCount;
            nextBoxCount++;
        }
    }

    //--------------------------------------------------------------
    // 範囲チェック
    //--------------------------------------------------------------
    public bool InRange(int x)
    {
        return x >= 0 && x < SIZE;
    }

    //--------------------------------------------------------------
    // タイマ更新（C++仕様）
    // 6 → 5 → 4 ... → 0 → 6 とループ
    //--------------------------------------------------------------
    public void ExecCountTimer()
    {
        // C++: Dec(m_tmBoxVal) → 0なら6へ戻す
        boxValTimer--;

        if (boxValTimer <= 0)
        {
            boxValTimer = 6;
        }
    }

    //--------------------------------------------------------------
    // 値設定（ロード用）
    //--------------------------------------------------------------
    public void SetVal(int x, int val)
    {
        if (!InRange(x)) return;
        nextQueue[x] = val;
    }

    //--------------------------------------------------------------
    // 値取得
    //--------------------------------------------------------------
    public int GetVal(int x)
    {
        if (!InRange(x)) return -1;
        return nextQueue[x];
    }

    //--------------------------------------------------------------
    // 次の値生成ロジック（C++再現）
    //
    // ・最初の6個は 0,1,2,3,4,5 を順に出す
    // ・以降は timer % 6 で循環
    //--------------------------------------------------------------
    int GetNextBoxNum()
    {
        int ret;

        if (nextBoxCount < 6)
        {
            ret = nextBoxCount;
        }
        else
        {
            ret = boxValTimer % 6;
        }

        nextBoxCount++;
        return ret;
    }

    //--------------------------------------------------------------
    // 内部状態取得（セーブ用）
    //--------------------------------------------------------------
    public int GetNextBoxCount()
    {
        return nextBoxCount;
    }

    public void SetNextBoxCount(int c)
    {
        nextBoxCount = c;
    }

    public int GetBoxValTimer()
    {
        return boxValTimer;
    }

    public void SetBoxValTimer(int timer)
    {
        boxValTimer = timer;
    }

    //--------------------------------------------------------------
    // 次のブロックを取り出し（左シフト）
    // ・先頭を返す
    // ・残りを左に詰める
    // ・末尾に新規生成値を追加
    //--------------------------------------------------------------
    public int ExecPush()
    {
        int ret = nextQueue[0];

        // 左シフト
        for (int i = 0; i < nextQueue.Length - 1; i++)
        {
            nextQueue[i] = nextQueue[i + 1];
        }

        // 末尾に新規生成
        nextQueue[nextQueue.Length - 1] = GetNextBoxNum();

        return ret;
    }
}
