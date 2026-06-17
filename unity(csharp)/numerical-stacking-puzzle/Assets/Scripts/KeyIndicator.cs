//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// キー入力可視化クラス（入力モニタUI）
// InputManagerの状態を元に、キー表示をON/OFF（色変化）する
// C++版相当クラス: CRAppKeyIndSet,CRAppKeyInd
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
//      +- CRAppKeyIndSet === KeyIndicator
//      |   +- CRAppKeyInd === KeyIndicator
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using UnityEngine;
using UnityEngine.UI;

//----------------------------------------------------------------------
// キー入力可視化クラス（入力モニタUI）
// InputManagerの状態を元に、キー表示をON/OFF（色変化）する
//----------------------------------------------------------------------
public class KeyIndicator : MonoBehaviour
{
    public InputManager inputManager;

    // 表示するキー一覧（配列順 = 表示位置）
    public Image[] keys;

    // 非押下 / 押下時の色
    private Color offColor = new Color(160.0f / 256.0f, 192.0f / 256.0f, 128.0f / 256.0f, 1.0f);
    private Color onColor  = new Color(240.0f / 256.0f, 192.0f / 256.0f, 128.0f / 256.0f, 1.0f);

    //--------------------------------------------------------------
    // 初期状態設定（全キーをOFFに）
    //--------------------------------------------------------------
    public void SetInitialState()
    {
        if (keys == null) return;

        for (int i = 0; i < keys.Length; i++)
        {
            SetKey(i, false);
        }
    }

    //--------------------------------------------------------------
    // 毎フレーム更新
    // InputManagerの状態を元にキー表示を更新
    //--------------------------------------------------------------
    void Update()
    {
        if (keys == null) return;

        for (int i = 0; i < keys.Length; i++)
        {
            bool isOn = false;

            // キー状態を取得
            // GameDef.KEY_X_NUMを超える場合は折り返し対応（UI配置用）
            if (inputManager.GetKeyDown(i < GameDef.KEY_X_NUM ? i : i - GameDef.KEY_X_NUM))
            {
                isOn = true;
            }

            SetKey(i, isOn);
        }
    }

    //--------------------------------------------------------------
    // キー1つ分の表示更新
    // 色を補間して滑らかにON/OFF表現
    //--------------------------------------------------------------
    private void SetKey(int index, bool on)
    {
        if (keys == null) return;
        if (index < 0 || index >= keys.Length) return;
        if (keys[index] == null) return;

        // 現在色から目的色へ少しずつ補間
        keys[index].color = Color.Lerp(
            keys[index].color,
            on ? onColor : offColor,
            0.3f
        );
    }
}
