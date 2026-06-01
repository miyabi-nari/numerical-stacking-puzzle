"""
Copyright 2026 Miyabi
Project Name  : Numerical Stacking Puzzle (Python)
File Name     : celldat.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : セルのデータ管理
"""

#
# Import
#
from __future__ import annotations
from routine import Routine
from define import E_CELL_X_NUM, E_CELL_Y_NUM

class Cell:
    def __init__(self):
        self.nVal = -1
        self.nStackVal = -1
        self.nMergeVal = -1
        self.nMergeX = -1
        self.nMergeY = -1
        self.nLandY = -1
        self.nLandVal = -1

#
# Definition of Class
#
class CellDat(Routine):
    _inst = None

    @staticmethod
    def get_inst():
        return CellDat._inst

    #----- Constructor(s)/Destructor
    def __init__(self):
        """コンストラクタ
        """
        super().__init__()

        #------------------------------------------------------------------------
        # インスタンスポインタ保存
        CellDat._inst = self

        # バッファクリア
        self.cell = [
            [Cell() for _ in range(E_CELL_Y_NUM)]
            for _ in range(E_CELL_X_NUM)
        ]

        #------------------------------------------------------------------------
        # 初期値設定
        self.score = 0      # 得点 <- 0

        # 初期化正常終了
        self.init_ok_mark()

    def set_val(self, x, y, v):
        """セル格納値を設定

        Args:
            格納位置X,Y
            格納値
        """
        self.cell[x][y].nVal = int(v)

    def get_val(self, x, y):
        """セル格納値を取得

        Args:
            格納位置X,Y

        Returns:
            セル格納値
        """
        return self.cell[x][y].nVal

    def get_stack_val(self, x, y):
        """スタック情報を取得

        Args:
            格納位置X,Y

        Returns:
            スタック情報
        """
        return self.cell[x][y].nStackVal

    def get_merge_val(self, x, y):
        """マージするセルの値を取得

        Args:
            格納位置X,Y

        Returns:
            マージするセルの値
        """
        return self.cell[x][y].nMergeVal

    def get_merge_x(self, x, y):
        """マージ位置を取得

        Args:
            格納位置X,Y

        Returns:
            マージ位置
        """
        return self.cell[x][y].nMergeX

    def get_merge_y(self, x, y):
        """マージ位置を取得

        Args:
            格納位置X,Y

        Returns:
            マージ位置
        """
        return self.cell[x][y].nMergeY

    def get_land_y(self, x, y):
        """落下位置を取得

        Args:
            格納位置X,Y

        Returns:
            落下位置
        """

        return self.cell[x][y].nLandY

    def get_score(self):
        """得点を取得

        Returns:
            得点
        """
        return self.score

    def set_score(self, s):
        """得点を設定

        Args:
            得点
        """
        self.score = int(s)

    def check_en_stack(self, x, y, val):
        """セル格納値の積み上げ可否を検査

        Args:
            格納位置X,Y
            格納値

        Returns:
            積み上げ可否
        """
        # セルデータのスタック情報を全消去
        for ix in range(E_CELL_X_NUM):
            for iy in range(E_CELL_Y_NUM):
                self.cell[ix][iy].nStackVal = -1

        if y >= E_CELL_Y_NUM - 1:
            self.cell[x][y].nStackVal = val
            return True

        below = self.cell[x][y + 1].nVal
        if below >= 0:
            if y >= 1:
                self.cell[x][y].nStackVal = val
                return True
            else:
                if val == below:
                    self.cell[x][y].nStackVal = val
                    return True
        return False

    def exec_stack(self):
        """セルデータの積み上げを実行
        """
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                c = self.cell[x][y]
                if c.nStackVal >= 0:
                    c.nVal = c.nStackVal
                    c.nStackVal = -1

    def check_merge(self, base_x, base_y):
        """セル格納値のマージ可否を検査

        Args:
            格納位置X,Y
            格納値

        Return:マージ可否
            最大マージ値(ポインタ引数返し)
            マージ規模(ポインタ引数返し)
        """
        # セルデータのマージ情報を全消去
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                c = self.cell[x][y]
                c.nMergeX = -1
                c.nMergeY = -1
                c.nMergeVal = -1

        found = False

        # セルデータサイズ分繰り返す(マージ可否検査)
        for x in range(base_x, E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                # 同値セルを再帰的に探索し、連結数をカウントする
                if self._merge_sub(x, y, x, y) > 1:
                    found = True
        for x in range(base_x - 1, -1, -1):
            for y in range(E_CELL_Y_NUM):
                # 同値セルを再帰的に探索し、連結数をカウントする
                if self._merge_sub(x, y, x, y) > 1:
                    found = True

        # マージ可否検査結果をセット
        max_val = -1
        max_diff = -1

        if found:
            # セルデータサイズ分繰り返す
            for x in range(E_CELL_X_NUM):
                for y in range(E_CELL_Y_NUM):
                    c = self.cell[x][y]
                    if c.nVal < c.nMergeVal:
                        if c.nMergeVal > max_val:
                            max_val = c.nMergeVal
                        diff = c.nMergeVal - c.nVal
                        if diff > max_diff:
                            max_diff = diff

        return found, max_val, max_diff

    def _merge_sub(self, x, y, bx, by):
        dirs = [(-1,0),(1,0),(0,-1),(0,1)]
        c = self.cell[x][y]

        if c.nVal == -1:
            return 0

        if c.nMergeX != -1:
            return 0

        if c.nVal != self.cell[bx][by].nVal:
            return 0

        c.nMergeX = bx
        c.nMergeY = by
        count = 1

        for dx, dy in dirs:
            nx, ny = x + dx, y + dy
            if 0 <= nx < E_CELL_X_NUM and 0 <= ny < E_CELL_Y_NUM:
                count += self._merge_sub(nx, ny, bx, by)

        if x == bx and y == by:
            c.nMergeVal = c.nVal + count - 1

        return count

    def exec_merge(self):
        """セル格納値のマージを実行
        """
        # セルデータサイズ分繰り返す
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                c = self.cell[x][y]
                c.nVal = c.nMergeVal
                c.nMergeX = -1
                c.nMergeY = -1
                c.nMergeVal = -1

    def check_en_land(self):
        """セル格納値の着地可否を検査

        Return:
            着地可否
        """
        ret = False

        # セルデータの落下情報を全消去
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                self.cell[x][y].nLandY = -1

        # セルデータサイズ分繰り返す
        for x in range(E_CELL_X_NUM):
            for y in reversed(range(E_CELL_Y_NUM)):
                # 落下可能位置を計算
                if self._land_sub(x, y):
                    ret = True

        return ret

    def _land_sub(self, x, y):
        c = self.cell[x][y]
        if c.nVal == -1:
            return False

        if y >= E_CELL_Y_NUM - 1:
            return False

        for oy in range(y + 1, E_CELL_Y_NUM):
            if self.cell[x][oy].nVal == -1:
                c.nLandY = oy
                return True
            elif self.cell[x][oy].nLandY >= 0:
                c.nLandY = self.cell[x][oy].nLandY - 1
                return True
            else:
                break

        return False

    def exec_land(self):
        """セル格納値の着地を実行
        """
        # セルデータサイズ分繰り返す
        for x in range(E_CELL_X_NUM):
            for y in reversed(range(E_CELL_Y_NUM)):
                land_y = self.cell[x][y].nLandY
                if land_y != -1:
                    self.cell[x][land_y].nVal = self.cell[x][y].nVal
                    self.cell[x][y].nVal = -1
                self.cell[x][y].nLandY = -1

    def calc_score(self):
        """得点計算

        Returns:
            得点増加の有無
        """
        score = 0
        # セルデータの数値を合算する
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                v = self.cell[x][y].nVal
                if 0 <= v <= 30:
                    score += (1 << v)
                if score < 0:
                    # オーバーフロー
                    return False

        self.score = score
        return True

    def update(self):
        """定期更新処理

        Returna:
            False=処理終了（オブジェクト破棄）

        Notes:
            呼び出し周期＝1/60sec
        """
        return True
