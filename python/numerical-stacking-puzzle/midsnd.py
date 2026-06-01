"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : midsnd.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 音再生のミドルウェア
"""

#
# Import
#
from __future__ import annotations

from PySide6.QtMultimedia import QSoundEffect
from PySide6.QtCore import QUrl

#
# Definition
#

# 音ID
E_MIDSND_STACK   = 0    # 音ID:スタック音
E_MIDSND_MERGE_1 = 1    # 音ID:マージ音1
E_MIDSND_MERGE_2 = 2    # 音ID:マージ音2
E_MIDSND_MERGE_3 = 3    # 音ID:マージ音3
E_MIDSND_NUM     = 4    # 音IDの総数

#
# Definition of Class
#
class MidSnd:
    _inst = None

    @staticmethod
    def get_inst():
        return MidSnd._inst

    #----- Constructor(s)/Destructor
    def __init__(self):
        """コンストラクタ
        """
        #------------------------------------------------------------------------
        # インスタンスポインタ保存
        MidSnd._inst = self

        #------------------------------------------------------------------------
        # 初期値設定
        self.disable_play = False   # 再生禁止FG <- OFF

        #------------------------------------------------------------------------
        # バッファクリア
        self._snd_set = [False] * E_MIDSND_NUM
        # QSoundEffectインスタンス
        self._snd = [None] * E_MIDSND_NUM

        self._init_sound()

    def _init_sound(self):
        """
        wavファイルをロード
        同じフォルダに置く想定
        """

        filelist = [
            "stack.wav",
            "merge1.wav",
            "merge2.wav",
            "merge3.wav",
        ]

        for i in range(E_MIDSND_NUM):
            snd = QSoundEffect()
            snd.setSource(QUrl.fromLocalFile(filelist[i]))
            snd.setVolume(0.25)
            self._snd[i] = snd

    def get_disable_play(self) -> bool:
        """再生禁止FGの取得
        """
        return self.disable_play

    def set_disable_play(self, flag: bool):
        """再生禁止FGの更新
        """
        self.disable_play = bool(flag)

    def set_snd(self, snd_id: int):
        """再生する音声を登録する
    
        Args:
            snd_id=音ID
        """
        if 0 <= snd_id < E_MIDSND_NUM:
            self._snd_set[snd_id] = True

    def update(self) -> bool:
        """定期更新処理

        Returns:
            False=処理終了（オブジェクト破棄）
        
        Notes:
            呼び出し周期＝1/60sec
        """
        if self.disable_play:
            # フラグだけ消す
            self._snd_set = [False] * E_MIDSND_NUM
            return True

        # 音ID毎に再生処理する
        for i in range(E_MIDSND_NUM):
            # 有効な音IDのみ処理する
            if self._snd_set[i]:
                snd = self._snd[i]
                if snd is not None:
                    # 音を非同期で再生
                    snd.stop()
                    snd.play()

        # フラグは1フレームで消える（C++と同じ）
        self._snd_set = [False] * E_MIDSND_NUM

        return True
