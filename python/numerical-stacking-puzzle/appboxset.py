"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : appboxset.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 箱オブジェクト全体管理
"""
#
# Import
#
from __future__ import annotations
from routine import Routine
from define import dec_val, E_CELL_X_NUM, E_CELL_Y_NUM
from appbox import AppBox, E_APP_BOX_STAT_SLIDE

#
# Definition
#

# 箱全体処理状態
E_APP_BOX_SET_STAT_IDLE  = 0    # 箱全体処理状態：待機
E_APP_BOX_SET_STAT_STACK = 1    # 箱全体処理状態：積上動作
E_APP_BOX_SET_STAT_MERGE = 2    # 箱全体処理状態：結合動作
E_APP_BOX_SET_STAT_LAND  = 3    # 箱全体処理状態：着地動作

# シーケンス
E_SEQ_INIT  = 0 # 初期状態
E_SEQ_IDLE  = 1 # 待機状態
E_SEQ_STACK = 2 # 積上動作状態
E_SEQ_MERGE = 3 # 結合動作状態
E_SEQ_LAND  = 4 # 着地動作状態

# タイマー
E_TM_STACK = 0  # 積上動作状態の処理時間
E_TM_MERGE = 15 # 結合動作状態の処理時間
E_TM_LAND  = 0  # 着地動作状態の処理時間

# 子プロセス
E_RT_NUM = 160  # 子プロセスの上限数

#
# Definition of Class
#
class AppBoxSet(Routine):
    #----- Constructor(s)/Destructor
    def __init__(self, cell_dat):
        """コンストラクタ
        """
        super().__init__()

        #------------------------------------------------------------------------
        # 初期化処理
        self.set_child_num(E_RT_NUM)
        self.cell = cell_dat

        #------------------------------------------------------------------------
        # 初期値設定
        # バッファクリア
        self.box_rt = [[-1 for _ in range(E_CELL_Y_NUM)] for _ in range(E_CELL_X_NUM)]

        #------------------------------------------------------------------------
        # 初期値設定
        self.stat = E_APP_BOX_SET_STAT_IDLE
        self.tm_seq = 0         # シーケンスタイマー <- 0
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態

        # 初期化正常終了
        self.init_ok_mark()

    def _alloc_slot(self) -> int:
        """子プロセスの空きを探して空いているIDを返す

        Returns:
            空いているID
        """
        for i, ch in enumerate(self.children):
            if ch is None:
                return i
        return -1

    def create_gui_child(self) -> bool:
        """GUI系の子プロセスの生成
        
        Returns:
            False=生成失敗
        """
        #------------------------------------------------------------------------
        # セルデータ毎に箱オブジェクトの生成
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                v = self.cell.get_val(x, y)
                if v >= 0:
                    rt = self._alloc_slot()
                    if rt < 0:
                        return False
                    self.children[rt] = AppBox(v, x*100, y*100)
                    self.box_rt[x][y] = rt

        return True

    def is_child_alive(self) -> bool:
        """子プロセスの生存判定
        
        Returns:
            True=すべて生存
        """
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                rt = self.box_rt[x][y]
                if rt >= 0 and self.children[rt] is None:
                    return False

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
        # タイマー
        self.tm_seq = dec_val(self.tm_seq, 1)

        #------------------------------------------------------------------------
        # シーケンス処理
        # シーケンス処理:初期状態
        if self.seq == E_SEQ_INIT:
            #--------------------------------------------------------------------
            # GUI系の子プロセスの生成
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
            # 箱全体処理状態が積上動作の場合は積上動作状態へ
            if self.stat == E_APP_BOX_SET_STAT_STACK:
                # 箱に対応する箱オブジェクトが無い場合は生成する
                for x in range(E_CELL_X_NUM):
                    for y in range(E_CELL_Y_NUM):
                        v = self.cell.get_val(x, y)
                        if v < 0:
                            v = self.cell.get_stack_val(x, y)
                        if v >= 0 and self.box_rt[x][y] < 0:
                            rt = self._alloc_slot()
                            if rt < 0: return False
                            self.children[rt] = AppBox(v, x*100, y*100)
                            self.box_rt[x][y] = rt
                # シーケンス -> 結合動作状態
                self.seq = E_SEQ_STACK
                self.tm_seq = E_TM_STACK
            #--------------------------------------------------------------------
            # シーケンス -> 結合動作状態
            if self.stat == E_APP_BOX_SET_STAT_MERGE:
                # 結合する箱を「スライド移動」「種類変更」の状態に更新
                # （結合位置にある箱＝種類変更、結合位置にない箱＝結合位置までスライド移動）
                for x in range(E_CELL_X_NUM):
                    for y in range(E_CELL_Y_NUM):
                        rt = self.box_rt[x][y]
                        if rt < 0: continue
                        mx = self.cell.get_merge_x(x, y)
                        my = self.cell.get_merge_y(x, y)
                        if mx >= 0 and my >= 0:
                            box = self.children[rt]
                            if box is None: continue
                            if mx == x and my == y:
                                mv = self.cell.get_merge_val(x, y)
                                if mv > box.get_box_type_id():
                                    box.set_stat_change(mv)
                            else:
                                box.set_stat_slide(mx*100, my*100)
                                self.box_rt[x][y] = -1
                # シーケンス -> 結合動作状態
                self.seq = E_SEQ_MERGE
                self.tm_seq = E_TM_MERGE
            #--------------------------------------------------------------------
            # 箱全体処理状態が着地動作の場合は着地動作状態へ
            if self.stat == E_APP_BOX_SET_STAT_LAND:
                # 着地する箱＝落下状態に設定
                for x in range(E_CELL_X_NUM):
                    for y in range(E_CELL_Y_NUM-1, -1, -1):
                        rt = self.box_rt[x][y]
                        if rt < 0: continue
                        ly = self.cell.get_land_y(x, y)
                        if ly >= 0:
                            box = self.children[rt]
                            if box is not None:
                                box.set_stat_fall(x*100, ly*100)
                            self.box_rt[x][y] = -1
                            self.box_rt[x][ly] = rt
                # シーケンス -> 着地動作状態
                self.seq = E_SEQ_LAND
                self.tm_seq = E_TM_LAND
            return True

        #------------------------------------------------------------------------
        # シーケンス＝積上状態
        if self.seq == E_SEQ_STACK:
            # タイマーカウント中は待機(積上中)
            # タイマーカウント後は待機状態へ
            if self.tm_seq == 0:
                # 箱全体処理状態=待機状態
                self.stat = E_APP_BOX_SET_STAT_IDLE
                # シーケンス -> 待機状態
                self.seq = E_SEQ_IDLE
            return True

        #------------------------------------------------------------------------
        # シーケンス＝結合状態
        if self.seq == E_SEQ_MERGE:
            #--------------------------------------------------------------------
            # タイマーカウント中は待機(結合中)
            # タイマーカウント後は待機状態へ
            if self.tm_seq == 0:
                for i, ch in enumerate(self.children):
                    if ch is None: continue
                    if isinstance(ch, AppBox):
                        if ch.get_stat() == E_APP_BOX_STAT_SLIDE:
                            try:
                                ch.destroy()
                            except Exception:
                                pass
                            self.children[i] = None
                # 箱全体処理状態=待機状態
                self.stat = E_APP_BOX_SET_STAT_IDLE
                # シーケンス -> 待機状態
                self.seq = E_SEQ_IDLE
            return True

        #------------------------------------------------------------------------
        # シーケンス＝着地状態
        if self.seq == E_SEQ_LAND:
            # タイマーカウント中は待機(着地中)
            # タイマーカウント後は待機状態へ
            if self.tm_seq == 0:
                # タイマーカウント後
                # 箱全体処理状態=待機状態
                self.stat = E_APP_BOX_SET_STAT_IDLE
                # シーケンス -> 待機状態
                self.seq = E_SEQ_IDLE
            return True
        return True

    def set_stat_stack(self):
        """箱全体処理状態を積上動作に設定
        """
        self.stat = E_APP_BOX_SET_STAT_STACK

    def set_stat_merge(self):
        """箱全体処理状態を結合動作に設定
        """
        self.stat = E_APP_BOX_SET_STAT_MERGE

    def set_stat_land(self):
        """箱全体処理状態を着地動作に設定
        """
        self.stat = E_APP_BOX_SET_STAT_LAND
