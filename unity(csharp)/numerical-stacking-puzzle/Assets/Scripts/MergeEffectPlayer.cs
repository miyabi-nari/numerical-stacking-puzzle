//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// マージエフェクト再生クラス
// 生成されたセルに対して「拡大＋フェードアウト」を行い、
// 再生終了後、自動で破棄される一時オブジェクト
// C++版相当クラス: なし(unity版original)
//----------------------------------------------------------------------
using UnityEngine;
using TMPro;

//----------------------------------------------------------------------
// マージエフェクト再生クラス
// 生成されたセルに対して「拡大＋フェードアウト」を行い、
// 再生終了後、自動で破棄される一時オブジェクト
//----------------------------------------------------------------------
public class MergeEffectPlayer : MonoBehaviour
{
    private SpriteRenderer sr;
    private TextMeshPro tmp;

    // エフェクト全体時間
    private float duration = 0.25f;
    private float timer = 0f;

    // スケール補間
    private Vector3 startScale;
    private Vector3 endScale;

    // 色補間（フェード）
    private Color startColor;
    private Color endColor;

    //--------------------------------------------------------------
    // 初期化（生成直後に呼ばれる）
    //--------------------------------------------------------------
    public void Init(int value)
    {
        sr = GetComponent<SpriteRenderer>();
        tmp = GetComponentInChildren<TextMeshPro>();

        // 拡大アニメーション設定
        startScale = transform.localScale;
        endScale = startScale * 1.8f;

        // フェードアウト設定
        startColor = new Color(1f, 1f, 1f, 0.9f);
        endColor   = new Color(1f, 1f, 1f, 0f);

        // スプライト初期状態
        if (sr != null)
        {
            sr.color = startColor;
            sr.sortingOrder = 100; // 前面表示
        }

        // テキスト設定
        if (tmp != null)
        {
            tmp.text = ToDisplayLabel(value);
            tmp.color = new Color(1f, 1f, 1f, 1f);

            var renderer = tmp.GetComponent<Renderer>();
            if (renderer != null)
            {
                renderer.sortingOrder = 101;
            }
        }
    }

    //--------------------------------------------------------------
    // 毎フレーム更新
    // 拡大＋フェードを行い、終了後に自動破棄
    //--------------------------------------------------------------
    void Update()
    {
        timer += Time.deltaTime;

        float t = timer / duration;

        // 拡大（中心から膨らむ）
        transform.localScale = Vector3.Lerp(startScale, endScale, t);

        // スプライトフェード
        if (sr != null)
        {
            sr.color = Color.Lerp(startColor, endColor, t);
        }

        // テキストフェード
        if (tmp != null)
        {
            Color c = tmp.color;
            c.a = Mathf.Lerp(1f, 0f, t);
            tmp.color = c;
        }

        // 再生終了 → 自動削除
        if (timer >= duration)
        {
            Destroy(gameObject);
        }
    }

    //--------------------------------------------------------------
    // 表示ラベル（指数 → 数値 or 2^n）
    //--------------------------------------------------------------
    string ToDisplayLabel(int exponent)
    {
        if (exponent < 0) return "";

        if (exponent <= 30)
        {
            int v = 1 << exponent;
            return v.ToString();
        }

        return "2^" + exponent;
    }
}
