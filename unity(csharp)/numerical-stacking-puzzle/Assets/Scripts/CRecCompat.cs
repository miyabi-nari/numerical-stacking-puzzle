//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// C++版セーブデータ互換のシリアライズ／デシリアライズクラス
// 独自テキストフォーマットで int配列を保存・復元する
// C++版相当クラス: CRAppli(subset),CRec
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui
//  +- CRMidSnd
//  +- CRAppli === CRecCompat(subset)
//      +- CRCellDat
//      +- CRNextDat
//      +- CRec === CRecCompat
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
using System.Collections.Generic;
using System.IO;
using System.Text;

//----------------------------------------------------------------------
// C++版セーブデータ互換のシリアライズ／デシリアライズクラス
// 独自テキストフォーマットで int配列を保存・復元する
//----------------------------------------------------------------------
public static class CRecCompat
{
    //----------------------------------------------------------------------
    // セーブ
    // entries をテキスト形式へ変換してファイル出力
    //
    // フォーマット:
    // key:<name>
    // size:<n>
    // val,val,val,...
    // ...
    // end:
    //----------------------------------------------------------------------
    public static void Save(string path, List<RecEntry> entries)
    {
        var sb = new StringBuilder();

        foreach (var e in entries)
        {
            sb.Append("key:").Append(e.Key).Append('\n');
            sb.Append("size:").Append(e.Data.Length).Append('\n');

            for (int i = 0; i < e.Data.Length; i++)
            {
                sb.Append(e.Data[i]).Append(",");

                // C++互換：8個ごとに改行
                if ((i & 7) == 7)
                    sb.Append('\n');
            }

            sb.Append('\n');
        }

        sb.Append("end:\n");

        File.WriteAllText(path, sb.ToString(), Encoding.ASCII);
    }

    //----------------------------------------------------------------------
    // ロード
    // テキスト形式から key → int配列 の辞書を構築
    //----------------------------------------------------------------------
    public static Dictionary<string, int[]> Load(string path)
    {
        if (!File.Exists(path))
            return null;

        string text = File.ReadAllText(path, Encoding.ASCII);
        var lines = text.Split('\n');

        var result = new Dictionary<string, int[]>();

        string currentKey = null;
        var values = new List<int>();

        foreach (var raw in lines)
        {
            string line = raw.TrimEnd('\r');

            // key開始
            if (line.StartsWith("key:"))
            {
                // 前エントリを確定
                if (currentKey != null)
                {
                    result[currentKey] = values.ToArray();
                    values.Clear();
                }

                currentKey = line.Substring(4);
            }
            // sizeは互換のため存在（実際は使用しない）
            else if (line.StartsWith("size:"))
            {
                // 読み飛ばし
            }
            // ファイル終端
            else if (line.StartsWith("end:"))
            {
                if (currentKey != null)
                {
                    result[currentKey] = values.ToArray();
                }
                break;
            }
            // データ行
            else if (!string.IsNullOrWhiteSpace(line))
            {
                string[] parts = line.Split(',');

                foreach (var p in parts)
                {
                    // 空要素を除外しつつ数値変換
                    if (int.TryParse(p, out int val))
                        values.Add(val);
                }
            }
        }

        return result;
    }
}

//----------------------------------------------------------------------
// セーブ用エントリ（key + int配列）
//----------------------------------------------------------------------
public class RecEntry
{
    public string Key;
    public int[] Data;

    public RecEntry(string key, int[] data)
    {
        Key = key;
        Data = data;
    }
}
