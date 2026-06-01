"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : applblset.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : ラベルオブジェクト全体管理
"""
#
# Import
#
from __future__ import annotations

from routine import Routine
from applbl import AppLbl, E_APP_LBL_TYPE_MERGED_LBL, E_APP_LBL_TYPE_SCORE_VAL, E_APP_LBL_TYPE_ELAPSED_VAL, E_APP_LBL_TYPE_MERGED_VAL_1, E_APP_LBL_TYPE_NUM

#
# Definition
#

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態

#
# Definition of Class
#
class AppLblSet(Routine):
    #----- Constructor(s)/Destructor
    _inst = None

    @staticmethod
    def get_inst():
        """インスタンス取得

        Returns:
            インスタンス
        """
        return AppLblSet._inst

    def __init__(self):
        """コンストラクタ
        """
        super().__init__()
        AppLblSet._inst = self
        # 子プロセス
        self.set_child_num(E_APP_LBL_TYPE_NUM)
        # ラベル種類毎にラベルオブジェクトの生成
        for i in range(E_APP_LBL_TYPE_NUM):
            self.children[i] = AppLbl(i)

        # 初期化正常終了
        self.init_ok_mark()

    def update(self) -> bool:
        """定期更新処理

        Returns:
            False=処理終了（オブジェクト破棄）

        Notes:
            呼び出し周期＝1/60sec
        """
        #--------------------------------------------------------------------
        # 基底クラスの処理
        return super().update()

    def set_score_val(self, score: int):
        """スコア表示値を設定

        Args:
                表示値
        """
        self.children[E_APP_LBL_TYPE_SCORE_VAL].set_val(score, 0)

    def set_elapsed_time_val(self, ticks: int):
        """経過時間表示値を設定

        Args:
            表示値
        """
        self.children[E_APP_LBL_TYPE_ELAPSED_VAL].set_val(ticks, 0)

    def set_merge_time_val(self, idx: int, ticks: int, pow_val: int):
        """マージ時間表示値を設定
        
        Args:
            表示値
        """
        if 0 <= idx < 5:
            self.children[E_APP_LBL_TYPE_MERGED_VAL_1 + idx].set_val(ticks, pow_val)
