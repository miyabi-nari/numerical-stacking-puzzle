//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// 盤面描画クラス（View層）
// GameBoard（データ）を元に、タイル・落下・演出を描画する
// C++版相当クラス: CRAppBoxSet,CRAppBox,CRAppBox(FallingBox),CRMidGui
//----------------------------------------------------------------------
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui === BoardRenderer
//  +- CRMidSnd
//  +- CRAppli
//      +- CRCellDat
//      +- CRNextDat
//      +- CRec
//      +- CRTitleLblSet
//      |   +- CRTitleLbl
//      +- CRAppBoxSet === BoardRenderer
//      |   +- CRAppBox === BoardRenderer
//      +- CRAppNextBoxSet
//      |   +- CRAppBox
//      +- CRAppKeyIndSet
//      |   +- CRAppKeyInd
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox) === BoardRenderer
//----------------------------------------------------------------------
using UnityEngine;
using TMPro;
using System.Collections.Generic;

public class BoardRenderer : MonoBehaviour
{
    public GameObject tilePrefab;
    public GameObject mergeEffectPrefab;

    private GameObject[,] tiles;    // 固定タイル
    private GameObject fallingTile; // 落下中タイル

    private GameBoard gameBoard;
    
    // GameBoardの描画時の座標オフセット(原点位置)
    public Vector3 boardOffset = new Vector3(-5.5f, -3.0f, 0f);

    //--------------------------------------------------------------
    // 落下中タイル状態（GameControllerから受け取る）
    //--------------------------------------------------------------
    private bool m_hasFalling = false;
    private int m_fallingX = 0;
    private float m_fallingY = 0f;
    private int m_fallingValue = -1;

    //--------------------------------------------------------------
    // Merge Preview（マージ演出用ゴースト）
    //--------------------------------------------------------------
    private class MergeGhost
    {
        public GameObject obj;
        public Vector2 from;
        public Vector2 to;
        public int value;
    }

    private List<MergeGhost> mergeGhosts = new List<MergeGhost>();
    private bool mergePreviewActive = false;
    private float mergePreviewProgress = 0f;

    //--------------------------------------------------------------
    // Land Preview（落下補正演出用）
    //--------------------------------------------------------------
    private class LandGhost
    {
        public GameObject obj;
        public Vector2 from;
        public Vector2 to;
        public int value;
    }

    private List<LandGhost> landGhosts = new List<LandGhost>();
    private bool landPreviewActive = false;
    private float landPreviewProgress = 0f;

    //--------------------------------------------------------------
    // 初期化（タイル生成）
    //--------------------------------------------------------------
    public void Init(GameBoard gameBoard)
    {
        this.gameBoard = gameBoard;

        tiles = new GameObject[GameDef.CELL_X_NUM, GameDef.CELL_Y_NUM];

        // 全セル分オブジェクト生成（非表示スタート）
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                GameObject obj = Instantiate(tilePrefab);

                float displayY = DisplayY(y);
                obj.transform.position = new Vector3(x, displayY, 0) + boardOffset;
                obj.SetActive(false);
                tiles[x, y] = obj;
            }
        }

        // 落下中タイル生成（非表示スタート）
        fallingTile = Instantiate(tilePrefab);
        fallingTile.name = "FallingTile";
        fallingTile.SetActive(false);
    }

    //--------------------------------------------------------------
    // 毎フレーム描画更新
    //--------------------------------------------------------------
    public void Update()
    {
        UpdateBoardTile();
        UpdateFallingTile();
        UpdateMergePreview();
        UpdateLandPreview();
    }

    //--------------------------------------------------------------
    // 固定盤面の描画更新
    //--------------------------------------------------------------
    void UpdateBoardTile()
    {
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                int value = gameBoard.GetVal(x, y);
                GameObject tile = tiles[x, y];

                // マージ・落下演出中のタイルの場合、元タイル非表示
                bool hideAsMergeSource = mergePreviewActive && IsMergeSource(x, y);
                bool hideAsLandSource = landPreviewActive && IsLandSource(gameBoard, x, y);

                if (value < 0 || hideAsMergeSource || hideAsLandSource)
                {
                    tile.SetActive(false);
                    continue;
                }

                // マージ・落下演出中でなければ表示
                tile.SetActive(true);

                float displayY = DisplayY(y);
                tile.transform.position = new Vector3(x, displayY, 0) + boardOffset;

                // 色
                var sr = tile.GetComponent<SpriteRenderer>();
                if (sr != null)
                {
                    sr.color = GetColor(value);
                }

                // テキスト
                var text = tile.GetComponentInChildren<TextMeshPro>();
                if (text != null)
                {
                    text.text = ToDisplayLabel(value);
                    text.fontSize = GetFontsize(value);
                }
            }
        }
    }

    //--------------------------------------------------------------
    // 落下タイル描画
    //--------------------------------------------------------------
    void UpdateFallingTile()
    {
        // 落下演出中でなければ非表示
        if (!m_hasFalling)
        {
            fallingTile.SetActive(false);
            return;
        }

        // 落下演出中のみ表示
        fallingTile.SetActive(true);

        float displayY = DisplayY(m_fallingY);
        fallingTile.transform.position = new Vector3(m_fallingX, displayY, -0.1f) + boardOffset;

        // 色
        var sr = fallingTile.GetComponent<SpriteRenderer>();
        if (sr != null)
        {
            sr.color = GetColor(m_fallingValue);
        }

        // テキスト
        var text = fallingTile.GetComponentInChildren<TextMeshPro>();
        if (text != null)
        {
            text.text = ToDisplayLabel(m_fallingValue);
            text.fontSize = GetFontsize(m_fallingValue);
        }
    }

    //--------------------------------------------------------------
    // マージプレビュー（吸い寄せ演出）
    //--------------------------------------------------------------
    void UpdateMergePreview()
    {
        // マージ演出中でなければ処理なし
        if (!mergePreviewActive)
        {
            return;
        }

        // マージ演出中のみ処理する
        float t = mergePreviewProgress;
        t = 1f - (1f - t) * (1f - t); // ease
        t = Mathf.SmoothStep(0f, 1f, t);

        foreach (var g in mergeGhosts)
        {
            if (g.obj == null) continue;

            // スライド移動
            Vector2 pos = Vector2.Lerp(g.from, g.to, t);
            g.obj.transform.position = new Vector3(pos.x, pos.y, -0.15f) + boardOffset;

            // 少し縮む＆フェード
            float scale = Mathf.Lerp(1.0f, 0.85f, t);
            g.obj.transform.localScale = new Vector3(scale, scale, 1f);

            // 色
            var sr = g.obj.GetComponent<SpriteRenderer>();
            if (sr != null)
            {
                Color c = sr.color;
                c.a = Mathf.Lerp(1f, 0.2f, t);
                sr.color = c;
            }

            // テキスト
            var text = g.obj.GetComponentInChildren<TextMeshPro>();
            if (text != null)
            {
                Color c = text.color;
                c.a = Mathf.Lerp(1f, 0.2f, t);
                text.color = c;
            }
        }
    }

    //--------------------------------------------------------------
    // 落下補正プレビュー
    //--------------------------------------------------------------
    void UpdateLandPreview()
    {
        // 落下補正中でなければ処理なし
        if (!landPreviewActive)
        {
            return;
        }

        // 落下補正中のみ処理する
        float t = landPreviewProgress * landPreviewProgress;
        t = Mathf.SmoothStep(0f, 1f, t);

        foreach (var g in landGhosts)
        {
            if (g.obj == null) continue;

            // スライド移動
            Vector2 pos = Vector2.Lerp(g.from, g.to, t);
            g.obj.transform.position = new Vector3(pos.x, pos.y, -0.15f) + boardOffset;

            // 少し縮む
            float scale = Mathf.Lerp(1.0f, 0.95f, t);
            g.obj.transform.localScale = new Vector3(scale, scale, 1f);
        }
    }

    //--------------------------------------------------------------
    // 落下タイルパラメータ設定（外部から呼び出し）
    //--------------------------------------------------------------
    public void SetFallingTile(bool hasFalling, int x, float y, int value)
    {
        m_hasFalling = hasFalling;
        m_fallingX = x;
        m_fallingY = y;
        m_fallingValue = value;
    }

    //--------------------------------------------------------------
    // マージ演出開始
    //--------------------------------------------------------------
    public void StartMergePreview()
    {
        // 直前のマージ演出を終了(念のため)
        EndMergePreview();

        mergePreviewActive = true;
        mergePreviewProgress = 0f;

        // 各セルからゴースト生成
        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                int mx = gameBoard.GetMergeX(x, y);
                int my = gameBoard.GetMergeY(x, y);

                // 対象外のセル...マージ対象外orマージ中心地or空白セルは処理対象外
                if (mx < 0 || my < 0) continue;
                if (mx == x && my == y) continue;
                int value = gameBoard.GetVal(x, y);
                if (value < 0) continue;

                // インスタンス生成
                GameObject obj = Instantiate(tilePrefab);
                obj.name = $"MergeGhost_{x}_{y}";
                obj.transform.position = new Vector3(x, DisplayY(y), -0.15f) + boardOffset;

                // 色
                var sr = obj.GetComponent<SpriteRenderer>();
                if (sr != null)
                {
                    sr.color = GetColor(value);
                    sr.sortingOrder = 50;
                }

                // テキスト
                var text = obj.GetComponentInChildren<TextMeshPro>();
                if (text != null)
                {
                    text.text = ToDisplayLabel(value);
                    text.fontSize = GetFontsize(value);
                    var r = text.GetComponent<Renderer>();
                    if (r != null) r.sortingOrder = 51;
                }

                // 移動ゴースト配置
                MergeGhost g = new MergeGhost
                {
                    obj = obj,
                    from = new Vector2(x, DisplayY(y)),
                    to = new Vector2(mx, DisplayY(my)),
                    value = value
                };
                mergeGhosts.Add(g);
            }
        }
    }

    //--------------------------------------------------------------
    // マージ演出変更(progress変更 0→1に次第に変えていく)
    //--------------------------------------------------------------
    public void SetMergePreviewProgress(float progress)
    {
        mergePreviewProgress = progress;
    }

    //--------------------------------------------------------------
    // マージ演出終了
    //--------------------------------------------------------------
    public void EndMergePreview()
    {
        // 全ゴースト削除
        foreach (var g in mergeGhosts)
        {
            if (g.obj != null)
            {
                Destroy(g.obj);
            }
        }

        mergeGhosts.Clear();
        mergePreviewActive = false;
        mergePreviewProgress = 0f;
    }

    //--------------------------------------------------------------
    // 落下補正演出開始
    //--------------------------------------------------------------
    public void StartLandPreview()
    {
        // 直前の落下補正演出を終了(念のため)
        EndLandPreview();
        landPreviewActive = true;
        landPreviewProgress = 0f;

        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                // 対象外のセル...落下補正対象外or空白セルは処理対象外
                int targetY = gameBoard.GetLandY(x, y);
                if (targetY < 0) continue;
                int value = gameBoard.GetVal(x, y);
                if (value < 0) continue;

                // インスタンス生成
                GameObject obj = Instantiate(tilePrefab);
                obj.name = $"LandGhost_{x}_{y}";
                obj.transform.position = new Vector3(x, DisplayY(y), -0.15f) + boardOffset;

                // 色
                var sr = obj.GetComponent<SpriteRenderer>();
                if (sr != null)
                {
                    sr.color = GetColor(value);
                    sr.sortingOrder = 60;
                }

                // テキスト
                var text = obj.GetComponentInChildren<TextMeshPro>();
                if (text != null)
                {
                    text.text = ToDisplayLabel(value);
                    text.fontSize = GetFontsize(value);
                    var r = text.GetComponent<Renderer>();
                    if (r != null) r.sortingOrder = 61;
                }

                // 落下補正ゴースト配置
                LandGhost g = new LandGhost();
                g.obj = obj;
                g.from = new Vector2(x, DisplayY(y));
                g.to = new Vector2(x, DisplayY(targetY));
                g.value = value;

                landGhosts.Add(g);
            }
        }
    }

    //--------------------------------------------------------------
    // 落下補正演出変更(progress変更 0→1に次第に変えていく)
    //--------------------------------------------------------------
    public void SetLandPreviewProgress(float progress)
    {
        landPreviewProgress = progress;
    }

    //--------------------------------------------------------------
    // 落下補正演出終了
    //--------------------------------------------------------------
    public void EndLandPreview()
    {
        // 全ゴースト削除
        foreach (var g in landGhosts)
        {
            if (g.obj != null)
            {
                Destroy(g.obj);
            }
        }

        landGhosts.Clear();
        landPreviewActive = false;
        landPreviewProgress = 0f;
    }

    //--------------------------------------------------------------
    // マージ確定エフェクト
    //--------------------------------------------------------------
    public void PlayMergeEffect(int x, int y, int value)
    {
        if (mergeEffectPrefab == null)
        {
            Debug.LogError("Merge Effect Prefab が設定されていません。");
            return;
        }

        GameObject fx = Instantiate(mergeEffectPrefab);

        float displayY = DisplayY(y);
        fx.transform.position = new Vector3(x, displayY, -0.2f) + boardOffset;

        var effect = fx.GetComponent<MergeEffectPlayer>();
        if (effect != null)
        {
            effect.Init(value);
        }
    }

    //--------------------------------------------------------------
    // 全タイル削除（タイトル戻り時）
    //--------------------------------------------------------------
    public void ClearAllTiles()
    {
        if (tiles == null) return;

        foreach (var tile in tiles)
        {
            if (tile != null)
                GameObject.Destroy(tile);
        }

        tiles = null;

        if (fallingTile != null)
        {
            GameObject.Destroy(fallingTile);
            fallingTile = null;
        }
    }

    //--------------------------------------------------------------
    // Utility
    //--------------------------------------------------------------
    bool IsMergeSource(int x, int y)
    {
        int mx = gameBoard.GetMergeX(x, y);
        int my = gameBoard.GetMergeY(x, y);

        if (mx < 0 || my < 0) return false;

        // 基準セル以外はスライド元
        return !(mx == x && my == y);
    }

    bool IsLandSource(GameBoard gameBoard, int x, int y)
    {
        return gameBoard.GetLandY(x, y) >= 0;
    }

    float DisplayY(float logicalY)
    {
        return GameDef.CELL_Y_NUM - 1 - logicalY;
    }

    // 表示用テキスト（指数→2^n形式）
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
}
