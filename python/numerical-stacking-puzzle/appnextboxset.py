"""
Copyright 2026 Miyabi
Project Name  : Numerical Stacking Puzzle (Python)
File Name     : appnextboxset.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 次回箱オブジェクト全体管理
"""

#
# Import
#
from __future__ import annotations
from routine import Routine
from define import dec_val, E_NEXT_BOX_NUM
from nextdat import NextDat
from appbox import AppBox

#
# Definition
#

# シーケンス
E_SEQ_INIT = 0  # 初期状態
E_SEQ_IDLE = 1  # 待機状態
E_SEQ_PUSH = 2  # PUSH動作状態

# タイマー
E_TM_PUSH = 0   # PUSH動作状態の処理時間

E_APP_NEXT_BOX_SET_STAT_IDLE = 0
E_APP_NEXT_BOX_SET_STAT_PUSH = 1

#
# Definition of Class
#
class AppNextBoxSet(Routine):
    _inst = None

    @staticmethod
    def get_inst():
        return AppNextBoxSet._inst

    #----- Constructor(s)/Destructor
    def __init__(self):
        """コンストラクタ
        """
        super().__init__()
        AppNextBoxSet._inst = self
        #------------------------------------------------------------------------
        # 初期化処理
        self.set_child_num(E_NEXT_BOX_NUM)

        #------------------------------------------------------------------------
        # 初期値設定
        self.stat = E_APP_NEXT_BOX_SET_STAT_IDLE
        self.tm_seq = 0         # シーケンスタイマー <- 0
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態

        # 初期化正常終了
        self.init_ok_mark()     

    def create_gui_child(self) -> bool:
        """GUI系の子プロセスの生成

        Returns:
            False=生成失敗
        """
        #------------------------------------------------------------------------
        # 次落下箱管理インスタンスを取得する
        nd = NextDat.get_inst()
        if nd is None:
            return False

        #------------------------------------------------------------------------
        # 次回箱データ毎に次回箱オブジェクトの生成
        for x in range(E_NEXT_BOX_NUM):
            v = nd.get_val(x)
            self.children[x] = AppBox(v, x*100 + 1200, 0)
        return True

    def is_child_alive(self) -> bool:
        """子プロセスの生存判定

        Returns:
            True=すべて生存
        """
        return all(ch is not None for ch in (self.children or []))

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
        self.tm_seq = dec_val(self.tm_seq, 1)

        #------------------------------------------------------------------------
        # シーケンス処理
        # シーケンス処理:初期状態
        if self.seq == E_SEQ_INIT:
            #--------------------------------------------------------------------
            # シーケンス処理:初期状態
            if not self.create_gui_child():
                return False
            # シーケンス -> 待機状態
            self.seq = E_SEQ_IDLE
            return True

        #------------------------------------------------------------------------
        # シーケンス処理:待機状態
        if self.seq == E_SEQ_IDLE:
            #--------------------------------------------------------------------
            # 枝葉ルーチン生存していない -> 強制終了
            if not self.is_child_alive():
                return False
            #--------------------------------------------------------------------
            # 次回箱全体処理状態がPUSH動作の場合はPUSH動作状態へ
            if self.stat == E_APP_NEXT_BOX_SET_STAT_PUSH:
                nd = NextDat.get_inst()
                if nd is None:
                    return False
                for x in range(E_NEXT_BOX_NUM):
                    v = nd.get_val(x)
                    old = self.children[x]
                    if old is not None:
                        try:
                            old.destroy()
                        except Exception:
                            pass
                        self.children[x] = None
                    b = AppBox(v, x*100 + 1300, 0)
                    b.set_stat_serve(x*100 + 1200, 0)
                    self.children[x] = b
                # シーケンス -> PUSH動作状態
                self.seq = E_SEQ_PUSH
                self.tm_seq = E_TM_PUSH
            return True

        #------------------------------------------------------------------------
        # シーケンス＝PUSH状態
        if self.seq == E_SEQ_PUSH:
            #--------------------------------------------------------------------
            # タイマーカウント後は待機状態へ
            if self.tm_seq == 0:
                # タイマーカウント後
                # 次回箱全体処理状態=待機状態
                self.stat = E_APP_NEXT_BOX_SET_STAT_IDLE
                # シーケンス -> 待機状態
                self.seq = E_SEQ_IDLE
            return True

        return True

    def set_stat_push(self):
        """次回箱全体処理状態をPUSH動作に設定
        """
        self.stat = E_APP_NEXT_BOX_SET_STAT_PUSH
