//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// サウンド再生管理クラス
// 効果音（SE）の再生とON/OFF制御を担当する
// C++版相当クラス: CRMidSnd
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey
//  +- CRMidGui
//  +- CRMidSnd === SoundManager
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
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using UnityEngine;

//----------------------------------------------------------------------
// サウンド再生管理クラス
// 効果音（SE）の再生とON/OFF制御を担当する
//----------------------------------------------------------------------
public class SoundManager : MonoBehaviour
{
    // 再生用AudioSource
    public AudioSource audioSource;

    // サウンド有効/無効フラグ
    public bool isEnabled = true;

    //--------------------------------------------------------------
    // 効果音データ
    //--------------------------------------------------------------
    public AudioClip stackSE;     // 着地音
    public AudioClip mergeSE1;    // 小マージ
    public AudioClip mergeSE2;    // 中マージ
    public AudioClip mergeSE3;    // 大マージ

    //--------------------------------------------------------------
    // 再生処理
    // isEnabled=false の場合は何も再生しない
    //--------------------------------------------------------------
    public void PlayStack()
    {
        if (!isEnabled) return;
        audioSource.PlayOneShot(stackSE);
    }

    public void PlayMerge1()
    {
        if (!isEnabled) return;
        audioSource.PlayOneShot(mergeSE1);
    }

    public void PlayMerge2()
    {
        if (!isEnabled) return;
        audioSource.PlayOneShot(mergeSE2);
    }

    public void PlayMerge3()
    {
        if (!isEnabled) return;
        audioSource.PlayOneShot(mergeSE3);
    }

    //--------------------------------------------------------------
    // サウンド制御
    //--------------------------------------------------------------

    // サウンドOFF
    public void SetSoundOff()
    {
        isEnabled = false;
    }

    // サウンドON（ON時にフィードバック音を再生）
    public void SetSoundOn()
    {
        isEnabled = true;

        // ON操作のフィードバック
        audioSource.PlayOneShot(mergeSE3);
    }
}
