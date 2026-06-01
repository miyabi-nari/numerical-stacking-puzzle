"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : midkey.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : Key入力のミドルウェア
"""
from __future__ import annotations
from dataclasses import dataclass
from typing import Optional

# define.h の E_CELL_X_NUM = 11 と一致（qwerty[0..10]）
E_QWERTY_NUM = 11  # Z,X,C,V,B,N,M,Comma,Period,Slash,Backslash

@dataclass
class KeyOn:
    qwerty: list[bool]
    esc: bool

@dataclass
class KeyDown:
    qwerty: list[bool]
    esc: bool

class MidKey:
    """
    方針:
      - key_press/key_release は「押しっぱなし状態(KeyOn)」だけ更新
      - KeyDown（押した瞬間）は MidKey.update() 内で
        prev_on と on の差分から生成（edge検出）
    """
    _inst: Optional["MidKey"] = None

    @staticmethod
    def get_inst():
        return MidKey._inst

    def __init__(self):
        """コンストラクタ
        """
        MidKey._inst = self

        # 押しっぱなし（現在）
        self._on_qwerty = [False] * E_QWERTY_NUM
        self._on_esc = False

        # 前フレームの押しっぱなし
        self._prev_on_qwerty = [False] * E_QWERTY_NUM
        self._prev_on_esc = False

        # 押した瞬間（このフレームの結果）
        self._down_qwerty = [False] * E_QWERTY_NUM
        self._down_esc = False

        # マウス（必要なら）
        self._mouse_l = False
        self._mx = 0
        self._my = 0

    def set_mouse_l_in(self, down: bool):
        """キー入力情報(input)を設定(マウスLクリック)
        """
        self._mouse_l = bool(down)

    def set_mouse_pos_norm(self, x_norm: int, y_norm: int):
        self._mx = int(x_norm)
        self._my = int(y_norm)

    # -------------------------
    # keyboard event input
    # -------------------------
    def key_press(self, keyname: str):
        """キー入力情報(input-press)を設定(キー配列)(ESCキー)

        Args:
            キー入力情報(input)

        Notes:
            Qtイベントから呼ばれる（フレーム同期ではない）。
            ここでは KeyOn のみ更新する。
        """
        idx = self._keyname_to_qwerty_index(keyname)
        if idx is not None:
            self._on_qwerty[idx] = True
            return

        if keyname == "ESC":
            self._on_esc = True

    def key_release(self, keyname: str):
        """キー入力情報(input-release)を設定(キー配列)(ESCキー)

        Args:
            キー入力情報(input)

        Notes:
            Qtイベントから呼ばれる（フレーム同期ではない）。
            ここでは KeyOff のみ更新する。
        """
        idx = self._keyname_to_qwerty_index(keyname)
        if idx is not None:
            self._on_qwerty[idx] = False
            return

        if keyname == "ESC":
            self._on_esc = False

    def update(self) -> bool:
        """定期更新処理

        Returns:
            False=処理終了（オブジェクト破棄）
        
        Notes:
            呼び出し周期＝1/60sec
            毎フレーム（MainFrmのRoutineツリー更新）で呼ばれる想定。
            ここで KeyDown を生成するので「1フレームだけ true」が保証される。
        """
        # キー入力立ち上がり検出
        # edge検出： on かつ prev_on でない -> down
        self._down_qwerty = [on and (not prev) for on, prev in zip(self._on_qwerty, self._prev_on_qwerty)]
        self._down_esc = self._on_esc and (not self._prev_on_esc)

        # 前回のキー入力値の保存
        self._prev_on_qwerty = self._on_qwerty[:]
        self._prev_on_esc = self._on_esc

        return True

    def get_key_on(self) -> KeyOn:
        """キー入力情報(確定値)の取得
        """
        return KeyOn(self._on_qwerty[:], self._on_esc)

    def get_key_down(self) -> KeyDown:
        """キー入力情報(ON立上り検出)の取得
        """
        return KeyDown(self._down_qwerty[:], self._down_esc)

    @staticmethod
    def _keyname_to_qwerty_index(keyname: str):
        mapping = {
            "Z": 0, "X": 1, "C": 2, "V": 3, "B": 4, "N": 5, "M": 6,
            "COMMA": 7, "PERIOD": 8, "SLASH": 9, "BACKSLASH": 10,
        }
        return mapping.get(keyname, None)
