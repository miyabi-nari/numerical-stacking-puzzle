"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : timerlib.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : タイマー制御クラスライブラリ
Attention     : Winmm.lib 必須
                mmsystem.h 必須
"""
#
# Import
#
from __future__ import annotations
import time

#
# Definition
#
TIMER_ISIDLE = 0    # タイマー到達まで余裕あり
TIMER_ISSOON = 1    # タイマー到達直前
TIMER_ISJUST = 2    # タイマー到達
TIMER_ISOVER = 3    # タイマー超過

#
# Definition of Class
#
class TimerLib:
    #----- Constructor(s)/Destructor
    def __init__(self):
        """コンストラクタ
        """
        self.fps = 60
        self.period = 1.0 / 60.0
        self.last = time.perf_counter()
        self.resolution = 0.001

    def create(self, resolution_ms: int) -> bool:
        """タイマー初期化処理（タイマー最大精度取得と設定）

        Args:
            resolution_ms=希望するタイマー分解能(msec)

        Returns:
            初期化 成功:True 失敗:False
        """
        self.resolution = max(0.001, resolution_ms / 1000.0)
        return True

    def start(self, fps: int) -> bool:
        """タイマー動作開始

        Args:
            fps=希望するタイマー周波数（Hz）
            0を指定した場合，デフォルトのFPS(=E_FPS_DEFAULT)を用いる

        Returns 
            開始 成功:True 失敗:False
        """
        self.fps = max(1, int(fps))
        self.period = 1.0 / self.fps
        self.last = time.perf_counter()
        return True

    def get_time_stat(self) -> int:
        """タイマー状態取得

        Returns:
            TIMER_ISOVER=タイマー超過
            TIMER_ISJUST=タイマー到達
            TIMER_ISSOON=タイマー到達直前
            TIMER_ISIDLE=タイマー到達まで余裕あり

        Notes:
            現在時刻と次回実行時刻の差から状態を判定
            １秒間の間にStart函数で指定した回数、TIMER_ISJUST/OVERを返す。
            TIMER_ISJUST/OVERに合わせて処理を行う事でタイマーとして利用。
            TIMER_ISSOONが返った場合，タイマー到達が近いので、
            TIMER_ISJUST/OVERが返るまで繰返しこの函数を呼んで監視する事。
        """
        # タイマーアップ時刻 － タイマー現在時刻
        now = time.perf_counter()
        dt = now - self.last

        if dt < self.period - self.resolution:
            # タイマー到達まで余裕あり
            return TIMER_ISIDLE
        if dt < self.period:
            # タイマー到達間近
            return TIMER_ISSOON
        if dt > self.period + self.resolution:
            # タイマー超過
            self.last = now         # 次回タイマー到達時刻 set
            return TIMER_ISOVER

        # タイマー到達
        self.last += self.period    # 次回タイマー到達時刻 set
        return TIMER_ISJUST
