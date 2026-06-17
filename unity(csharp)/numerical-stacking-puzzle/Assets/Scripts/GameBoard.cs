//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// 盤面データとゲームルールを管理するModelクラス
//
// ・セル値の保持
// ・スタック判定／実行
// ・マージ判定／実行
// ・落下補正判定／実行
// ・スコア計算
//
// C++版 CRCellDat のロジックをベースに移植
// C++版相当クラス: CRCellDat
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui
//  +- CRMidSnd
//  +- CRAppli
//      +- CRCellDat === GameBoard
//      +- CRNextDat
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

public class GameBoard
{
    //----------------------------------------------------------------------
    // 1セル分の内部データ
    // C++版 T_LD_CELLDAT 相当
    //----------------------------------------------------------------------
    public struct CellData
    {
        public int Val;       // 現在値
        public int StackVal;  // スタック予定値
        public int MergeX;    // マージ基準X
        public int MergeY;    // マージ基準Y
        public int MergeVal;  // マージ後の値
        public int LandY;     // 落下先Y
        public int LandVal;   // 未使用（互換用）
    }

    private CellData[,] cells = new CellData[GameDef.CELL_X_NUM, GameDef.CELL_Y_NUM];
    private int score = 0;

    public GameBoard()
    {
        ClearAll();
    }

    //----------------------------------------------------------------------
    // 全セル初期化
    //----------------------------------------------------------------------
    public void ClearAll()
    {
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                cells[x, y].Val = -1;
                cells[x, y].StackVal = -1;
                cells[x, y].MergeX = -1;
                cells[x, y].MergeY = -1;
                cells[x, y].MergeVal = -1;
                cells[x, y].LandY = -1;
                cells[x, y].LandVal = -1;
            }

        score = 0;
    }

    //----------------------------------------------------------------------
    // 範囲チェック
    //----------------------------------------------------------------------
    public bool InRange(int x, int y)
    {
        return x >= 0 && x < GameDef.CELL_X_NUM && y >= 0 && y < GameDef.CELL_Y_NUM;
    }

    //----------------------------------------------------------------------
    // 値取得／設定
    //----------------------------------------------------------------------
    public void SetVal(int x, int y, int value)
    {
        if (!InRange(x, y)) return;
        cells[x, y].Val = value;
    }

    public int GetVal(int x, int y)
    {
        if (!InRange(x, y)) return -1;
        return cells[x, y].Val;
    }

    public int GetStackVal(int x, int y)
    {
        if (!InRange(x, y)) return -1;
        return cells[x, y].StackVal;
    }

    public int GetMergeVal(int x, int y)
    {
        if (!InRange(x, y)) return -1;
        return cells[x, y].MergeVal;
    }

    public int GetMergeX(int x, int y)
    {
        if (!InRange(x, y)) return -1;
        return cells[x, y].MergeX;
    }

    public int GetMergeY(int x, int y)
    {
        if (!InRange(x, y)) return -1;
        return cells[x, y].MergeY;
    }

    public int GetLandY(int x, int y)
    {
        if (!InRange(x, y)) return -1;
        return cells[x, y].LandY;
    }

    //----------------------------------------------------------------------
    // スタック可能判定
    // C++: CheckEnStack
    //
    // 落下中ブロックが現在位置で着地できるかを判定し、
    // 着地可能なら StackVal に仮設定する
    //----------------------------------------------------------------------
    public bool CheckEnStack(int x, int y, int val)
    {
        bool ret = false;

        // スタック候補を初期化
        for (int cx = 0; cx < GameDef.CELL_X_NUM; cx++)
            for (int cy = 0; cy < GameDef.CELL_Y_NUM; cy++)
            {
                cells[cx, cy].StackVal = -1;
            }

        if (!InRange(x, y)) return false;

        // 最下段なら着地可能
        if (y >= GameDef.CELL_Y_NUM - 1)
        {
            cells[x, y].StackVal = val;
            ret = true;
        }
        else
        {
            int belowVal = cells[x, y + 1].Val;

            // 下に値がある場合のみ着地候補
            if (belowVal >= 0)
            {
                if (y >= 1)
                {
                    // 2段目以降は通常着地可能
                    cells[x, y].StackVal = val;
                    ret = true;
                }
                else
                {
                    // 最上段は同値の場合のみ着地可能
                    if (val == belowVal)
                    {
                        cells[x, y].StackVal = val;
                        ret = true;
                    }
                }
            }
        }

        return ret;
    }

    //----------------------------------------------------------------------
    // スタック実行
    // C++: ExecStack
    //----------------------------------------------------------------------
    public void ExecStack()
    {
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                if (cells[x, y].StackVal >= 0)
                {
                    cells[x, y].Val = cells[x, y].StackVal;
                    cells[x, y].StackVal = -1;
                }
            }
    }

    //----------------------------------------------------------------------
    // マージ可能判定
    // C++: CheckMerge
    //
    // startX から左右へ走査し、連結している同値セルを探索する
    // 探索結果は MergeX / MergeY / MergeVal に保持される
    //----------------------------------------------------------------------
    public bool CheckMerge(int startX, int startY, out int maxMergeVal, out int maxMergeDifVal)
    {
        bool ret = false;
        maxMergeVal = -1;
        maxMergeDifVal = -1;

        // マージ情報初期化
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                cells[x, y].MergeX = -1;
                cells[x, y].MergeY = -1;
                cells[x, y].MergeVal = -1;
            }

        // startX から右方向
        for (int x = startX; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                if (CheckMergeSub(x, y, x, y) > 1)
                {
                    ret = true;
                }
            }
        }

        // startX の左方向
        for (int x = startX - 1; x >= 0; x--)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                if (CheckMergeSub(x, y, x, y) > 1)
                {
                    ret = true;
                }
            }
        }

        // 最大マージ値と規模を算出
        if (ret)
        {
            for (int x = 0; x < GameDef.CELL_X_NUM; x++)
                for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
                {
                    if (cells[x, y].Val < cells[x, y].MergeVal)
                    {
                        maxMergeVal = Math.Max(maxMergeVal, cells[x, y].MergeVal);
                        maxMergeDifVal = Math.Max(maxMergeDifVal, cells[x, y].MergeVal - cells[x, y].Val);
                    }
                }
        }

        return ret;
    }

    //----------------------------------------------------------------------
    // 連結セル探索（DFS）
    // C++: CheckMergeSub
    //
    // baseセルと同じ値の連結数を数え、
    // 基準セルには最終的な MergeVal を設定する
    //----------------------------------------------------------------------
    private int CheckMergeSub(int x, int y, int baseX, int baseY)
    {
        int ret = 0;

        if (!InRange(x, y)) return 0;
        if (!InRange(baseX, baseY)) return 0;

        if (cells[x, y].Val != -1 &&
            cells[x, y].MergeX == -1 &&
            cells[x, y].MergeY == -1)
        {
            if (cells[x, y].Val == cells[baseX, baseY].Val)
            {
                ret = 1;
                cells[x, y].MergeX = baseX;
                cells[x, y].MergeY = baseY;

                // 4方向探索
                ret += CheckMergeSub(x - 1, y, baseX, baseY);
                ret += CheckMergeSub(x + 1, y, baseX, baseY);
                ret += CheckMergeSub(x, y - 1, baseX, baseY);
                ret += CheckMergeSub(x, y + 1, baseX, baseY);

                // 基準セルだけ最終値を持つ
                if (x == baseX && y == baseY)
                {
                    cells[x, y].MergeVal = cells[x, y].Val + ret - 1;
                }
            }
        }

        return ret;
    }

    //----------------------------------------------------------------------
    // マージ実行
    // C++: ExecMerge
    //
    // 各セルを MergeVal に置き換え、マージ情報をクリアする
    //----------------------------------------------------------------------
    public void ExecMerge()
    {
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                cells[x, y].Val = cells[x, y].MergeVal;
                cells[x, y].MergeX = -1;
                cells[x, y].MergeY = -1;
                cells[x, y].MergeVal = -1;
            }
    }

    //----------------------------------------------------------------------
    // 落下補正可能判定
    // C++: CheckEnLand
    //
    // 空中に浮いているセルを検出し、LandY に落下先を記録する
    //----------------------------------------------------------------------
    public bool CheckEnLand()
    {
        bool ret = false;

        // 落下先情報初期化
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                cells[x, y].LandY = -1;
            }

        // 下段から順に落下先を探索
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = GameDef.CELL_Y_NUM - 1; y >= 0; y--)
            {
                if (CheckEnLandSub(x, y))
                {
                    ret = true;
                }
            }
        }

        return ret;
    }

    //----------------------------------------------------------------------
    // 単一セルの落下先探索
    // C++: CheckEnLandSub
    //----------------------------------------------------------------------
    private bool CheckEnLandSub(int x, int y)
    {
        bool ret = false;

        if (!InRange(x, y)) return false;

        if (cells[x, y].Val != -1)
        {
            if (y < GameDef.CELL_Y_NUM - 1)
            {
                for (int cellY = y + 1; cellY < GameDef.CELL_Y_NUM; cellY++)
                {
                    if (cells[x, cellY].Val == -1)
                    {
                        // 空きがあればそこへ落ちる
                        cells[x, y].LandY = cellY;
                        ret = true;
                    }
                    else
                    {
                        // 下が先に落ちる場合はその上に詰める
                        if (cells[x, cellY].LandY >= 0)
                        {
                            cells[x, y].LandY = cells[x, cellY].LandY - 1;
                            ret = true;
                        }
                        break;
                    }
                }
            }
        }

        return ret;
    }

    //----------------------------------------------------------------------
    // 落下補正実行
    // C++: ExecLand
    //----------------------------------------------------------------------
    public void ExecLand()
    {
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = GameDef.CELL_Y_NUM - 1; y >= 0; y--)
            {
                int landY = cells[x, y].LandY;

                if (landY != -1)
                {
                    cells[x, landY].Val = cells[x, y].Val;
                    cells[x, y].Val = -1;
                }

                cells[x, y].LandY = -1;
            }
        }
    }

    //----------------------------------------------------------------------
    // スコア再計算
    // C++: CalcScore
    //
    // 盤面上の値を 2^n へ変換して総和を取る
    //----------------------------------------------------------------------
    public bool CalcScore()
    {
        bool ret = false;
        long newScore = 0;

        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                int v = cells[x, y].Val;

                if (v >= 0 && v <= 30)
                {
                    newScore += (1 << v);
                }

                if (newScore < 0)
                {
                    break;
                }
            }

            if (newScore < 0)
            {
                break;
            }
        }

        if (newScore >= 0)
        {
            score = (int)newScore;
            ret = true;
        }

        return ret;
    }

    //----------------------------------------------------------------------
    // スコア取得／設定
    //----------------------------------------------------------------------
    public int GetScore()
    {
        return score;
    }

    public void SetScore(int s)
    {
        score = s;
    }

    //----------------------------------------------------------------------
    // セーブデータ互換用セル復元
    //
    // struct は値型なので、ローカル変数へ代入後に
    // 最後に cells[x, y] へ書き戻す必要がある
    //----------------------------------------------------------------------
    public void SetCompatCell(
        int x, int y,
        int val, int stackVal, int mergeX, int mergeY, int mergeVal, int landY, int landVal)
    {
        var c = cells[x, y];

        c.Val = val;
        c.StackVal = stackVal;
        c.MergeX = mergeX;
        c.MergeY = mergeY;
        c.MergeVal = mergeVal;
        c.LandY = landY;
        c.LandVal = landVal;

        cells[x, y] = c;
    }
}
