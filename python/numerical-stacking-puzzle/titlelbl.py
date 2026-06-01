"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : titlelbl.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : タイトル用ラベルオブジェクト
"""
#
# Import
#
from __future__ import annotations
from dataclasses import dataclass

from routine import Routine
from define import E_APP_NAME_STR, E_APP_VER_STR, E_APP_CR_STR, numok
from midgui import MidGui, E_MIDGUI_TXT_LBL, E_MIDGUI_TXT_NUM
from midsnd import MidSnd

#
# Definition
#

# ラベル種類
E_TITLE_LBL_TYPE_NUM = 7    # ラベル種類の総数

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態

# GUI配置情報(構造体定義)
@dataclass
class TitleInf:
    x:int                   # 文字位置X
    y:int                   # 文字位置Y
    w:int                   # 文字幅W
    h:int                   # 文字幅H
    text:str                # 文字列
    col:tuple[int,int,int]  # 文字色
    font:int                # 文字サイズ

# GUI配置情報(テーブル)
F_TITLE = [
    TitleInf(100,100,1400,100, E_APP_NAME_STR,                                      (  0,  0,  0),100), # タイトルネーム項目
    TitleInf(100,200,1400, 50, E_APP_VER_STR,                                       (  0,  0,  0), 25), # バージョン情報項目
    TitleInf(100,225,1400, 50, E_APP_CR_STR,                                        (  0,  0,  0), 25), # コピーライト項目
    TitleInf(100,350,1400, 50, "New Game ... Please press the [Z] key.",            (128,128,128), 50), # ニューゲーム項目
    TitleInf(100,450,1400, 50, "Recent Game ... Please press the [X] key.",         (128,128,128), 50), # 最近のゲーム項目
    TitleInf(100,750,1400, 50, "Sound Off ... Please press the [C] key. (now, ON)", (128,128,128), 50), # サウンドオフ項目
    TitleInf(100,750,1400, 50, "Sound On ... Please press the [V] key. (now, OFF)", (128,128,128), 50), # サウンドオン項目
]

#
# Definition of Class
#
class TitleLbl(Routine):
    def __init__(self, tid:int):
        """コンストラクタ

        Args:
            ラベル種類
        """
        super().__init__()
        #------------------------------------------------------------------------
        # 引数チェック
        if not numok(tid, E_TITLE_LBL_TYPE_NUM):
            self.init_ok = False
            return

        #------------------------------------------------------------------------
        # 初期値設定
        self.tid = tid          # ラベル種類設定
        self.gui_txt_id = -1    # 初期GUIのテキストID=-1
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態

        # 初期化正常終了
        self.init_ok_mark()

        #------------------------------------------------------------------------
        # この時点でGUI表示の初期設定をする
        self.update_gui()

    def destroy_gui(self):
        #------------------------------------------------------------------------
        # GUIインスタンスを取得する
        mg = MidGui.get_inst()
        if mg and self.gui_txt_id >= 0:
            # 描画するテキストを登録削除する
            mg.clr_txt(self.gui_txt_id)
            self.gui_txt_id = -1

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
        # 音出力インスタンスを取得する
        snd = MidSnd.get_inst()

        #------------------------------------------------------------------------
        # GUI配置情報を特定する
        inf = F_TITLE[self.tid]

        #------------------------------------------------------------------------
        # 文字表示
        # 文字表示範囲の矩形の設定
        rect = (inf.x, inf.y, inf.x+inf.w, inf.y+inf.h)
        # 文字列の設定(音のON/OFF設定に応じて表示する文字列を切替)
        txt = inf.text
        # Sound ON/OFF 表示切替（C++同様）
        if self.tid == 5 and snd and snd.get_disable_play():
            txt = ""
        if self.tid == 6 and snd and (not snd.get_disable_play()):
            txt = ""
        # GUIのテキストID未取得時は取得する
        if self.gui_txt_id < 0:
            self.gui_txt_id = mg.get_unset_txt_id(E_MIDGUI_TXT_LBL)
        # GUIドライバに文字列の配置を登録
        if 0 <= self.gui_txt_id < E_MIDGUI_TXT_NUM:
            mg.set_txt(self.gui_txt_id, rect, inf.col, txt, inf.font, True, True)
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
        if self.seq == E_SEQ_INIT:
            # GUI表示更新
            if not self.update_gui():
                return False
            self.seq = E_SEQ_IDLE
            return True
        return self.update_gui()
