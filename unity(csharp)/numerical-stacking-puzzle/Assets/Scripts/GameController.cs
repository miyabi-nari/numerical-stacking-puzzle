//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// ゲーム全体の進行管理クラス
// タイトル～ゲームの遷移、および各フェーズ制御を担当
// C++版相当クラス: CRAppli
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui
//  +- CRMidSnd
//  +- CRAppli === GameController
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
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using System.Collections.Generic;
using System.IO;
using System;
using UnityEngine;
using UnityEngine.UI;

//--------------------------------------------------------------
// ゲーム全体の進行管理クラス
// タイトル～ゲームの遷移、および各フェーズ制御を担当
//--------------------------------------------------------------
public class GameController : MonoBehaviour
{
    //--------------------------------------------------------------
    // フェーズ（C++版シーケンス相当）
    //--------------------------------------------------------------
    public enum GamePhase
    {
        Serve,  // NEXT投入演出
        Fall,   // 落下中
        Stack,  // 着地
        Merge,  // マージ
        Land    // 落下補正
    }

    //--------------------------------------------------------------
    // アプリ状態
    //--------------------------------------------------------------
    public enum AppState
    {
        Title,
        Game
    }

    //--------------------------------------------------------------
    // マージで新規生成されるセル（エフェクト用）
    //--------------------------------------------------------------
    private struct MergeCreatedInfo
    {
        public int x;
        public int y;
        public int value;

        public MergeCreatedInfo(int x, int y, int value)
        {
            this.x = x;
            this.y = y;
            this.value = value;
        }
    }

    private AppState appState = AppState.Title;

    //--------------------------------------------------------------
    // 参照（Inspector指定）
    //--------------------------------------------------------------
    [Header("Optional UI")]
    public SoundManager soundManager;
    public InputManager inputManager;
    public GameObject titleUI;
    public GameObject titleView;
    public GameObject gameUI;
    public GameObject gameView;
    public TitleRenderer titleRenderer;
    public BoardRenderer boardRenderer;
    public NextRenderer nextRenderer;
    public LabelRenderer labelRenderer;
    public KeyIndicator keyIndicator;

    private GameBoard gameBoard;

    [Header("Common UI")]
    public Button backButton;
    public Button loadButton;
    public Button saveButton;

    //--------------------------------------------------------------
    // Nextキュー（CRNextDat相当）
    //--------------------------------------------------------------
    private GameNextQueue gameNextQueue;

    //--------------------------------------------------------------
    // 落下中タイル状態
    //--------------------------------------------------------------
    private bool hasFalling = false;
    private int fallingX;
    private int fallingY;
    private int fallingValue;

    // 落下アニメーション補間
    private float fallingPosY;
    private float fallingAnimFromY;
    private float fallingAnimToY;

    //--------------------------------------------------------------
    // フェーズ制御（フレームベース）
    //--------------------------------------------------------------
    private const int FPS = 60;

    private GamePhase phase = GamePhase.Serve;

    private int phaseFrameCounter = 0;
    private int phaseFrameLimit = 0;

    private int fallFrameCounter = 0;
    private int currentFallFrames = 60;
    private bool isFastFall = false;

    // フェーズ毎の演出時間
    private const int SERVE_FRAMES = 15;
    private const int FALL_FRAMES_NORMAL = 60;
    private const int FALL_FRAMES_FAST = 5;
    private const int STACK_FRAMES = 10;
    private const int MERGE_FRAMES = 30;
    private const int LAND_FRAMES = 30;

    //--------------------------------------------------------------
    // キー入力
    //--------------------------------------------------------------
    // キー入力割り当て
    private const int KEY_Z = 0;
    private const int KEY_X = 1;
    private const int KEY_C = 2;
    private const int KEY_V = 3;

    // 入力トリガ（フェーズ依存処理用）
    private bool gameEscapeTrigger = false;
    private bool gameSaveTrigger = false;

    //--------------------------------------------------------------
    // 経過時間・マージ履歴（上位5件）
    //--------------------------------------------------------------
    private float elapsedTime = 0f;
    private float[] mergeTimesArray_Time = new float[5];
    private int[] mergeTimesArray_Value = new int[5];

    //--------------------------------------------------------------
	// 内部処理用マージ状態
    //--------------------------------------------------------------
    private int pendingMaxMergeVal;
    private int pendingMaxMergeDifVal;

    //--------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------
    void Start()
    {
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = FPS;

        appState = AppState.Title;

        if (titleUI != null) titleUI.SetActive(true);
        if (titleView != null) titleView.SetActive(true); else titleView.SetActive(false);
        if (gameUI != null) gameUI.SetActive(false);
        if (gameView != null) gameView.SetActive(false);
    }

    //--------------------------------------------------------------
    // UI更新
    //--------------------------------------------------------------
    void UpdateScoreUI()
    {
        labelRenderer?.SetScore(gameBoard.GetScore());
    }

    // 共通UI（ボタン）の状態切り替え
    void UpdateCommonUI()
    {
        if (appState == AppState.Title)
        {
            backButton.interactable = true;
            loadButton.interactable = true;
            saveButton.interactable = false;
        }
        else
        {
            backButton.interactable = true;
            loadButton.interactable = false;

            // 一定スコア以上で保存許可（初期盤面回避）
            saveButton.interactable = (gameBoard.GetScore() >= (1+2+4+8+16+32));
        }
    }

    //--------------------------------------------------------------
    // マージ履歴更新（上位5件を保持）
    // 最大値を優先し、値が大きい順に並べ替える
    //--------------------------------------------------------------
    void UpdateMergeTime(int maxMergeVal)
    {
        if (maxMergeVal < 0) return;

        int n;

        // 既存エントリ確認（重複防止）
        for (n = 0; n < 5; n++)
        {
            if (maxMergeVal == mergeTimesArray_Value[n])
                return;
        }

        float mergeTime = elapsedTime;

        // 挿入ソート方式で入れ替え
        for (n = 0; n < 5; n++)
        {
            if (maxMergeVal >= mergeTimesArray_Value[n])
            {
                // 値スワップ
                int tmpVal = mergeTimesArray_Value[n];
                mergeTimesArray_Value[n] = maxMergeVal;
                maxMergeVal = tmpVal;

                float tmpTime = mergeTimesArray_Time[n];
                mergeTimesArray_Time[n] = mergeTime;
                mergeTime = tmpTime;
            }
        }
    }

    //--------------------------------------------------------------
    // 落下アニメーション補間（描画専用）
    //--------------------------------------------------------------
    void UpdateFallingAnimation()
    {
        // 落下中または着地直前のみ表示
        bool showFalling =
            (phase == GamePhase.Fall || phase == GamePhase.Stack) && hasFalling;

        if (showFalling)
        {
            // フレーム進行に応じて位置補間
	        float frameProgress = 0f;
	        if (currentFallFrames > 0)
	        {
	            frameProgress = Mathf.Clamp01((float)fallFrameCounter / currentFallFrames);
	        }

            fallingPosY = Mathf.Lerp(fallingAnimFromY, fallingAnimToY, frameProgress);
        }

        boardRenderer.SetFallingTile(
            showFalling,
            fallingX,
            fallingPosY,
            fallingValue
        );
    }

    // 論理座標 → 表示座標変換（Y反転）
    float DisplayY(float logicalY)
    {
        return GameDef.CELL_Y_NUM - 1 - logicalY;
    }

    //--------------------------------------------------------------
    // セーブ／ロード（C++互換フォーマット）
    //--------------------------------------------------------------
    void SaveGameCompat()
    {
        string path = Path.Combine(Application.persistentDataPath, "save.txt");

        List<RecEntry> entries = new List<RecEntry>();

        // 識別ID
        entries.Add(new RecEntry(GameDef.APP_SAVE_ID_KEY, new int[] { GameDef.APP_SAVE_ID_NUM }));

        // 盤面
        entries.Add(new RecEntry("CellData", ExportCellDataCompat()));

        // スコア
        entries.Add(new RecEntry("Score", new int[] { gameBoard.GetScore() }));

        // NEXT情報
        entries.Add(new RecEntry("NextData", ExportQueDataCompat()));
        entries.Add(new RecEntry("NextBoxCnt", new int[] { gameNextQueue.GetNextBoxCount() }));
        entries.Add(new RecEntry("BoxValTime", new int[] { gameNextQueue.GetBoxValTimer() }));

        // 落下中状態
        entries.Add(new RecEntry("FallX", new int[] { fallingX }));
        entries.Add(new RecEntry("FallY", new int[] { fallingY }));
        entries.Add(new RecEntry("FallVal", new int[] { fallingValue }));

        // 経過時間（フレーム換算）
        entries.Add(new RecEntry("ElapsedTime", new int[] { (int)(elapsedTime * 60) }));

        // マージ履歴
        entries.Add(new RecEntry("MergeTime", Array.ConvertAll(mergeTimesArray_Time, x => (int)(x * 60))));
        entries.Add(new RecEntry("MergeVal", mergeTimesArray_Value));

        CRecCompat.Save(path, entries);
    }

    //--------------------------------------------------------------
    // セルデータを書き出し（C++互換フォーマット）
    // CellDat構造をフラット配列に変換
    //--------------------------------------------------------------
    int[] ExportCellDataCompat()
    {
        int width = GameDef.CELL_X_NUM;
        int height = GameDef.CELL_Y_NUM;

        int[] arr = new int[width * height * 7];
        int idx = 0;

        // x優先ループ（C++版と同一順序）
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                arr[idx++] = gameBoard.GetVal(x, y);
                arr[idx++] = gameBoard.GetStackVal(x, y);
                arr[idx++] = gameBoard.GetMergeX(x, y);
                arr[idx++] = gameBoard.GetMergeY(x, y);
                arr[idx++] = gameBoard.GetMergeVal(x, y);
                arr[idx++] = gameBoard.GetLandY(x, y);
                arr[idx++] = -1; // 未使用（C++互換のため保持）
            }
        }

        return arr;
    }

    //--------------------------------------------------------------
    // NEXTキューデータ書き出し
    //--------------------------------------------------------------
    int[] ExportQueDataCompat()
    {
        int width = 4;

        int[] arr = new int[width];

        for (int x = 0; x < width; x++)
        {
            arr[x] = gameNextQueue.GetVal(x);
        }

        return arr;
    }

    //--------------------------------------------------------------
    // セーブデータ読み込み
    //--------------------------------------------------------------
    void LoadGameCompat()
    {
        string path = Path.Combine(Application.persistentDataPath, "save.txt");

        var table = CRecCompat.Load(path);
        if (table == null) return;

        // バージョンチェック（互換確認）
        if (!table.ContainsKey(GameDef.APP_SAVE_ID_KEY) ||
            table[GameDef.APP_SAVE_ID_KEY][0] != GameDef.APP_SAVE_ID_NUM)
        {
            Debug.Log("Invalid Save File");
            return;
        }

        // 盤面復元
        ImportCellDataCompat(table["CellData"]);

        gameBoard.SetScore(table["Score"][0]);

        // NEXT復元
        ImportQueDataCompat(table["NextData"]);
        gameNextQueue.SetNextBoxCount(table["NextBoxCnt"][0]);
        gameNextQueue.SetBoxValTimer(table["BoxValTime"][0]);

        // 落下状態復元
        fallingX = table["FallX"][0];
        fallingY = table["FallY"][0];
        fallingValue = table["FallVal"][0];

        // 時間復元（フレーム→秒）
        elapsedTime = ((float)table["ElapsedTime"][0]) / 60f;

        // マージ履歴復元
        mergeTimesArray_Time = Array.ConvertAll(table["MergeTime"], x => ((float)x) / 60f);
        mergeTimesArray_Value = table["MergeVal"];
    }

    //--------------------------------------------------------------
    // セルデータ読み込み（配列 → CellDat復元）
    //--------------------------------------------------------------
    void ImportCellDataCompat(int[] arr)
    {
        int width = GameDef.CELL_X_NUM;
        int height = GameDef.CELL_Y_NUM;
        int idx = 0;

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                // 7要素セットで復元
                gameBoard.SetCompatCell(
                    x, y,
                    arr[idx++], // Val
                    arr[idx++], // StackVal
                    arr[idx++], // MergeX
                    arr[idx++], // MergeY
                    arr[idx++], // MergeVal
                    arr[idx++], // LandY
                    arr[idx++]  // LandVal
                );
            }
        }
    }

    //--------------------------------------------------------------
    // NEXTキュー読み込み
    //--------------------------------------------------------------
    void ImportQueDataCompat(int[] arr)
    {
        for (int x = 0; x < arr.Length; x++)
        {
            gameNextQueue.SetVal(x, arr[x]);
        }
    }

    //--------------------------------------------------------------
    // フェーズ制御
    //--------------------------------------------------------------
    // NEXT → 落下開始準備（スライド演出）
    void EnterServePhase()
    {
        phase = GamePhase.Serve;
        phaseFrameCounter = 0;
        phaseFrameLimit = SERVE_FRAMES;

        fallingY = 0;
        fallingPosY = fallingY;
        fallingAnimFromY = fallingY;
        fallingAnimToY = fallingY;

        fallFrameCounter = 0;
        currentFallFrames = FALL_FRAMES_NORMAL;
        isFastFall = false;

        // NEXTシフト＋生成
        fallingValue = gameNextQueue.ExecPush();

        // スライド演出ターゲット
        Vector3 launchTargetWorldPos = new Vector3(
            fallingX,
            DisplayY(0),
            0f
        ) + boardRenderer.boardOffset;

        nextRenderer.StartSlide(fallingValue, launchTargetWorldPos);

        hasFalling = false;
    }

    // ロード時用（Serve省略）
    void EnterFallPhase_WithoutServe()
    {
        fallingPosY = fallingY;
        if(fallingY > 0)
        {
	        fallingAnimFromY = fallingY-1;
	    }
	    else
	    {
	        fallingAnimFromY = 0;
	    }
        fallingAnimToY = fallingY;

        hasFalling = true;

        phase = GamePhase.Fall;
        fallFrameCounter = 0;
        currentFallFrames = FALL_FRAMES_NORMAL;

        UpdateScoreUI();
    }

    //--------------------------------------------------------------
    // フレーム更新
    //--------------------------------------------------------------
    void Update()
    {
        if (appState == AppState.Title)
            UpdateTitle();
        else
            UpdateGame();

        UpdateCommonUI();
    }

    //--------------------------------------------------------------
    // タイトル画面入力処理
    //--------------------------------------------------------------
    void UpdateTitle()
    {
        bool startTrigger = false;
        bool loadTrigger = false;

        // サウンド切替
        if (inputManager.GetKeyDownTrigger(KEY_C))
            soundManager?.SetSoundOff();

        if (inputManager.GetKeyDownTrigger(KEY_V))
            soundManager?.SetSoundOn();

        // アプリ終了
        if (inputManager.GetEscapeDownTrigger())
        {
            QuitApplication();
            return;
        }

        // 開始／ロード操作
        if (inputManager.GetLoadDownTrigger() ||
            inputManager.GetKeyDownTrigger(KEY_X))
        {
            startTrigger = true;
            loadTrigger = true;
        }
        else if (inputManager.GetKeyDownTrigger(KEY_Z))
        {
            startTrigger = true;
        }
        else if (inputManager.GetMouseButtonDownTrigger() &&
                 !IsPositionLessThanColumnZero(inputManager.GetMousePosition()))
        {
            startTrigger = true;
        }
        else if (inputManager.GetTouchDownTrigger() &&
                 !IsPositionLessThanColumnZero(inputManager.GetTouchPosition()))
        {
            startTrigger = true;
        }

        if (startTrigger)
        {
            StartGame();

            if (!loadTrigger)
            {
                EnterServePhase();
            }
            else
            {
                LoadGameCompat();
                EnterFallPhase_WithoutServe();
            }
        }
    }

    //--------------------------------------------------------------
    // ゲーム開始初期化
    //--------------------------------------------------------------
    void StartGame()
    {
        appState = AppState.Game;

        titleUI.SetActive(false);
        titleView.SetActive(false);
        gameUI.SetActive(true);
        gameView.SetActive(true);

        gameBoard = new GameBoard();
        gameNextQueue = new GameNextQueue();

        // Renderer初期化
        if (boardRenderer == null)
        {
            Debug.LogError("BoardRenderer が見つかりません。");
            enabled = false;
            return;
        }

        if (nextRenderer == null)
        {
            Debug.LogError("NextRenderer が見つかりません。");
            enabled = false;
            return;
        }

        if (labelRenderer == null)
        {
            Debug.LogError("LabelRenderer が見つかりません。");
            enabled = false;
            return;
        }

        boardRenderer.Init(gameBoard);
        nextRenderer.Init(gameNextQueue);
        labelRenderer.Init();

        UpdateScoreUI();

        fallingX = 0;

        elapsedTime = 0f;

        for (int i = 0; i < 5; i++)
        {
            mergeTimesArray_Time[i] = 0f;
            mergeTimesArray_Value[i] = -1;
        }

        gameEscapeTrigger = false;
        gameSaveTrigger = false;
    }

    //--------------------------------------------------------------
    // ゲーム更新（メインループ）
    // フェーズ制御・入力・時間・描画を統括
    //--------------------------------------------------------------
    void UpdateGame()
    {
        elapsedTime += Time.deltaTime;

        // マージ履歴のUI更新
        labelRenderer?.SetMergedTimes(mergeTimesArray_Value, mergeTimesArray_Time);

        // ESC → タイトルへ（安全なフェーズのみ）
        if (inputManager.GetEscapeDownTrigger())
        {
            gameEscapeTrigger = true;
        }
        if (gameEscapeTrigger &&
            (phase == GamePhase.Serve || phase == GamePhase.Fall))
        {
            gameEscapeTrigger = false;
            BackToTitle();
            return;
        }

        // セーブ（操作受付＋フェーズ同期）
        if (inputManager.GetSaveDownTrigger())
        {
            gameSaveTrigger = true;
            soundManager?.PlayMerge1();
        }
        if (gameSaveTrigger &&
            (phase == GamePhase.Serve || phase == GamePhase.Fall))
        {
            gameSaveTrigger = false;
            SaveGameCompat();
        }

        // NEXTタイマ進行
        gameNextQueue.ExecCountTimer();

        // 落下中のみ操作入力
        HandleInputDuringFallOnly();

        // フェーズ別処理
        switch (phase)
        {
            case GamePhase.Serve:
                UpdateServePhase();
                break;

            case GamePhase.Fall:
                UpdateFallPhase();
                break;

            case GamePhase.Stack:
                UpdateStackPhase();
                break;

            case GamePhase.Merge:
                UpdateMergePhase();
                break;

            case GamePhase.Land:
                UpdateLandPhase();
                break;
        }

        UpdateElapsedTime();
        UpdateFallingAnimation();
    }

    // NEXTスライド演出
    void UpdateServePhase()
    {
        float progress = 0f;
        if (phaseFrameLimit > 0)
        {
            progress = Mathf.Clamp01((float)phaseFrameCounter / phaseFrameLimit);
        }

        nextRenderer.SetSlideProgress(progress);

        phaseFrameCounter++;
        if (phaseFrameCounter < phaseFrameLimit) return;

        nextRenderer.EndSlide();

        hasFalling = true;

        phase = GamePhase.Fall;
        fallFrameCounter = 0;
        currentFallFrames = FALL_FRAMES_NORMAL;
    }

    // 落下処理（フレーム単位）
    void UpdateFallPhase()
    {
        if (!hasFalling) return;

        currentFallFrames = isFastFall ? FALL_FRAMES_FAST : FALL_FRAMES_NORMAL;

        fallFrameCounter++;

        if (fallFrameCounter >= currentFallFrames)
        {
            fallFrameCounter = 0;

            // 着地判定
            if (gameBoard.CheckEnStack(fallingX, fallingY, fallingValue))
            {
                phase = GamePhase.Stack;
                phaseFrameCounter = 0;
                phaseFrameLimit = STACK_FRAMES;

                fallingPosY = fallingY;
                fallingAnimFromY = fallingY;
                fallingAnimToY = fallingY;

                return;
            }

            // 下が空いている場合のみ落下
	        if (fallingY == 0 && gameBoard.GetVal(fallingX, fallingY + 1) >= 0)
            {
                // 下が空いていない
            }
            else
            {
                int previousY = fallingY;
                fallingY++;
                fallingAnimFromY = previousY;
                fallingAnimToY = fallingY;
            }
        }
    }

    // 着地処理
    void UpdateStackPhase()
    {
        phaseFrameCounter++;
        if (phaseFrameCounter < phaseFrameLimit) return;

        isFastFall = false;
        hasFalling = false;

        gameBoard.ExecStack();
        soundManager?.PlayStack();

        gameBoard.CalcScore();
        UpdateScoreUI();

        // マージ分岐
        if (gameBoard.CheckMerge(fallingX, fallingY, out pendingMaxMergeVal, out pendingMaxMergeDifVal))
        {
            boardRenderer.StartMergePreview();

            // マージの規模に応じて音を切り替え
            if (pendingMaxMergeDifVal <= 1)
            {
                soundManager?.PlayMerge1();
            }
            else if (pendingMaxMergeDifVal == 2)
            {
                soundManager?.PlayMerge2();
            }
            else
            {
                soundManager?.PlayMerge3();
            }

            phase = GamePhase.Merge;
            phaseFrameCounter = 0;
            phaseFrameLimit = MERGE_FRAMES;

            UpdateMergeTime(pendingMaxMergeVal);
        }
        else
        {
            // マージなし → 次の投入へ
            EnterServePhase();
        }
    }

    //--------------------------------------------------------------
    // マージ処理
    // 演出 → 実行 → 次フェーズ判定
    //--------------------------------------------------------------
    void UpdateMergePhase()
    {
        float progress = 0f;
        if (phaseFrameLimit > 0)
        {
            progress = Mathf.Clamp01((float)phaseFrameCounter / phaseFrameLimit);
        }

        // マージプレビュー（拡大／縮小など演出）
        boardRenderer.SetMergePreviewProgress(progress);

        phaseFrameCounter++;
        if (phaseFrameCounter < phaseFrameLimit) return;

        boardRenderer.EndMergePreview();

        // マージ後に生成されるセル情報を取得（エフェクト用）
        List<MergeCreatedInfo> createdInfos = CollectCreatedMergeInfos();

        // 実際のマージ処理
        gameBoard.ExecMerge();

        gameBoard.CalcScore();
        UpdateScoreUI();

        // 新規生成セルのみエフェクト表示
        PlayCreatedMergeEffects(createdInfos);

        // 落下補正の必要有無
        if (gameBoard.CheckEnLand())
        {
            boardRenderer.StartLandPreview();

            phase = GamePhase.Land;
            phaseFrameCounter = 0;
            phaseFrameLimit = LAND_FRAMES;
        }
        else
        {
            // 連鎖マージ判定
            if (gameBoard.CheckMerge(fallingX, fallingY, out pendingMaxMergeVal, out pendingMaxMergeDifVal))
            {
                boardRenderer.StartMergePreview();

                if (pendingMaxMergeDifVal <= 1)
                {
                    soundManager?.PlayMerge1();
                }
                else if (pendingMaxMergeDifVal == 2)
                {
                    soundManager?.PlayMerge2();
                }
                else
                {
                    soundManager?.PlayMerge3();
                }

                phase = GamePhase.Merge;
                phaseFrameCounter = 0;
                phaseFrameLimit = MERGE_FRAMES;

                UpdateMergeTime(pendingMaxMergeVal);
            }
            else
            {
                EnterServePhase();
            }
        }
    }

    //--------------------------------------------------------------
    // 落下補正（重力適用後の再配置）
    //--------------------------------------------------------------
    void UpdateLandPhase()
    {
        float progress = 0f;
        if (phaseFrameLimit > 0)
        {
            progress = Mathf.Clamp01((float)phaseFrameCounter / phaseFrameLimit);
            progress = progress * progress;
        }

        boardRenderer.SetLandPreviewProgress(progress);

        phaseFrameCounter++;
        if (phaseFrameCounter < phaseFrameLimit) return;

        boardRenderer.EndLandPreview();

        // 実際の落下補正
        gameBoard.ExecLand();
        gameBoard.CalcScore();
        UpdateScoreUI();

        // 補正後のマージ判定
        if (gameBoard.CheckMerge(fallingX, fallingY, out pendingMaxMergeVal, out pendingMaxMergeDifVal))
        {
            boardRenderer.StartMergePreview();


            if (pendingMaxMergeDifVal <= 1)
            {
                soundManager?.PlayMerge1();
            }
            else if (pendingMaxMergeDifVal == 2)
            {
                soundManager?.PlayMerge2();
            }
            else
            {
                soundManager?.PlayMerge3();
            }

            phase = GamePhase.Merge;
            phaseFrameCounter = 0;
            phaseFrameLimit = MERGE_FRAMES;

            UpdateMergeTime(pendingMaxMergeVal);
        }
        else
        {
            EnterServePhase();
        }
    }

    // キー入力から列番号を取得
    int GetColumnFromKeyboard()
    {
        for (int i = 0; i < GameDef.KEY_X_NUM; i++)
        {
            if (inputManager.GetKeyDownTrigger(i))
                return i;
        }

        return -1;
    }

    // 経過時間UI更新
    void UpdateElapsedTime()
    {
        labelRenderer.SetElapsedTime(elapsedTime);
    }

    //--------------------------------------------------------------
    // タイトルへ戻る
    // 描画オブジェクトをクリアして状態初期化
    //--------------------------------------------------------------
    void BackToTitle()
    {
        boardRenderer?.ClearAllTiles();
        nextRenderer?.ClearAllTiles();

        gameView.SetActive(false);
        gameUI.SetActive(false);
        titleUI.SetActive(true);
        titleView.SetActive(true);

        appState = AppState.Title;
    }

    //--------------------------------------------------------------
    // 入力（落下中のみ）
    //--------------------------------------------------------------
    void HandleInputDuringFallOnly()
    {
        if (phase != GamePhase.Fall || !hasFalling) return;

        int col = GetColumnFromKeyboard();

        // キー入力で横移動
        if (col >= 0 && col < GameDef.CELL_X_NUM)
        {
            if (gameBoard.GetVal(col, fallingY) < 0)
            {
                fallingX = col;
                isFastFall = true;
            }
        }

        // マウス入力
        if (inputManager.GetMouseButtonDownTrigger())
        {
            ProcessPointer(inputManager.GetMousePosition());
        }

        // タッチ入力
        if (inputManager.GetTouchDownTrigger())
        {
            ProcessPointer(inputManager.GetTouchPosition());
        }
    }

    // ポインタ位置が盤面左外か判定（誤操作防止）
    bool IsPositionLessThanColumnZero(Vector3 screenPosition)
    {
        Camera cam = Camera.main;
        if (cam == null) return false;

        screenPosition.z = -cam.transform.position.z;
        Vector3 world = cam.ScreenToWorldPoint(screenPosition) - boardRenderer.boardOffset;

        int column = Mathf.RoundToInt(world.x);
        return (column < 0);
    }

    // マウス／タッチ位置から列を決定し移動
    void ProcessPointer(Vector3 screenPosition)
    {
        Camera cam = Camera.main;
        if (cam == null) return;

        screenPosition.z = -cam.transform.position.z;
        Vector3 world = cam.ScreenToWorldPoint(screenPosition) - boardRenderer.boardOffset;

        int column = Mathf.RoundToInt(world.x);

        if (column < 0 || column >= GameDef.CELL_X_NUM)
            return;

        // 横移動のみ（高さ固定）
        if (gameBoard.GetVal(column, fallingY) < 0)
        {
            fallingX = column;

            // 移動した瞬間から高速落下（操作感再現）
            isFastFall = true;
        }
    }

    //--------------------------------------------------------------
    // マージ生成セル抽出（エフェクト用）
    //--------------------------------------------------------------
    List<MergeCreatedInfo> CollectCreatedMergeInfos()
    {
        List<MergeCreatedInfo> created = new List<MergeCreatedInfo>();

        for (int x = 0; x < GameDef.CELL_X_NUM; x++)
        {
            for (int y = 0; y < GameDef.CELL_Y_NUM; y++)
            {
                int oldVal = gameBoard.GetVal(x, y);
                int mergeVal = gameBoard.GetMergeVal(x, y);

                // 新規生成セルのみ抽出
                if (mergeVal >= 0 && mergeVal > oldVal)
                {
                    created.Add(new MergeCreatedInfo(x, y, mergeVal));
                }
            }
        }

        return created;
    }

    // マージ生成エフェクト再生
    void PlayCreatedMergeEffects(List<MergeCreatedInfo> createdInfos)
    {
        if (boardRenderer == null) return;
        if (createdInfos == null || createdInfos.Count == 0) return;

        for (int i = 0; i < createdInfos.Count; i++)
        {
            MergeCreatedInfo info = createdInfos[i];
            boardRenderer.PlayMergeEffect(info.x, info.y, info.value);
        }
    }

    //--------------------------------------------------------------
    // アプリ終了（Editor対応）
    //--------------------------------------------------------------
    void QuitApplication()
    {
        Debug.Log("QuitApplication");

    #if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
    #else
        Application.Quit();
    #endif
    }
}
