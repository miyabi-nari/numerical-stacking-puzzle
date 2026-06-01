"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : titlelblset.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : タイトル用ラベルオブジェクト全体管理
"""
#
# Import
#
from __future__ import annotations

from routine import Routine
from titlelbl import TitleLbl, E_TITLE_LBL_TYPE_NUM

#
# Definition
#

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態

#
# Definition of Class
#
class TitleLblSet(Routine):
    """コンストラクタ
    """
    def __init__(self):
        super().__init__()
        #------------------------------------------------------------------------
        # 初期化処理
        # 子プロセス
        self.set_child_num(E_TITLE_LBL_TYPE_NUM)

        #------------------------------------------------------------------------
        # 初期値設定
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態

        #------------------------------------------------------------------------
        # 初期化正常終了
        self.init_ok_mark()

    def create_children(self) -> bool:
        """GUI系の子プロセスの生成

        Returns:
            False=生成失敗
        """
        #------------------------------------------------------------------------
        # ラベル種類毎にラベルオブジェクトの生成
        for i in range(E_TITLE_LBL_TYPE_NUM):
            self.children[i] = TitleLbl(i)
        return True

    def destroy_gui(self):
        """C++のデストラクタに代わる解放処理

        Notes:
            pythonのデストラクタはGCが非同期で呼び出されるため、
            インスタンス解放時の処理はこれを明示的に呼び出して代用する。
        """
        if not self.children:
            return
        for ch in self.children:
            if ch:
                ch.destroy_gui()

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
            # GUI系の子プロセスの生成
            if not self.create_children():
                return False
            self.seq = E_SEQ_IDLE
            return True
        return True
