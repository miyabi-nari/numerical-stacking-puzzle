"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : appkeyind.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : キー表示オブジェクト
"""
#
# Import
#
from __future__ import annotations
from dataclasses import dataclass

from routine import Routine
from define import numok
from midgui import MidGui, E_MIDGUI_RECT_KEY_IND, E_MIDGUI_TXT_KEY_IND
from midkey import MidKey

#
# Definition
#

E_APP_KEY_IND_TYPE_NUM = 11

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態

# GUI配置情報(構造体定義)
@dataclass
class KeyInf:
    frm_x: int                  # フレーム位置X
    frm_y: int                  # フレーム位置Y
    frm_w: int                  # フレーム幅W
    frm_h: int                  # フレーム幅H
    a_x: int                    # 文字A位置X
    a_y: int                    # 文字A位置Y
    a_w: int                    # 文字A幅W
    a_h: int                    # 文字A幅H
    b_x: int                    # 文字B位置X
    b_y: int                    # 文字B位置Y
    b_w: int                    # 文字B幅W
    b_h: int                    # 文字B幅H
    txt_a: str                  # 文字列A
    txt_b: str                  # 文字列B
    line: tuple[int,int,int]    # フレームの線色
    off: tuple[int,int,int]     # フレームの塗潰し色(OFF時)
    on: tuple[int,int,int]      # フレームの塗潰し色(ON時)
    txt: tuple[int,int,int]     # 文字色
    font: int                   # 文字サイズ

# GUI配置情報(テーブル)
F_KEY_INF = [
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｚっ","　つ",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 0番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｘ　","　さ",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 1番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｃ　","　そ",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 2番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｖ　","　ひ",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 3番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｂ　","　こ",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 4番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｎ　","　み",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 5番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "Ｍ　","　も",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 6番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "＜、","，ね",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 7番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "＞。","．る",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 8番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "？・","／め",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置 9番目
    KeyInf(10,10,80,80, 10,10,80,40, 10,50,80,40, "＿　","＼ろ",(0,128,0),(160,192,128),(240,192,128),(0,0,0),36),  # キー位置10番目
]

#
# Definition of Class
#
class AppKeyInd(Routine):
    #----- Constructor(s)/Destructor
    def __init__(self, key_id: int, pos_x: int, pos_y: int):
        """コンストラクタ

        Args:
            key_id=キー表示タイプ
        """
        super().__init__()

        #------------------------------------------------------------------------
        # 引数チェック
        if not numok(key_id, E_APP_KEY_IND_TYPE_NUM):
            self.init_ok = False
            return

        #------------------------------------------------------------------------
        # 初期値設定
        self.key_id = key_id    # キー表示タイプ設定
        self.pos_x = pos_x      # 初期位置X
        self.pos_y = pos_y      # 初期位置Y
        self.gui_rect_id = -1   # 初期GUIの矩形ID=-1
        self.gui_a_id = -1      # 初期GUIのテキストAのID=-1
        self.gui_b_id = -1      # 初期GUIのテキストBのID=-1
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
        if not mg:
            return
        if self.gui_rect_id >= 0:
            # 描画する矩形を登録削除する
            mg.clr_rect(self.gui_rect_id)
            self.gui_rect_id = -1
        if self.gui_a_id >= 0:
            # 描画するテキストを登録削除する
            mg.clr_txt(self.gui_a_id)
            self.gui_a_id = -1
        if self.gui_b_id >= 0:
            # 描画するテキストを登録削除する
            mg.clr_txt(self.gui_b_id)
            self.gui_b_id = -1

    def update_gui(self) -> bool:
        """GUI表示の更新処理
        
        Returns:
            False=更新失敗
        """
        #------------------------------------------------------------------------
        # GUIインスタンスを取得する
        mg = MidGui.get_inst()

        #------------------------------------------------------------------------
        # Key入力インスタンスを取得する
        mk = MidKey.get_inst()
        if not mg or not mk:
            return False

        #------------------------------------------------------------------------
        # GUI配置情報を特定する
        inf = F_KEY_INF[self.key_id]

        #------------------------------------------------------------------------
        # キー入力分岐
        on = mk.get_key_on().qwerty[self.key_id]
        fill = inf.on if on else inf.off

        #------------------------------------------------------------------------
        # 基準位置設定
        bx, by = self.pos_x, self.pos_y

        #------------------------------------------------------------------------
        # フレーム表示
        # GUIの矩形ID未取得時は取得する
        if self.gui_rect_id < 0:
            self.gui_rect_id = mg.get_unset_rect_id(E_MIDGUI_RECT_KEY_IND)
        # フレーム矩形の設定
        r_frm = (bx+inf.frm_x, by+inf.frm_y, bx+inf.frm_x+inf.frm_w, by+inf.frm_y+inf.frm_h)
        # GUIドライバに矩形の配置を指示する
        mg.set_rect(self.gui_rect_id, r_frm, inf.line, fill, True)

        #------------------------------------------------------------------------
        # 文字表示A
        # GUIのテキストID未取得時は取得する
        if self.gui_a_id < 0:
            self.gui_a_id = mg.get_unset_txt_id(E_MIDGUI_TXT_KEY_IND)
        # 文字表示範囲の矩形の設定
        r_a = (bx+inf.a_x, by+inf.a_y, bx+inf.a_x+inf.a_w, by+inf.a_y+inf.a_h)
        # GUIドライバに文字列の配置を指示する
        mg.set_txt(self.gui_a_id, r_a, inf.txt, inf.txt_a, inf.font, False, True)

        #------------------------------------------------------------------------
        # 文字表示B
        # GUIのテキストID未取得時は取得する
        if self.gui_b_id < 0:
            self.gui_b_id = mg.get_unset_txt_id(E_MIDGUI_TXT_KEY_IND)
        # 文字表示範囲の矩形の設定
        r_b = (bx+inf.b_x, by+inf.b_y, bx+inf.b_x+inf.b_w, by+inf.b_y+inf.b_h)
        # GUIドライバに文字列の配置を指示する
        mg.set_txt(self.gui_b_id, r_b, inf.txt, inf.txt_b, inf.font, False, True)

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

