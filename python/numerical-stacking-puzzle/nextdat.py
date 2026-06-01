"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : nextdat.cpp
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 次に落下する箱のデータ管理
"""
#
# Import
#
from __future__ import annotations
from routine import Routine
from define import dec_val, E_NEXT_BOX_NUM

#
# Definition
#

E_NEXT_BOX_VAL_NUM = 6

#
# Definition of Class
#
class NextDat(Routine):
    _inst = None

    @staticmethod
    def get_inst():
        return NextDat._inst

    def __init__(self):
        """コンストラクタ
        """
        super().__init__()
        #------------------------------------------------------------------------
        # インスタンスポインタ保存
        NextDat._inst = self

        #------------------------------------------------------------------------
        # 初期値設定
        self.tm_box_val = 6     # 箱の値ランダムタイマ
        self.next_box_cnt = 0   # 次回箱の生成数 <- 0

        #------------------------------------------------------------------------
        # バッファクリア
        self.next_vals = [-1] * E_NEXT_BOX_NUM

        #------------------------------------------------------------------------
        # この時点で次回箱の中身をセットする
        for n in range(E_NEXT_BOX_NUM):
            self.next_vals[n] = self.next_box_cnt
            self.next_box_cnt += 1

        #------------------------------------------------------------------------
        # 初期化正常終了
        self.init_ok_mark()

    def _get_next_box_num(self) -> int:
        """次回箱の値を算出する

        Returns:
            次回箱の値
        """
        # 最初は0～E_NEXT_BOX_VAL_NUM-1の値を順に返す
        # 以降は処理時間 % E_NEXT_BOX_VAL_NUMの疑似乱数で値を返す
        if self.next_box_cnt < E_NEXT_BOX_VAL_NUM:
            ret = self.next_box_cnt
        else:
            ret = self.tm_box_val % E_NEXT_BOX_VAL_NUM
        self.next_box_cnt += 1
        return ret

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
        # タイマー
        self.tm_box_val = dec_val(self.tm_box_val, 1)
        if self.tm_box_val == 0:
            self.tm_box_val = E_NEXT_BOX_VAL_NUM
        return True

    def get_val(self, x: int) -> int:
        """次回箱の格納値を取得

        Args:
            次回箱の格納位置X

        Returns:
            格納値
        """
        return self.next_vals[x]

    def exec_push(self) -> int:
        """次回箱の落下指示
        
        Returns:
            落下する次回箱の格納値
        """
        ret = self.next_vals[0]
        # セルデータサイズ分繰り返す
        for i in range(E_NEXT_BOX_NUM-1):
            self.next_vals[i] = self.next_vals[i+1]
        self.next_vals[E_NEXT_BOX_NUM-1] = self._get_next_box_num()
        return ret
