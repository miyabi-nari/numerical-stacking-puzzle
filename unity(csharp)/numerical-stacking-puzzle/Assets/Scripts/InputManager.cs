//----------------------------------------------------------------------
// Copyright 2026 Miyabi
// 入力管理クラス
// キーボード・マウス・タッチ・UIボタンを一元管理し、
// 「押された瞬間（トリガ）」を提供する
// C++版相当クラス: CRMidKey
// CRMainFrm
//  +- CTimerLib
//  +- CRMidKey === InputManager
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
//      +- CRAppLblSet
//      |   +- CRAppLbl
//      +- CRAppBox(FallingBox)
//----------------------------------------------------------------------
using UnityEngine;

//----------------------------------------------------------------------
// 入力管理クラス
// キーボード・マウス・タッチ・UIボタンを一元管理し、
// 「押された瞬間（トリガ）」を提供する
//----------------------------------------------------------------------
public class InputManager : MonoBehaviour
{
    //--------------------------------------------------------------
    // マウス入力
    //--------------------------------------------------------------
	private Vector3 mousePosition = new Vector3(0f, 0f, 0f);
	private bool mouseButtonDown = false;
	private bool mouseButtonDownTrigger = false;

    //--------------------------------------------------------------
    // タッチ入力
    //--------------------------------------------------------------
	private Vector2 touchPosition = new Vector2(0f, 0f);
	private bool touchDown;
	private bool touchDownTrigger;

    //--------------------------------------------------------------
    // キー入力（列操作用）
    //--------------------------------------------------------------
	private bool[] keyDown = new bool[GameDef.KEY_X_NUM];
	private bool[] keyDownTrigger = new bool[GameDef.KEY_X_NUM];

    //--------------------------------------------------------------
    // 機能キー入力（ESC / LOAD / SAVE）
    //--------------------------------------------------------------
	private bool escapeDown = false;
	private bool escapeDownTrigger = false;

	private bool loadDown = false;
	private bool loadDownTrigger = false;

	private bool saveDown = false;
	private bool saveDownTrigger = false;

    //--------------------------------------------------------------
    // UIボタン入力（Unityボタン → 内部トリガ化）
    //--------------------------------------------------------------
	private bool clickBack = false;
	private bool clickLoad = false;
	private bool clickSave = false;

    //--------------------------------------------------------------
    // キー定義（列操作）
    // index = 列番号と一致
    //--------------------------------------------------------------
	static readonly KeyCode[] columnKeys = new KeyCode[]
	{
	    KeyCode.Z,
	    KeyCode.X,
	    KeyCode.C,
	    KeyCode.V,
	    KeyCode.B,
	    KeyCode.N,
	    KeyCode.M,
	    KeyCode.Comma,
	    KeyCode.Period,
	    KeyCode.Slash,
	    KeyCode.Backslash
	};

    //--------------------------------------------------------------
    // 毎フレーム更新
    //--------------------------------------------------------------
    void Update()
    {
		UpdateMouse();
		UpdateTouch();
		UpdateKey();
		UpdateFunction();
    }

    //--------------------------------------------------------------
    // マウス入力更新
    // 位置とクリックトリガを取得
    //--------------------------------------------------------------
	void UpdateMouse()
	{
		bool down;

		mousePosition = Input.mousePosition;

		// 押した瞬間トリガ生成
		down = Input.GetMouseButtonDown(0);
		mouseButtonDownTrigger = (!mouseButtonDown) && down;
		mouseButtonDown = down;
	}

    //--------------------------------------------------------------
    // タッチ入力更新
    // 最初の指のみ利用（シンプル化）
    //--------------------------------------------------------------
	void UpdateTouch()
	{
		bool down;

        if (Input.touchCount > 0)
        {
            Touch touch = Input.GetTouch(0);

            if (touch.phase == TouchPhase.Began)
            {
            	touchPosition = touch.position;
                down = true;
            }
            else
            {
                down = false;
            }
        }
        else
        {
            down = false;
        }

		touchDownTrigger = (!touchDown) && down;
		touchDown = down;
	}

    //--------------------------------------------------------------
    // キー入力更新
    // 列操作キー（配列対応）
    //--------------------------------------------------------------
	void UpdateKey()
	{
		bool down;
		
	    for (int i = 0; i < columnKeys.Length; i++)
	    {
	        down = Input.GetKey(columnKeys[i]);

	        // IME影響対策（\キー）
	        if (i == 10)
	        {
	            down |= Input.inputString.Contains("\\");
	        }

			keyDownTrigger[i] = (!keyDown[i]) && down;
			keyDown[i] = down;
	    }
	}

    //--------------------------------------------------------------
    // 機能キー（ESC / SAVE / LOAD）更新
    // UIボタン入力も統合
    //--------------------------------------------------------------
	void UpdateFunction()
	{
		bool down;

        // ESC / 戻る
        down = Input.GetKeyDown(KeyCode.Escape);
        down |= clickBack;

		escapeDownTrigger = (!escapeDown) && down;
		escapeDown = down;
		clickBack = false;

        // LOAD（UIのみ）
        down = clickLoad;

		loadDownTrigger = (!loadDown) && down;
		loadDown = down;
		clickLoad = false;

        // SAVE（Sキー or UI）
        down = Input.GetKeyDown(KeyCode.S);
        down |= clickSave;

		saveDownTrigger = (!saveDown) && down;
		saveDown = down;
		clickSave = false;
	}

    //--------------------------------------------------------------
    // Getter（入力参照）
    //--------------------------------------------------------------
    public Vector3 GetMousePosition()
	{
		return mousePosition;
	}

    public bool GetMouseButtonDownTrigger()
	{
		return mouseButtonDownTrigger;
	}

    public Vector2 GetTouchPosition()
	{
		return touchPosition;
	}

	public bool GetTouchDownTrigger()
	{
		return touchDownTrigger;
	}

    public bool GetKeyDown(int keyId)
	{
		if (keyId >= 0 && keyId < GameDef.KEY_X_NUM)
		{
			return keyDown[keyId];
		}
		else
		{
			return false;
		}
	}

    public bool GetKeyDownTrigger(int keyId)
	{
		if(keyId >= 0 && keyId < GameDef.KEY_X_NUM)
		{
			return keyDownTrigger[keyId];
		}
		else
		{
			return false;
		}
	}

    public bool GetEscapeDownTrigger()
	{
		return escapeDownTrigger;
	}

    public bool GetLoadDownTrigger()
	{
		return loadDownTrigger;
	}

    public bool GetSaveDownTrigger()
	{
		return saveDownTrigger;
	}

    //--------------------------------------------------------------
    // UIボタン入力（Unity Buttonから呼ばれる）
    // ※1フレームだけ有効なトリガとして扱う
    //--------------------------------------------------------------

    // タイトルへ戻る
    public void OnClickBack()
    {
    	clickBack = true;
    }
    
    // ロード
    public void OnClickLoad()
    {
    	clickLoad = true;
    }
    
    // セーブ
    public void OnClickSave()
    {
    	clickSave = true;
    }
}
