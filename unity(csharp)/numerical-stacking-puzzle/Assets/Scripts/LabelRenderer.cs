//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// ラベル描画クラス（UI View）
// スコア・時間・マージ履歴などのテキスト表示を管理する
// C++版相当クラス: CRAppLblSet,CRAppLbl
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui
//  +- CRMidSnd
//  +- CRAppli
//      +- CRCellDat
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
//      +- CRAppLblSet === LabelRenderer
//      |   +- CRAppLbl === LabelRenderer
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using UnityEngine;
using TMPro;

//----------------------------------------------------------------------
// ラベル描画クラス（UI View）
// スコア・時間・マージ履歴などのテキスト表示を管理する
//----------------------------------------------------------------------
public class LabelRenderer : MonoBehaviour
{
    //--------------------------------------------------------------
    // UI参照
    //--------------------------------------------------------------
    public TextMeshProUGUI nextLabel;
    public TextMeshProUGUI scoreLabel;
    public TextMeshProUGUI scoreValue;
    public TextMeshProUGUI elapsedTimeLabel;
    public TextMeshProUGUI elapsedTimeValue;
    public TextMeshProUGUI mergedTimesLabel;
    public TextMeshProUGUI[] mergedTimesValue;   // 上位5件

    //--------------------------------------------------------------
    // 内部表示用データ（GameControllerから受け取る値）
    //--------------------------------------------------------------
    private int scoreRaw = 0;
    private float elapsedTimeRaw = 0f;

    private int[] mergedTimesRaw_Value = new int[5];
    private float[] mergedTimesRaw_Time = new float[5];

    //--------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------
    public void Init()
    {
        ClearMergedTimes();
    }

    //--------------------------------------------------------------
    // 毎フレーム更新
    //--------------------------------------------------------------
    void Update()
    {
        UpdateScore();
        UpdateElapsedTime();
        UpdateMergedTime();
    }
    
    //--------------------------------------------------------------
    // スコア表示更新
    //--------------------------------------------------------------
    void UpdateScore()
    {
        if (scoreValue != null)
        {
            scoreValue.text = scoreRaw.ToString();
        }
    }

    //--------------------------------------------------------------
    // 経過時間表示（hh:mm:ss.ff）
    //--------------------------------------------------------------
	void UpdateElapsedTime()
    {
        // 内部は秒 → 表示は 1/100秒単位
        int total = Mathf.FloorToInt(elapsedTimeRaw * 100);

        int msec = total % 100;
        total /= 100;

        int sec = total % 60;
        int min = (total / 60) % 60;
        int hour = total / (60 * 60);

        elapsedTimeValue.text = $"{hour:0}:{min:00}:{sec:00}.{msec:00}";
    }

    //--------------------------------------------------------------
    // マージ履歴表示（上位5件）
    //--------------------------------------------------------------
    public void UpdateMergedTime()
    {
        if (mergedTimesRaw_Value == null) return;
        if (mergedTimesRaw_Time == null) return;

        for (int i = 0; i < mergedTimesRaw_Value.Length &&
                        i < mergedTimesRaw_Time.Length &&
                        i < mergedTimesValue.Length; i++)
        {
            if (mergedTimesValue[i] != null)
            {
	            int exponent = mergedTimesRaw_Value[i];
	            float firstElapsed = mergedTimesRaw_Time[i];
	
	            if (exponent >= 0)
	            {
	                string label = ToDisplayLabel(exponent);
	
	                // 時間フォーマット（UpdateElapsedTimeと同様）
	                int total = Mathf.FloorToInt(firstElapsed * 100);
	
	                int msec = total % 100;
	                total /= 100;
	
	                int sec = total % 60;
	                int min = (total / 60) % 60;
	                int hour = total / (60 * 60);
	
	                string timeText = $"{hour:0}:{min:00}:{sec:00}.{msec:00}";
	
	                // ラベル + 時刻
	                mergedTimesValue[i].text = $"{label,4} : {timeText}";
	            }
	            else
	            {
	                mergedTimesValue[i].text = "-";
	            }
            }
        }
    }

    //--------------------------------------------------------------
    // 表示用ラベル（指数 → 2^n または数値）
    //--------------------------------------------------------------
    private string ToDisplayLabel(int exponent)
    {
        if (exponent < 0) return "";

        if (exponent <= 30)
        {
            int v = 1 << exponent;
            return v.ToString() + "...";
        }

        return "2^" + exponent + "...";
    }

    //--------------------------------------------------------------
    // 外部からのデータ設定（GameControllerから呼ばれる）
    //--------------------------------------------------------------
    public void SetScore(int score)
    {
        scoreRaw = score;
    }

    public void SetElapsedTime(float elapsedTime)
    {
        elapsedTimeRaw = elapsedTime;
    }

    public void SetMergedTimes(int[] mergeTimesValue, float[] mergeTimesTime)
    {
        if (mergedTimesRaw_Value == null) return;
        if (mergedTimesRaw_Time == null) return;

        ClearMergedTimes();

        // 値コピー（サイズ超過防止）
		int i;
        for (i = 0; i < mergedTimesRaw_Value.Length &&
                    i < mergeTimesValue.Length; i++)
        {
            mergedTimesRaw_Value[i] = mergeTimesValue[i];
        }
        for (i = 0; i < mergedTimesRaw_Time.Length &&
                    i < mergeTimesTime.Length; i++)
        {
            mergedTimesRaw_Time[i] = mergeTimesTime[i];
        }
    }

    //--------------------------------------------------------------
    // マージ履歴初期化
    //--------------------------------------------------------------
    void ClearMergedTimes()
    {
        if (mergedTimesRaw_Value == null) return;
        if (mergedTimesRaw_Time == null) return;

		int i;
        for (i = 0; i < mergedTimesRaw_Value.Length; i++)
        {
            mergedTimesRaw_Value[i] = -1;
        }

        for (i = 0; i < mergedTimesRaw_Time.Length; i++)
        {
            mergedTimesRaw_Time[i] = 0f;
        }
    }
}
