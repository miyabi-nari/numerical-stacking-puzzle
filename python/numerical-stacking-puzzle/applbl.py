"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : applbl.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : ラベルオブジェクト
"""
#
# Import
#
from __future__ import annotations
from dataclasses import dataclass

from routine import Routine
from define import numok
from midgui import MidGui, E_MIDGUI_TXT_NUM, E_MIDGUI_TXT_LBL

#
# Definition
#

# ラベル種類
E_APP_LBL_TYPE_NEXT_LBL     = 0     # 次落下箱案内
E_APP_LBL_TYPE_SCORE_LBL    = 1     # スコア項目
E_APP_LBL_TYPE_ELAPSED_LBL  = 2     # 経過時間項目
E_APP_LBL_TYPE_MERGED_LBL   = 3     # マージ時間項目
E_APP_LBL_TYPE_SCORE_VAL    = 4     # スコア値
E_APP_LBL_TYPE_ELAPSED_VAL  = 5     # 経過時間値
E_APP_LBL_TYPE_MERGED_VAL_1 = 6     # マージ時間値1
E_APP_LBL_TYPE_MERGED_VAL_2 = 7     # マージ時間値2
E_APP_LBL_TYPE_MERGED_VAL_3 = 8     # マージ時間値3
E_APP_LBL_TYPE_MERGED_VAL_4 = 9     # マージ時間値4
E_APP_LBL_TYPE_MERGED_VAL_5 = 10    # マージ時間値5
E_APP_LBL_TYPE_NUM          = 11    # ラベル種類の総数

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態

# GUI配置情報(構造体定義)
@dataclass
class LblGuiInf:
    x: int                  # 文字位置X
    y: int                  # 文字位置Y
    w: int                  # 文字幅W
    h: int                  # 文字幅H
    base: str               # 文字列
    col: tuple[int,int,int] # 文字色
    font: int               # 文字サイズ

# GUI配置情報(テーブル)
F_APP_LBL_GUI_INF = [
    LblGuiInf(1200,  40, 400, 60, "Next Block",    (128,128,128), 50),  # 次落下箱案内 ラベル
    LblGuiInf(1200, 220, 400, 60, "Score",         (128,128,128), 50),  # スコア項目 ラベル
    LblGuiInf(1200, 400, 400, 60, "Elapsed-Time",  (128,128,128), 50),  # 経過時間項目 ラベル
    LblGuiInf(1200, 580, 400, 60, "Marged-Time",   (128,128,128), 50),  # マージ時間項目 ラベル
    LblGuiInf(1220, 280, 380, 60, "0",             (128,128,128), 50),  # スコア値 ラベル
    LblGuiInf(1220, 460, 380, 60, "--:--:--.--",   (128,128,128), 50),  # 経過時間値 ラベル
    LblGuiInf(1220, 650, 380, 40, "-",             (128,128,128), 30),  # マージ時間値1 ラベル
    LblGuiInf(1220, 690, 380, 40, "-",             (128,128,128), 30),  # マージ時間値2 ラベル
    LblGuiInf(1220, 730, 380, 40, "-",             (128,128,128), 30),  # マージ時間値3 ラベル
    LblGuiInf(1220, 770, 380, 40, "-",             (128,128,128), 30),  # マージ時間値4 ラベル
    LblGuiInf(1220, 810, 380, 40, "-",             (128,128,128), 30),  # マージ時間値5 ラベル
]

F_MERGED_PREFIX = [
    "1...",         # 2の 0乗
    "2...",         # 2の 1乗
    "4...",         # 2の 2乗
    "8...",         # 2の 3乗
    "16...",        # 2の 4乗
    "32...",        # 2の 5乗
    "64...",        # 2の 6乗
    "128...",       # 2の 7乗
    "256...",       # 2の 8乗
    "512...",       # 2の 9乗
    "1,024...",     # 2の10乗
    "2,048...",     # 2の11乗
    "4,096...",     # 2の12乗
    "8,192...",     # 2の13乗
    "16,384...",    # 2の14乗
    "32,768...",    # 2の15乗
    "65,536...",    # 2の16乗
    "131,072...",   # 2の17乗
    "262,144...",   # 2の18乗
    "524,288...",   # 2の19乗
    "1,048,576..."  # 2の20乗
]

def _format_ticks(t: int) -> str:
    """経過時間を文字列に変換

    Args:
        経過時間

    Returns:
        変換した文字列
    """
    if t < 0:
        return "--:--:--.--"
    n = t
    h = n // (60*60*60); n %= (60*60*60)
    m = n // (60*60);    n %= (60*60)
    s = n // 60;         n %= 60
    cc = (n * 100) // 60
    return f"{h}:{m:02d}:{s:02d}.{cc:02d}"

def _approach(old: int, target: int) -> int:
    """スコアに徐々に寄せた数値を算出する

    Args:
        元の値(現在表示中のスコア)
        寄せる目標の値(本来のスコア)

    Returns:
        スコアに徐々に寄せた数値
    """
    if target < 0:
        return -1
    if old < 0:
        old = 0
    if old < target:
        old += ((target - old) + 99) // 100
    elif old > target:
        old -= ((old - target) + 99) // 100
    return old

#
# Definition of Class
#
class AppLbl(Routine):
    #----- Constructor(s)/Destructor
    def __init__(self, lbl_type_id: int):
        """コンストラクタ

        Args:
            lbl_type_id=ラベルタイプ
        """
        super().__init__()

        #------------------------------------------------------------------------
        # 引数チェック
        if not numok(lbl_type_id, E_APP_LBL_TYPE_NUM):
            self.init_ok = False
            return

        #------------------------------------------------------------------------
        # 初期値設定
        self.tid = lbl_type_id  # ラベルタイプ設定
        self.val1 = -1          # 表示値=-1
        self.val2 = -1          # 表示値=-1
        self.val_old = -1
        self.gui_txt_id = -1    # 初期GUIのテキストID=-1
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態

        # 初期化正常終了
        self.init_ok_mark()

        #------------------------------------------------------------------------
        # この時点でGUI表示の初期設定をする
        self.update_gui()

    def destroy_gui(self):
        """C++のデストラクタに代わる解放処理

        Notes:
            pythonのデストラクタはGCが非同期で呼び出されるため、
            インスタンス解放時の処理はこれを明示的に呼び出して代用する。
        """
        #------------------------------------------------------------------------
        # GUIインスタンスを取得する
        mg = MidGui.get_inst()
        if mg and self.gui_txt_id >= 0:
            # 描画するテキストを登録削除する
            mg.clr_txt(self.gui_txt_id)
            self.gui_txt_id = -1

    def set_val(self, v1: int, v2: int = 0):
        """ラベル表示値を設定

        Args:
            表示値
        """
        self.val1 = v1
        self.val2 = v2

    def update_gui(self) -> bool:
        """GUI表示の更新処理
        
        Returns:
            False=更新失敗
        """
        #------------------------------------------------------------------------
        # GUIインスタンスを取得する
        mg = MidGui.get_inst()
        if mg is None:
            return False

        #------------------------------------------------------------------------
        # GUI配置情報を特定する
        inf = F_APP_LBL_GUI_INF[self.tid]
        rect = (inf.x, inf.y, inf.x+inf.w, inf.y+inf.h)

        text = inf.base

        # 文字列に表示値を追記する
        if self.tid == E_APP_LBL_TYPE_SCORE_VAL:
            # ラベル種類: スコア値
            if self.val1 >= 0:
                # スコアに徐々に寄せた数値に更新する
                self.val_old = _approach(self.val_old, self.val1)
                text = f"{self.val_old:,}" if self.val_old >= 0 else "-"
            else:
                text = "-"
        elif self.tid == E_APP_LBL_TYPE_ELAPSED_VAL:
            # ラベル種類: 経過時間値
            # 経過時間を文字列に変換
            text = _format_ticks(self.val1)
        elif E_APP_LBL_TYPE_MERGED_VAL_1 <= self.tid <= E_APP_LBL_TYPE_MERGED_VAL_5:
            # ラベル種類: マージ時間値
            if self.val2 > 0 and self.val2 < len(F_MERGED_PREFIX):
                # 経過時間を文字列に変換
                text = F_MERGED_PREFIX[self.val2] + _format_ticks(self.val1)
            else:
                text = "-"
        # GUIのテキストID未取得時は取得する
        if self.gui_txt_id < 0:
            self.gui_txt_id = mg.get_unset_txt_id(E_MIDGUI_TXT_LBL)
        # GUIドライバに文字列の配置を指示する
        if 0 <= self.gui_txt_id < E_MIDGUI_TXT_NUM:
            mg.set_txt(self.gui_txt_id, rect, inf.col, text, inf.font, True, True)

        return True

    def update(self) -> bool:
        """定期更新処理
        
        Returns:
            False=処理終了（オブジェクト破棄）
        
        Notes:
            呼び出し周期＝1/60sec
        """
        #------------------------------------------------------------------------
        # 基底クラスの処理
        if not super().update():
            return False

        #------------------------------------------------------------------------
        # シーケンス処理
        # シーケンス処理:初期状態
        if self.seq == E_SEQ_INIT:
            #--------------------------------------------------------------------
            # GUI表示の更新処理
            if not self.update_gui():
                return False
            #--------------------------------------------------------------------
            # シーケンス -> 待機状態
            self.seq = E_SEQ_IDLE
            return True
        # シーケンス処理:待機状態
        # GUI表示更新
        return self.update_gui()

