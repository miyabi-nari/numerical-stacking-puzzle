"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : mainfrm.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 全体構造制御
"""

#
# Import
#
from __future__ import annotations

from routine import Routine
from timerlib import TimerLib, TIMER_ISOVER, TIMER_ISJUST, TIMER_ISSOON, TIMER_ISIDLE
from define import dec_val, E_WID_X, E_WID_Y

from midgui import MidGui
from midkey import MidKey
from midsnd import MidSnd
from appli import Appli

import time

#
# Definition
#

# シーケンス
E_SEQ_INIT  = 0 # 初期状態
E_SEQ_MID   = 1 # ミドルウェア起動中
E_SEQ_APPLI = 2 # アプリケーション動作中

# 子プロセス
E_RT_MID_KEY = 0    # Key入力のミドルウェアルーチン
E_RT_MID_GUI = 1    # GUI描画のミドルウェアルーチン
E_RT_MID_SND = 2    # 音再生のミドルウェアルーチン
E_RT_APPLI   = 3    # アプリケーションルーチン
E_RT_NUM     = 10   # 子プロセスの上限数

# タイマー
E_TM_1STWAIT = 60   # 初期安定化時間

#
# Definition of Class
#
class MainFrm(Routine):
    #----- Constructor(s)/Destructor
    def __init__(self, window):
        """コンストラクタ

        Args:
            window=ウィンドウハンドル
        """
        super().__init__()
        self.window = window    # ウィンドウハンドル

        #------------------------------------------------------------------------
        # 初期化処理
        self.set_child_num(E_RT_NUM)

        #------------------------------------------------------------------------
        # タイマーオブジェクト起動 分解能＝1ms, 60fps
        self.timer = TimerLib()
        self.timer.create(1)
        self.timer.start(60)

        #------------------------------------------------------------------------
        # 初期値設定
        self.tm1st = E_TM_1STWAIT   # 初期待機時間設定
        self.seq = E_SEQ_INIT       # 初期シーケンス=初期状態

        self.midkey = None
        self.midgui = None
        self.midsnd = None
        self.appli  = None

        #------------------------------------------------------------------------
        # 初期化正常終了
        self.init_ok_mark()

    def update(self) -> bool:
        """定期更新処理

        Returns:
            False=処理終了（オブジェクト破棄）
        """
        disable_draw = False

        #------------------------------------------------------------------------
        # タイマー状態に応じて処理
        st = self.timer.get_time_stat()
        if st == TIMER_ISOVER:      # タイマー時間経過超過 -> 描画無し処理
            disable_draw = True
        elif st == TIMER_ISSOON:    # タイマー経過直前 -> ここで待つ
            while st == TIMER_ISSOON:
                #time.sleep(0.0)
                st = self.timer.get_time_stat()
        elif st == TIMER_ISIDLE:    # 余裕のある状態 -> 処理無し
            time.sleep(0.001)
            return True             # 以降の処理無し

        # ↓↓↓以降の部分は1/60sec周期で処理されます
        #------------------------------------------------------------------------
        # 初期待機時間中はすべて描画処理有りとする
        if self.tm1st != 0:
            # 初期待機時間中
            disable_draw = False
            # タイマーチェック＆タイマーリセット
            while st == TIMER_ISOVER:
                #time.sleep(0.0)
                st = self.timer.get_time_stat()

        #------------------------------------------------------------------------
        # 描画有無の更新
        # GUIインスタンスを取得する
        if self.midgui:
            self.midgui.disable_draw(disable_draw)

        #------------------------------------------------------------------------
        # 基底クラスの処理
        if not super().update():
            return False

        #------------------------------------------------------------------------
        # タイマー
        self.tm1st = dec_val(self.tm1st, 1)

        #------------------------------------------------------------------------
        # シーケンス処理
        # シーケンス処理:初期状態
        if self.seq == E_SEQ_INIT:
            #--------------------------------------------------------------------
            # Key入力のミドルウェアルーチン起動
            self.midkey = MidKey()
            # GUI描画のミドルウェアルーチン起動
            self.midgui = MidGui(self.window.width(), self.window.height())
            # 音再生のミドルウェアルーチン起動
            self.midsnd = MidSnd()
            self.children[E_RT_MID_KEY] = self.midkey
            self.children[E_RT_MID_GUI] = self.midgui
            self.children[E_RT_MID_SND] = self.midsnd

            # シーケンス -> ミドルウェア起動中
            self.seq = E_SEQ_MID
            return True

        #------------------------------------------------------------------------
        # シーケンス処理:ミドルウェア起動中
        if self.seq == E_SEQ_MID:
            #--------------------------------------------------------------------
            # GUIインスタンスを取得する
            # 設定...60fps
            if self.midgui:
                self.midgui.set_fps(1)
            #--------------------------------------------------------------------
            # アプリケーションルーチン起動
            self.appli = Appli()
            self.children[E_RT_APPLI] = self.appli
            # シーケンス -> アプリケーション動作中
            self.seq = E_SEQ_APPLI
            return True

        #------------------------------------------------------------------------
        # シーケンス処理:アプリケーション動作中
        if self.seq == E_SEQ_APPLI:
            if self.appli and self.appli.is_exit_seq():
                return False
            return True
        return True

    def draw(self, painter):
        if self.midgui:
            self.midgui.blit(painter)

    def on_resize(self, w, h):
        if self.midgui:
            self.midgui.on_resize(w, h)

    # ---- input wiring ----
    def set_mouse_l(self, down: bool):
        if self.midkey:
            self.midkey.set_mouse_l_in(down)

    def set_mouse_pos(self, x, y, w, h):
        if self.midkey:
            w = max(1, int(w)); h = max(1, int(h))
            xn = int(E_WID_X * int(x) / w)
            yn = int(E_WID_Y * int(y) / h)
            self.midkey.set_mouse_pos_norm(xn, yn)

    def key_press(self, keyname: str):
        if self.midkey:
            self.midkey.key_press(keyname)

    def key_release(self, keyname: str):
        if self.midkey:
            self.midkey.key_release(keyname)

    def end_of_frame(self):
        # 不要
        return

    def close_request(self):
        if self.appli:
            self.appli.set_exit()
