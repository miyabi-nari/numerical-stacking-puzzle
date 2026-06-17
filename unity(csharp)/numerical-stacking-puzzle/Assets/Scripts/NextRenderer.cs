//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// NEXT表示・演出クラス（View層）
// GameNextQueueの状態を表示し、
// 「スライド投入演出（C++風）」を担当する
// C++版相当クラス: CRAppNextBoxSet,CRAppBox,CRMidGui
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui === NextRenderer
//  +- CRMidSnd
//  +- CRAppli
//      +- CRCellDat
//      +- CRNextDat
//      +- CRec
//      +- CRTitleLblSet
//      |   +- CRTitleLbl
//      +- CRAppBoxSet
//      |   +- CRAppBox
//      +- CRAppNextBoxSet === NextRenderer
//      |   +- CRAppBox === NextRenderer
//      +- CRAppKeyIndSet
//      |   +- CRAppKeyInd
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using UnityEngine;
using TMPro;
using System.Collections.Generic;

//----------------------------------------------------------------------
// NEXT表示・演出クラス（View層）
// GameNextQueueの状態を表示し、
// 「スライド投入演出（C++風）」を担当する
//----------------------------------------------------------------------
public class NextRenderer : MonoBehaviour
{
    public GameObject tilePrefab;
    private GameNextQueue gameNextQueue;

    //--------------------------------------------------------------
    // 固定表示用タイル（常時表示）
    //--------------------------------------------------------------
    private GameObject[] nextTiles;

    //--------------------------------------------------------------
    // スライド演出用ゴースト
    //--------------------------------------------------------------
    private class NextSlideGhost
    {
        public GameObject obj;
        public Vector3 from;
        public Vector3 to;
    }

    private List<NextSlideGhost> slideGhosts = new List<NextSlideGhost>();
    private bool sliding = false;
    private float slideProgress = 0f;

    //--------------------------------------------------------------
    // 表示位置（右側横並び）
    // 左端が次に落下するブロック
    //--------------------------------------------------------------
    private Vector3[] slotPositions =
    {
        new Vector3(6.5f, 3.0f, 0f), // 左端（次に出る）
        new Vector3(7.5f, 3.0f, 0f),
        new Vector3(8.5f, 3.0f, 0f),
        new Vector3(9.5f, 3.0f, 0f)  // 右端（最新）
    };

    //--------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------
    public void Init(GameNextQueue gameNextQueue)
    {
        this.gameNextQueue = gameNextQueue;

        nextTiles = new GameObject[4];

        for (int i = 0; i < nextTiles.Length; i++)
        {
            GameObject obj = Instantiate(tilePrefab, transform);

            obj.name = "NextTile_" + i;
            obj.transform.position = slotPositions[i];
            obj.transform.localScale = new Vector3(1.0f, 1.0f, 1f);

            nextTiles[i] = obj;
        }
    }

    //--------------------------------------------------------------
    // 毎フレーム更新
    //--------------------------------------------------------------
    public void Update()
    {
        UpdateQueTile();
        UpdateSlide();
    }

    //--------------------------------------------------------------
    // スライド演出更新
    //--------------------------------------------------------------
    void UpdateSlide()
    {
        if (!sliding) return;

        float t = Mathf.SmoothStep(0f, 1f, Mathf.Clamp01(slideProgress));

        foreach (var g in slideGhosts)
        {
            if (g.obj == null) continue;

            Vector3 pos = Vector3.Lerp(g.from, g.to, t);
            g.obj.transform.position = pos;
        }
    }

    //--------------------------------------------------------------
    // 通常表示（スライド中は非表示）
    //--------------------------------------------------------------
    void UpdateQueTile()
    {
        for (int i = 0; i < 4; i++)
        {
            if (nextTiles[i] == null) continue;

            if (sliding)
            {
                // 演出中は固定表示を隠す
                nextTiles[i].SetActive(false);
            }
            else
            {
                nextTiles[i].SetActive(true);
                nextTiles[i].transform.position = slotPositions[i];
                nextTiles[i].transform.localScale = new Vector3(1.0f, 1.0f, 1f);

                ApplyTileVisual(nextTiles[i], gameNextQueue.GetVal(i));
            }
        }
    }

    //--------------------------------------------------------------
    // スライド開始（NEXT → NEXT+落下）
    //
    // old[0] → 落下位置へ
    // old[1..3] → 左へ詰める
    // new[3] → 右から追加
    //--------------------------------------------------------------
    public void StartSlide(int oldTop, Vector3 launchTargetPosition)
    {
        EndSlideImmediate();

        sliding = true;
        slideProgress = 0f;

        // 先頭 → 落下開始位置
        CreateGhost(oldTop, slotPositions[0], launchTargetPosition);

        // 左詰め
        CreateGhost(gameNextQueue.GetVal(0), slotPositions[1], slotPositions[0]);
        CreateGhost(gameNextQueue.GetVal(1), slotPositions[2], slotPositions[1]);
        CreateGhost(gameNextQueue.GetVal(2), slotPositions[3], slotPositions[2]);

        // 新規追加（右外 → 右端）
        Vector3 newEntryPos = new Vector3(slotPositions[3].x + 1.0f, slotPositions[3].y, slotPositions[3].z);
        CreateGhost(gameNextQueue.GetVal(3), newEntryPos, slotPositions[3]);
    }

    //--------------------------------------------------------------
    // スライド進行（0?1）
    //--------------------------------------------------------------
    public void SetSlideProgress(float progress)
    {
        slideProgress = progress;
    }

    //--------------------------------------------------------------
    // スライド終了（固定表示へ戻す）
    //--------------------------------------------------------------
    public void EndSlide()
    {
        EndSlideImmediate();
        sliding = false;
    }

    private void EndSlideImmediate()
    {
        foreach (var g in slideGhosts)
        {
            if (g.obj != null)
            {
                Destroy(g.obj);
            }
        }

        slideGhosts.Clear();
        sliding = false;
    }

    //--------------------------------------------------------------
    // ゴースト生成
    //--------------------------------------------------------------
    private void CreateGhost(int value, Vector3 from, Vector3 to)
    {
        GameObject obj = Instantiate(tilePrefab, transform);

        obj.name = "NextSlideGhost";
        obj.transform.position = from;
        obj.transform.localScale = new Vector3(1.0f, 1.0f, 1f);

        ApplyTileVisual(obj, value);

        NextSlideGhost g = new NextSlideGhost();
        g.obj = obj;
        g.from = from;
        g.to = to;

        slideGhosts.Add(g);
    }

    //--------------------------------------------------------------
    // タイル見た目設定（色・文字）
    //--------------------------------------------------------------
    private void ApplyTileVisual(GameObject tile, int value)
    {
        var sr = tile.GetComponent<SpriteRenderer>();
        if (sr != null)
        {
            sr.color = GetColor(value);
            sr.sortingOrder = 20;
        }

        var text = tile.GetComponentInChildren<TextMeshPro>();
        if (text != null)
        {
            text.text = ToDisplayLabel(value);
            text.fontSize = GetFontsize(value);

            var r = text.GetComponent<Renderer>();
            if (r != null)
            {
                r.sortingOrder = 21;
            }
        }
    }

    //--------------------------------------------------------------
    // Utility
    //--------------------------------------------------------------
    private string ToDisplayLabel(int exponent)
    {
        if (exponent < 0) return "";

        if (exponent <= 30)
        {
            int v = 1 << exponent;
            return v.ToString();
        }

        return "2^" + exponent;
    }

    // 値に応じた色
    Color GetColor(int value)
    {
        switch (value)
        {
            case 0: return new Color(0.50f, 0.50f, 0.50f);
            case 1: return new Color(0.25f, 0.25f, 0.75f);
            case 2: return new Color(0.75f, 0.25f, 0.25f);
            case 3: return new Color(0.75f, 0.25f, 0.75f);
            case 4: return new Color(0.25f, 0.75f, 0.25f);
            case 5: return new Color(0.25f, 0.75f, 0.75f);
            case 6: return new Color(0.75f, 0.75f, 0.25f);
            case 7: return new Color(0.75f, 0.75f, 0.75f);
            case 8: return new Color(0.75f, 0.75f, 1.00f);
            case 9: return new Color(1.00f, 0.75f, 0.75f);
            case 10: return new Color(1.00f, 0.75f, 1.00f);
            case 11: return new Color(0.75f, 1.00f, 0.75f);
            case 12: return new Color(0.75f, 1.00f, 1.00f);
            case 13: return new Color(1.00f, 1.00f, 0.75f);
            case 14: return new Color(1.00f, 1.00f, 1.00f);
            case 15: return new Color(0.125f, 0.125f, 0.125f);
            case 16: return new Color(0.125f, 0.125f, 0.50f);
            case 17: return new Color(0.50f, 0.125f, 0.125f);
            case 18: return new Color(0.50f, 0.125f, 0.50f);
            case 19: return new Color(0.125f, 0.50f, 0.125f);
            case 20: return new Color(0.125f, 0.50f, 0.50f);
            default: return Color.white;
        }
    }

    // 値に応じたフォントサイズ
    float GetFontsize(int value)
    {
        switch (value)
        {
            case 0: return 0.64f * 10.0f;
            case 1: return 0.64f * 10.0f;
            case 2: return 0.64f * 10.0f;
            case 3: return 0.64f * 10.0f;
            case 4: return 0.48f * 10.0f;
            case 5: return 0.48f * 10.0f;
            case 6: return 0.48f * 10.0f;
            case 7: return 0.40f * 10.0f;
            case 8: return 0.40f * 10.0f;
            case 9: return 0.40f * 10.0f;
            case 10: return 0.32f * 10.0f;
            case 11: return 0.32f * 10.0f;
            case 12: return 0.32f * 10.0f;
            case 13: return 0.32f * 10.0f;
            case 14: return 0.28f * 10.0f;
            case 15: return 0.28f * 10.0f;
            case 16: return 0.28f * 10.0f;
            case 17: return 0.22f * 10.0f;
            case 18: return 0.22f * 10.0f;
            case 19: return 0.22f * 10.0f;
            case 20: return 0.20f * 10.0f;
            default: return 0.20f * 10.0f;
        }
    }

    //--------------------------------------------------------------
    // 全タイル削除（タイトル遷移時）
    //--------------------------------------------------------------
    public void ClearAllTiles()
    {
        if (nextTiles == null) return;

        foreach (var tile in nextTiles)
        {
            if (tile != null)
                GameObject.Destroy(tile);
        }

        nextTiles = null;
    }
}
