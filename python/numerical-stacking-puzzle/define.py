"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : define.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 全ファイル共通宣言
"""

#
# Import
#
from __future__ import annotations

#
# Definition
#

E_APP_NAME_STR = "Numerical Stacking Puzzle"    # アプリケーション名
E_APP_VER_STR  = "Version 0.02"                 # アプリケーションバージョン文字列
E_APP_VER_NUM  = 1                              # アプリケーションバージョン番号
E_APP_CR_STR   = "Copyright(C) 2026 Miyabi."    # アプリケーション著作権情報

E_APP_SAVE_ID_KEY = "N.S.P.SaveData.Version"    # 保存データの識別キー
E_APP_SAVE_ID_NUM = 1                           # 保存データの識別Ver

E_WID_X = 1600
E_WID_Y = 900

#
# 上限値
#
E_CELL_X_NUM = 11   # セル数(横方向)
E_CELL_Y_NUM = 7    # セル数(縦方向)

E_NEXT_BOX_NUM = 4  # 次回箱の数

# 数値評価用マクロ
def numok(n: int, m: int) -> bool:
    return 0 <= n < m

def max2(a: int, b: int) -> int:
    return a if a > b else b

def dec_val(n: int, m: int = 1) -> int:
    # C++ inline Dec(long &n, long m=1)
    return 0 if n <= m else n - m

# 矩形幅算出
def rect_width(rc) -> int:
    return rc[2] - rc[0]

# 矩形高さ算出
def rect_height(rc) -> int:
    return rc[3] - rc[1]

# 環境設定を取得
def get_env(key: str, default: int) -> int:
    return default
# 環境設定を設定
def save_env(key: str, value: int) -> None:
    pass
