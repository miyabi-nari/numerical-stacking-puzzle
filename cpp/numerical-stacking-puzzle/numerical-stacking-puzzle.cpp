/********************************************************************************
* Copyright 2026 Miyabi                                                         *
*                                                                               *
* Project Name  : Numerical Stacking Puzzle (Windows 11, Visual Studio 2022)    *
* File Name     : numerical-stacking-puzzle.cpp                                 *
* Author        : Miyabi                                                        *
* Create        : 2026/06/01                                                    *
* Purpose       : アプリケーションのエントリ ポイント定義                       *
********************************************************************************/

/********************************************************************************
*   Include Files                                                               *
********************************************************************************/
#include "framework.h"
#include "numerical-stacking-puzzle.h"

#include "define.h"
#include "mainfrm.h"
#include "fileio.h"
#include <locale.h>
#include "wmstr.h"

/********************************************************************************
*   Defines                                                                     *
********************************************************************************/
// 文字列の最大長
#define MAX_LOADSTRING 100

// 初期ウィンドウサイズX,Y
#define E_WND_WIDTH    1200
#define E_WND_HEIGHT    675

/********************************************************************************
*   Local Variables                                                             *
********************************************************************************/
// グローバル変数:
HINSTANCE hInst;                                // アプリケーションインスタンスハンドル
//※↑使いません(代わりにm_hAppInstを使う)
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

HWND       m_hAppWnd;                           // ウィンドウハンドル
HINSTANCE  m_hAppInst;                          // 現在のインターフェイス(インスタンスハンドル)
HACCEL     m_hAppAccelTable;                    // アクセラレータテーブルハンドル

CRMainFrm* m_lpMainFrm;                         // アプリケーション本体インスタンス
bool       m_bAppQuit;                          // 終了要求FG
bool       m_bAppActive;                        // ウィンドウアクティブ状態FG
RECT       m_rcLastWnd;                         // ウィンドウモードのサイズ

/********************************************************************************
*   Local Functions                                                             *
********************************************************************************/
// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void                AppIdle(MSG &msg);
void                QueryCmdLine(LPWSTR dstCmd1, LPWSTR dstCmd2);
void                ReleaseInstance(void);

/********************************************************************************
*   GlobalFunctions / APIEntry                                                  *
********************************************************************************/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_NUMERICALSTACKINGPUZZLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NUMERICALSTACKINGPUZZLE));
    m_hAppAccelTable = hAccelTable;

    MSG msg;

    // メイン メッセージ ループ:
    while (!m_bAppQuit)
    {
        AppIdle(msg);
        Debuglog(nullptr);
    }

    //-----------------------------------------------------------------------
    // アプリケーション終了処理
    ReleaseInstance();

    return (int) msg.wParam;
}

/********************************************************************************
* Function Name : MyRegisterClass                                               *
* Purpose       : ウィンドウ クラスを登録します                                 *
* Argument      : hInstance=インスタンスハンドル                                *
* Return        : RegisterClassExW戻り値                                        *
* Caution(s)    :                                                               *
********************************************************************************/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NUMERICALSTACKINGPUZZLE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NUMERICALSTACKINGPUZZLE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

/********************************************************************************
* Function Name : InitInstance                                                  *
* Purpose       : アプリケーションのウィンドウ生成および初期                    *
* Argument      : hInstance=インスタンスハンドル                                *
*               : nCmdShow=ウィンドウ表示設定                                   *
* Return        : TRUE=正常終了                                                 *
* Caution(s)    : この関数で、グローバル変数でインスタンス ハンドルを保存し、   *
*                 メイン プログラム ウィンドウを作成および表示します。          *
********************************************************************************/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する
   //※↑使いません(代わりにm_hAppInstを使う)

   HWND hWnd = CreateWindowW(szWindowClass, szTitle,
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

    m_hAppWnd = hWnd;       // グローバル変数にウィンドウ ハンドルを保存します
    m_hAppInst = hInstance; // グローバル変数にインスタンス ハンドルを保存します

    // ロケール設定(JPN)
    setlocale( LC_CTYPE, "jpn");

    // ファイルセットアップ（パス指定）
    WCHAR filepath[3][MAX_PATH];
    filepath[0][0] = filepath[1][0] = filepath[2][0] = WCHAR_NULL;  //全部NULL
    QueryCmdLine(filepath[0], filepath[1]);     //コマンドライン解析
    GetCurrentDirectory(MAX_PATH, filepath[2]); //カレントディレクトリ取得
    if(*filepath[1] != WCHAR_NULL)
    {
        CFileIO::AllSetUp(filepath[1], filepath[2], filepath[0]);
    }
    else
    {
        CFileIO::AllSetUp(filepath[2], filepath[0], nullptr);
    }
    
    // アプリケーション本体の起動
    NEW(m_lpMainFrm, CRMainFrm(hInstance, hWnd));
    if(!m_lpMainFrm)
    {
        return FALSE;
    }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

    // ウィンドウの初期配置を決定する
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hWnd, &wp);
    wp.rcNormalPosition.right = wp.rcNormalPosition.left + GetEnv((LPWSTR)(L"WINDOWWIDTH"), E_WND_WIDTH)+5;
    wp.rcNormalPosition.bottom = wp.rcNormalPosition.top + GetEnv((LPWSTR)(L"WINDOWHEIGHT"), E_WND_HEIGHT)+45;
    SetWindowPlacement(hWnd, &wp);

    // アプリケーション動作FGセット
    m_bAppActive = TRUE;
    m_bAppQuit = FALSE;
    ::GetWindowRect(m_hAppWnd, &m_rcLastWnd);

   return TRUE;
}

/********************************************************************************
* Function Name : WndProc                                                       *
* Purpose       : メイン ウィンドウのメッセージを処理します                     *
* Argument      : hWnd=ウィンドウハンドル                                       *
*                 message=メッセージ                                            *
*                 wParam, lParam=メッセージの補助パラメータ                     *
* Return        : 基本、0を返す                                                 *
* Caution(s)    : wWinMain関数から繰返しコールされるAppIdle関数は、             *
*                 拾い上げたメッセージをDispatchMessageで投げています。         *
*                 それをウィンドウがキャッチして、この関数がコールされます。    *
*                 WM_COMMAND  - アプリケーション メニューの処理                 *
*                 WM_PAINT    - メイン ウィンドウを描画する                     *
*                 WM_DESTROY  - アプリケーション終了処理                        *
********************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(m_hAppInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                m_lpMainFrm->PassWndMsg(message, wParam, lParam);
                break;
            case IDM_TITLE:
                m_lpMainFrm->PassWndMsg(message, wParam, lParam);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // HDC を使用する描画コードはMidGuiで実装
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CLOSE:
        m_lpMainFrm->PassWndMsg(message, wParam, lParam);
        break;
    case WM_DESTROY:
        // WM_DESTROYをフック処理してDefWindowProcを呼んでいないのでDESTROY処理はあまり意味がない
        m_bAppQuit = TRUE;
        PostQuitMessage(0);
        break;
    case WM_LBUTTONDOWN:
        ::SetCapture(hWnd);
        if(m_lpMainFrm != nullptr)
        {
            m_lpMainFrm->PassWndMsg(message, wParam, lParam);
        }
        break;
    case WM_LBUTTONUP:
        ::ReleaseCapture();
        if(m_lpMainFrm != nullptr)
        {
            m_lpMainFrm->PassWndMsg(message, wParam, lParam);
        }
        break;
    case WM_WINDOWPOSCHANGED:
        // ウィンドウ位置・サイズ変更
        m_rcLastWnd.left = ((WINDOWPOS*)(lParam))->x;
        m_rcLastWnd.top = ((WINDOWPOS*)(lParam))->y;
        m_rcLastWnd.right = m_rcLastWnd.left + ((WINDOWPOS*)(lParam))->cx;
        m_rcLastWnd.bottom = m_rcLastWnd.top + ((WINDOWPOS*)(lParam))->cy;
        // Defを呼んでWM_MOVING / WM_SIZINGへのパスも作る
        return DefWindowProc(hWnd, message, wParam, lParam);
//      break;
    case WM_KEYDOWN:
        if(m_lpMainFrm != nullptr)
        {
            m_lpMainFrm->PassWndMsg(message, wParam, lParam);
        }
        break;
    case WM_KEYUP:
        if(m_lpMainFrm != nullptr)
        {
            m_lpMainFrm->PassWndMsg(message, wParam, lParam);
        }
        break;
    case WM_ACTIVATE:
        // ウィンドウアクティブ化
        if(wParam == WA_INACTIVE)
        {
            // フォーカス消失
            m_bAppActive = FALSE;
        }
        else if(wParam == 0x00200000)
        {
            // フルスクリーン -> フォーカス消失（？）
            m_bAppActive = FALSE;
        }
        else
        {
            // その他
            m_bAppActive = TRUE;
            // ウィンドウモードに変更
            ::DrawMenuBar(m_hAppWnd);
            ::SetWindowPos(m_hAppWnd,
                           HWND_NOTOPMOST,
                           m_rcLastWnd.left,
                           m_rcLastWnd.top,
                           RectWidth(m_rcLastWnd),
                           RectHeight(m_rcLastWnd),
                           SWP_SHOWWINDOW);
        }
        m_lpMainFrm->PassWndMsg(message, wParam, lParam);
        break;
    case WM_IME_NOTIFY:
        // IMEツールバーを非表示にする
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/********************************************************************************
* Function Name : About                                                         *
* Purpose       : バージョン情報ボックスのメッセージ ハンドラーです             *
* Argument      : hDlg=ダイアログハンドル                                       *
*                 message=メッセージ                                            *
*                 wParam, lParam=メッセージの補助パラメータ                     *
* Return        : TRUE/FALSE(特に気にしなくて良い)                              *
* Caution(s)    : wWinMain関数から繰返しコールされるAppIdle関数は、             *
*                 拾い上げたメッセージをDispatchMessageで投げています。         *
*                 それをウィンドウがキャッチして、この関数がコールされます。    *
*                 WM_COMMAND  - アプリケーション メニューの処理                 *
*                 WM_PAINT    - メイン ウィンドウを描画する                     *
*                 WM_DESTROY  - 中止メッセージを表示して戻る                    *
********************************************************************************/
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    default:
        break;
    }
    return (INT_PTR)FALSE;
}

/********************************************************************************
* Function Name : AppIdle                                                       *
* Purpose       : ウィンドウメッセージ・タイマ処理                              *
* Argument      : msg=Windowsメッセージ                                         *
* Return        : none                                                          *
* Caution(s)    : 非アクティブ状態中は下位層の処理を実行しない                  *
********************************************************************************/
void
AppIdle(MSG &msg)
{
    // メッセージ処理
    while(PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE) == TRUE)
    {
        if (GetMessage(&msg, NULL, 0,0) == false)
        {
            return;
        }
        if (!TranslateAccelerator(msg.hwnd, m_hAppAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // タイマーウェイト
    if (!m_bAppActive)
    {
        // アクティブ状態以外 -> スリープかます．
        Sleep(50);
    }
    else
    {
        // メイン処理
        if(m_lpMainFrm == nullptr)
        {
            m_bAppQuit = TRUE;
        }
        else
        {
            // 定期更新処理
            if(m_lpMainFrm->Update() == FALSE)
            {
                m_bAppQuit = TRUE;
            }
        }
        Sleep(0);
    }
    return;
}

/********************************************************************************
* Function Name : QueryCmdLine                                                  *
* Purpose       : コマンドライン解析                                            *
* Argument      : lpApp=アプリケーションパス文字列格納先                        *
*                 lpArg=コマンドライン引数文字列格納先                          *
* Return        : none                                                          *
* Caution(s)    : 引数をパスと想定して文字列を取る                              *
*                 .exeのファイル本体や末尾\文字を省く処理も織り込み済           *
********************************************************************************/
void
QueryCmdLine(LPWSTR lpApp, LPWSTR lpArg)
{
    long i;
    bool bDblQt;
    LPWSTR lpSrc = ::GetCommandLine();
    *lpApp = WCHAR_NULL;
    *lpArg = WCHAR_NULL;

    if(wcslen(lpSrc) >= MAX_PATH)
    {
        return;
    }

    // 空白が見つかるまで前進
    while (*lpSrc && *lpSrc == L' ')
    {
        lpSrc++;
    }
    //文字列取り出し ダブルクォーテーションで括った中の空白は区切り記号とみなさない．
    i = 0;
    bDblQt = false;
    while (*lpSrc && (*lpSrc != L' ' || bDblQt))
    {
        //ダブルクォーテーション？
        if (*lpSrc == L'"')
        {
            bDblQt = !bDblQt;
        }
        else
        {
            lpApp[i++] = *lpSrc;
        }
        lpSrc++;
    }
    lpApp[i] = WCHAR_NULL;//終端にNULL

    //ファイルパスの最後の'\'記号以降を切る
    while (i >= 0 && lpApp[i] != L'\\')
    {
        i--;
    }
    lpApp[i] = WCHAR_NULL;

    // 空白が見つかるまで前進
    while (*lpSrc && *lpSrc == L' ')
    {
        lpSrc++;
    }
    //文字列取り出し ダブルクォーテーションで括った中の空白は区切り記号とみなさない．
    i = 0;
    bDblQt = false;
    while (*lpSrc && (*lpSrc != L' ' || bDblQt))
    {
        //ダブルクォーテーション？
        if (*lpSrc == L'"')
        {
            bDblQt = !bDblQt;
        }
        else
        {
            lpArg[i++] = *lpSrc;
        }
        lpSrc++;
    }
    lpArg[i] = WCHAR_NULL;//終端にNULL

    //ファイルパスの最後の'\'記号以降を切る
    while (i >= 0 && lpArg[i] != L'\\')
    {
        i--;
    }
    lpArg[i] = WCHAR_NULL;
}

/********************************************************************************
* Function Name : ReleaseInstance                                               *
* Purpose       : アプリケーション終了処理                                      *
* Argument      : none                                                          *
* Return        : none                                                          *
* Caution(s)    :                                                               *
********************************************************************************/
void ReleaseInstance(void)
{
    //-----------------------------------------------------------------------
    // 終了処理
    FREE(m_lpMainFrm);

    // ウィンドウサイズ保存
    SaveEnv((LPWSTR)(L"WINDOWWIDTH"),  m_rcLastWnd.right  - m_rcLastWnd.left -  5);
    SaveEnv((LPWSTR)(L"WINDOWHEIGHT"), m_rcLastWnd.bottom - m_rcLastWnd.top  - 45);

    CFileIO::AllCleanup();
}
