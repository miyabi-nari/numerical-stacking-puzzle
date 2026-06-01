"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : midgui.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : GUI描画のミドルウェア
"""

#
# Import
#
from __future__ import annotations
from dataclasses import dataclass
from typing import Optional, Dict, Tuple

from PySide6.QtGui import QPixmap, QPainter, QColor, QFont
from PySide6.QtCore import QRect, Qt

from define import E_WID_X, E_WID_Y, dec_val

#
# Definition
#

E_MIDGUI_RECT_NUM = 1024
E_MIDGUI_TXT_NUM  = 1024

# 矩形ID
E_MIDGUI_RECT_KEY_IND = 0   # 矩形ID:キー表示用の先頭ID
E_MIDGUI_RECT_BOX     = 200 # 矩形ID:箱用の先頭ID
# テキストID
E_MIDGUI_TXT_KEY_IND  = 0   # テキストID:キー表示用の先頭ID
E_MIDGUI_TXT_LBL      = 100 # テキストID:ラベル用の先頭ID
E_MIDGUI_TXT_BOX      = 150 # テキストID:箱用の先頭ID

@dataclass
class RectCmd:
    pos: Tuple[int,int,int,int]
    frame: Tuple[int,int,int]
    fill: Tuple[int,int,int]

@dataclass
class TextCmd:
    pos: Tuple[int,int,int,int]
    color: Tuple[int,int,int]
    text: str
    pt: int
    left_fit: bool

#
# Definition of Class
#
class MidGui:
    _inst: Optional["MidGui"] = None

    @staticmethod
    def get_inst():
        return MidGui._inst

    #----- Constructor(s)/Destructor
    def __init__(self, width: int, height: int):
        """コンストラクタ

        Args:
            ウィンドウサイズ
        """
        #------------------------------------------------------------------------
        # インスタンスポインタ保存
        MidGui._inst = self

        #------------------------------------------------------------------------
        # 初期値設定
        self.disable_draw_flag = False
        self.fps_div = 1
        self.tm_fps = self.fps_div

        #------------------------------------------------------------------------
        # バッファクリア
        self.rects: Dict[int, RectCmd] = {}
        self.texts: Dict[int, TextCmd] = {}

        self._w = max(1, int(width))
        self._h = max(1, int(height))

        # ★追加：スケール
        self._sx = self._w / E_WID_X
        self._sy = self._h / E_WID_Y

        # ★追加：フォント幅伸縮ON/OFF
        self.enable_font_stretch = True

        # ★追加：伸縮率の下限/上限（暴れ防止）
        self.font_stretch_min = 60   # 60%（細くなる下限）
        self.font_stretch_max = 180  # 180%（太くなる上限）

        self._pixmap = QPixmap(self._w, self._h)
        self._pixmap.fill(QColor("white"))

    def on_resize(self, w: int, h: int):
        self._w = max(1, int(w))
        self._h = max(1, int(h))

        # ★追加：スケール更新
        self._sx = self._w / E_WID_X
        self._sy = self._h / E_WID_Y

        self._pixmap = QPixmap(self._w, self._h)
        self._pixmap.fill(QColor("white"))

    def disable_draw(self, flag: bool):
        """描画禁止FGの更新
        """
        self.disable_draw_flag = bool(flag)

    def set_fps(self, n: int) -> bool:
        """FPSの設定

        Args:
            FPSの設定値

        Returns:
            FPSの設定可否
        """
        if n <= 0 or n >= 60:
            return False
        self.fps_div = n
        return True

    def get_unset_rect_id(self, start: int) -> int:
        """未使用の矩形IDを取得する

        Args:
            start=空きを探す先頭の矩形ID

        Return:
            未使用の矩形ID ただし発見できなかった場合は-1を返す             *
        """
        for i in range(max(0, start), E_MIDGUI_RECT_NUM):
            if i not in self.rects:
                return i
        return -1

    def get_unset_txt_id(self, start: int) -> int:
        """未使用のテキストIDを取得する

        Args:
            start=空きを探す先頭のテキストID

        Returns:
            未使用のテキストID ただし発見できなかった場合は-1を返す
        """
        for i in range(max(0, start), E_MIDGUI_TXT_NUM):
            if i not in self.texts:
                return i
        return -1

    def set_rect(self, rid: int, pos, frame, fill, bset: bool=True):
        """描画する矩形を登録する

        Args:
            rid=矩形ID, pos=矩形位置
            frame=枠線色, fill=塗潰し色
            bset=登録(=True)/削除(=False)
        """
        if not bset:
            self.rects.pop(rid, None)
            return
        self.rects[rid] = RectCmd(tuple(pos), tuple(frame), tuple(fill))

    def clr_rect(self, rid: int):
        """描画する矩形を登録削除する

        Args:
            nRectID=矩形ID
        """
        self.rects.pop(rid, None)

    def set_txt(self, tid: int, pos, color, text: str, pt: int, left_fit: bool, bset: bool=True):
        """描画するテキストを登録する
        
        Args:
            tid=テキストID, pos=テキスト位置
            color=テキスト色, str=文字列, pt=文字サイズ,
            left_fit=TRUEを設定すると左端揃え描画
            bset=登録(=True)/削除(=False)
        
        Returns:
            none
        """
        if not bset:
            self.texts.pop(tid, None)
            return
        self.texts[tid] = TextCmd(tuple(pos), tuple(color), text or "", int(pt), bool(left_fit))

    def clr_txt(self, tid: int):
        """描画するテキストを登録削除する
        
        Args:
            tid=テキストID
        """
        self.texts.pop(tid, None)

    def _render(self):
        """矩形とテキストの描画処理
        """
        self._pixmap.fill(QColor("white"))
        p = QPainter(self._pixmap)

        # 矩形ID毎に描画処理する
        # 有効な矩形IDのみ処理する
        for cmd in self.rects.values():
            # 矩形サイズ決定(クライアント領域に合わせて伸縮)
            l, t, r, b = cmd.pos
            x1 = int(l * self._sx)
            y1 = int(t * self._sy)
            x2 = int(r * self._sx)
            y2 = int(b * self._sy)
            # 塗り潰し -> fillrect
            p.fillRect(x1, y1, max(1, x2 - x1), max(1, y2 - y1), QColor(*cmd.fill))
            # 枠線 -> drawrect
            p.setPen(QColor(*cmd.frame))
            p.drawRect(x1, y1, max(1, x2 - x1), max(1, y2 - y1))

        # テキストID毎に描画処理する
        # 有効なテキストIDのみ処理する
        for cmd in self.texts.values():
            # 矩形サイズ決定(クライアント領域に合わせて伸縮)
            l, t, r, b = cmd.pos
            x1 = int(l * self._sx)
            y1 = int(t * self._sy)
            x2 = int(r * self._sx)
            y2 = int(b * self._sy)
            # フォント高さ：縦スケールで追従（従来通り）
            px_h = max(1, int(0.8 * cmd.pt * self._sy))
            # フォントハンドル生成
            font = QFont("Noto Sans CJK JP")  # WindowsならフォールバックしてもOK
            font.setPixelSize(px_h)
            # ★フォント幅：横スケールに追従（setStretchは%）
            if self.enable_font_stretch:
                # 横/縦の比率で幅を調整（縦だけ伸びると横が細く見えるのを補正）
                ratio = 1.4 * self._sx / self._sy if self._sy != 0 else 1.0
                stretch = int(100 * ratio)
                # 暴れ防止にクランプ
                if stretch < self.font_stretch_min:
                    stretch = self.font_stretch_min
                if stretch > self.font_stretch_max:
                    stretch = self.font_stretch_max

                font.setStretch(stretch)
            # フォントをセット
            p.setFont(font)
            # DrawText
            p.setPen(QColor(*cmd.color))
            align = Qt.AlignLeft if cmd.left_fit else Qt.AlignHCenter
            p.drawText(QRect(x1, y1, max(1, x2-x1), max(1, y2-y1)),
                       align | Qt.AlignVCenter | Qt.TextSingleLine,
                       cmd.text)
        p.end()

    def update(self) -> bool:
        """定期更新処理
        
        Returns:
            False=処理終了（オブジェクト破棄）
        
        Notes:
            呼び出し周期＝1/60sec
        """
        if self.tm_fps == 0:
            if not self.disable_draw_flag:
                self._render()
            self.tm_fps = self.fps_div
        else:
            self.tm_fps = dec_val(self.tm_fps, 1)
        return True

    def blit(self, painter: QPainter):
        painter.drawPixmap(0, 0, self._pixmap)
