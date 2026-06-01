"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : appkeyindset.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : キー表示オブジェクト全体管理
"""
#
# Import
#
from __future__ import annotations

from routine import Routine
from define import E_CELL_X_NUM
from appkeyind import AppKeyInd

#
# Definition
#

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態

#
# Definition of Class
#
class AppKeyIndSet(Routine):
    def __init__(self):
        """コンストラクタ
        """
        super().__init__()
        #------------------------------------------------------------------------
        # 初期化処理
        self.set_child_num(E_CELL_X_NUM * 2)

        #------------------------------------------------------------------------
        # 初期値設定
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態
        # 初期化正常終了
        self.init_ok_mark()

    def create_gui_child(self) -> bool:
        """GUI系の子プロセスの生成

        Returns:
            False=生成失敗
        """
        rt = 0
        #------------------------------------------------------------------------
        # セルデータ毎にキー表示オブジェクトの生成
        for row in range(2):
            for x in range(E_CELL_X_NUM):
                self.children[rt] = AppKeyInd(x, x*100, row*800)
                rt += 1
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
            # GUI系の子プロセスの生成
            if not self.create_gui_child():
                return False
            # シーケンス -> 待機状態
            self.seq = E_SEQ_IDLE
            return True
        # シーケンス処理:待機状態
        return True
