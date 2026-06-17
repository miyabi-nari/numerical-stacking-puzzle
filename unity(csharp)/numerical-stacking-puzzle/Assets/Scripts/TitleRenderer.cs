//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// タイトル画面表示クラス（UI View）
// タイトル・バージョン・操作案内・サウンド状態などを表示する
// C++版相当クラス: CRTitleLblSet,CRTitleLbl,CRMidGui
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui === TitleRenderer
//  +- CRMidSnd
//  +- CRAppli
//      +- CRCellDat
//      +- CRNextDat
//      +- CRec
//      +- CRTitleLblSet === TitleRenderer
//      |   +- CRTitleLbl === TitleRenderer
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
using TMPro;
using UnityEngine;

//----------------------------------------------------------------------
// タイトル画面表示クラス（UI View）
// タイトル・バージョン・操作案内・サウンド状態などを表示する
//----------------------------------------------------------------------
public class TitleRenderer : MonoBehaviour
{
    //--------------------------------------------------------------
    // UI参照
    //--------------------------------------------------------------
    public TextMeshProUGUI titleText;
    public TextMeshProUGUI versionText;
    public TextMeshProUGUI crText;
    public TextMeshProUGUI startText;
    public TextMeshProUGUI recentText;
    public TextMeshProUGUI soundText;

    // サウンド状態参照（表示用）
    public SoundManager soundManager;

    //--------------------------------------------------------------
    // 初期化
    //--------------------------------------------------------------
    void Start()
    {
        // バージョン表記
        titleText.text = GameDef.APP_NAME_STR;
        versionText.text = GameDef.APP_VER_STR;
        crText.text = GameDef.APP_CR_STR;
    }

    //--------------------------------------------------------------
    // 毎フレーム更新
    // サウンドON/OFF状態に応じて案内文を更新
    //--------------------------------------------------------------
    void Update()
    {
        if (soundManager != null && soundText != null)
        {
	        // サウンド状態に応じた操作案内表示
	        if (soundManager.isEnabled)
	        {
	            // 現在ON → OFFにする方法を提示
	            soundText.text = "Sound Off... Please press the [C] key. (now, ON)";
	        }
	        else
	        {
	            // 現在OFF → ONにする方法を提示
	            soundText.text = "Sound On ... Please press the [V] key. (now, OFF)";
	        }
        }
    }
}
