"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : appbox.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 箱オブジェクト
"""

#
# Import
#
from __future__ import annotations
from dataclasses import dataclass

from routine import Routine
from define import dec_val, numok
from midgui import MidGui, E_MIDGUI_RECT_BOX, E_MIDGUI_TXT_BOX

#
# Definition
#

# 箱状態
E_APP_BOX_STAT_IDLE   = 0   # 箱状態：アイドル
E_APP_BOX_STAT_SLIDE  = 1   # 箱状態：スライド移動
E_APP_BOX_STAT_CHANGE = 2   # 箱状態：種類の変更
E_APP_BOX_STAT_FALL   = 3   # 箱状態：落下
E_APP_BOX_STAT_SERVE  = 4   # 箱状態：サーブ

# シーケンス
E_SEQ_INIT   = 0    # 初期状態
E_SEQ_IDLE   = 1    # 待機状態
E_SEQ_SLIDE  = 2    # スライド移動状態
E_SEQ_CHANGE = 3    # 種類の変更状態
E_SEQ_FALL   = 4    # 落下状態
E_SEQ_SERVE  = 5    # サーブ状態

# タイマー
E_TM_SLIDE  = 12    # スライド移動状態の処理時間
E_TM_CHANGE = 12    # 種類の変更状態の処理時間
E_TM_FALL   = 12    # 落下状態の処理時間
E_TM_SERVE  = 12    # サーブ移動状態の処理時間

# GUI配置情報(構造体定義)
@dataclass
class BoxGuiInf:
    txt: str                        # 文字列
    frame_line: tuple[int,int,int]  # フレームの線色
    frame_fill: tuple[int,int,int]  # フレームの塗潰し色
    txt_color: tuple[int,int,int]   # 文字色
    font_size: int                  # 文字サイズ

# GUI配置情報(テーブル)
F_APP_BOX_GUI_INF: list[BoxGuiInf] = [
    BoxGuiInf(        "1",(64,64,64),(128,128,128),(  0,  0,  0), 64),  # 2の 0乗
    BoxGuiInf(        "2",(64,64,64),( 64, 64,192),(  0,  0,  0), 64),  # 2の 1乗
    BoxGuiInf(        "4",(64,64,64),(192, 64, 64),(  0,  0,  0), 64),  # 2の 2乗
    BoxGuiInf(        "8",(64,64,64),(192, 64,192),(  0,  0,  0), 64),  # 2の 3乗
    BoxGuiInf(       "16",(64,64,64),( 64,192, 64),(  0,  0,  0), 48),  # 2の 4乗
    BoxGuiInf(       "32",(64,64,64),( 64,192,192),(  0,  0,  0), 48),  # 2の 5乗
    BoxGuiInf(       "64",(64,64,64),(192,192, 64),(  0,  0,  0), 48),  # 2の 6乗
    BoxGuiInf(      "128",(64,64,64),(192,192,192),(  0,  0,  0), 40),  # 2の 7乗
    BoxGuiInf(      "256",(64,64,64),(192,192,255),(  0,  0,  0), 40),  # 2の 8乗
    BoxGuiInf(      "512",(64,64,64),(255,192,192),(  0,  0,  0), 40),  # 2の 9乗
    BoxGuiInf(    "1,024",(64,64,64),(255,192,255),(  0,  0,  0), 32),  # 2の10乗
    BoxGuiInf(    "2,048",(64,64,64),(192,255,192),(  0,  0,  0), 32),  # 2の11乗
    BoxGuiInf(    "4,096",(64,64,64),(192,255,255),(  0,  0,  0), 32),  # 2の12乗
    BoxGuiInf(    "8,192",(64,64,64),(255,255,192),(  0,  0,  0), 32),  # 2の13乗
    BoxGuiInf(   "16,384",(64,64,64),(255,255,255),(  0,  0,  0), 28),  # 2の14乗
    BoxGuiInf(   "32,768",(64,64,64),( 32, 32, 32),(255,255,255), 28),  # 2の15乗
    BoxGuiInf(   "65,536",(64,64,64),( 32, 32,128),(255,255,255), 28),  # 2の16乗
    BoxGuiInf(  "131,072",(64,64,64),(128, 32, 32),(255,255,255), 22),  # 2の17乗
    BoxGuiInf(  "262,144",(64,64,64),(128, 32,128),(255,255,255), 22),  # 2の18乗
    BoxGuiInf(  "524,288",(64,64,64),( 32,128, 32),(255,255,255), 22),  # 2の19乗
    BoxGuiInf("1,048,576",(64,64,64),( 32,128,128),(255,255,255), 20),  # 2の20乗
]
E_APP_BOX_TYPE_NUM = len(F_APP_BOX_GUI_INF)

E_APP_BOX_GUI_OFFSET_X =   0    # 箱オブジェクト全体のオフセット位置X
E_APP_BOX_GUI_OFFSET_Y = 100    # 箱オブジェクト全体のオフセット位置Y

#
# Definition of Class
#
class AppBox(Routine):
    #----- Constructor(s)/Destructor
    def __init__(self, box_type_id: int, pos_x: int, pos_y: int):
        """コンストラクタ

        Args:
            box_type_id=箱タイプ
            pos_x=初期位置X
            pos_y=初期位置Y
        """
        super().__init__()

        #------------------------------------------------------------------------
        # 引数チェック
        if not numok(box_type_id, E_APP_BOX_TYPE_NUM):
            self.init_ok = False
            return

        #------------------------------------------------------------------------
        # 初期値設定
        self.box_type_id = box_type_id      # 箱タイプ設定
        self.pos_x = pos_x                  # 初期位置X
        self.pos_y = pos_y                  # 初期位置Y
        self.stat = E_APP_BOX_STAT_IDLE     # 初期箱状態=オフ
        self.gui_rect_id = -1               # 初期GUIの矩形ID=-1
        self.gui_txt_id = -1                # 初期GUIのテキストID=-1
        self.tm_seq = 0                     # シーケンスタイマー=0
        self.seq = E_SEQ_INIT               # 初期シーケンス=初期状態

        self.slide_from_x = self.pos_x      # スライド移動開始位置X
        self.slide_from_y = self.pos_y      # スライド移動開始位置Y
        self.slide_to_x = self.pos_x        # スライド移動終了位置X
        self.slide_to_y = self.pos_y        # スライド移動終了位置Y

        self.fall_from_x = self.pos_x       # 落下移動開始位置X
        self.fall_from_y = self.pos_y       # 落下移動開始位置Y
        self.fall_to_x = self.pos_x         # 落下移動終了位置X
        self.fall_to_y = self.pos_y         # 落下移動終了位置Y

        self.serve_from_x = self.pos_x      # サーブ移動開始位置X
        self.serve_from_y = self.pos_y      # サーブ移動開始位置Y
        self.serve_to_x = self.pos_x        # サーブ移動終了位置X
        self.serve_to_y = self.pos_y        # サーブ移動終了位置Y

        self.change_to = self.box_type_id   # 変更後箱種類

        # 初期化正常終了
        self.init_ok_mark()
        # この時点でGUI表示の初期設定をする
        self.update_gui()

    def destroy_gui(self):
        """C++のデストラクタに代わる解放処理

        Notes:
            pythonのデストラクタはGCが非同期で呼び出されるため、
            インスタンス解放時の処理はこれを明示的に呼び出して代用する。
        """
        #------------------------------------------------------------------------
        # GUIインスタンスを取得する
        mg = MidGui.get_inst()
        if not mg:
            return
        if self.gui_rect_id >= 0:
            # 描画する矩形を登録削除する
            mg.clr_rect(self.gui_rect_id)
            self.gui_rect_id = -1
        if self.gui_txt_id >= 0:
            # 描画するテキストを登録削除する
            mg.clr_txt(self.gui_txt_id)
            self.gui_txt_id = -1

    def get_box_type_id(self):
        """箱種類を取得

        Returns:
            箱種類
        """
        return self.box_type_id

    def set_pos(self, x: int, y: int):
        """位置を設定
        
        Args:
            位置X,Y
        """
        self.pos_x, self.pos_y = x, y

    def set_stat_slide(self, to_x: int, to_y: int):
        """箱状態をスライド移動に設定

        Args:
            スライド終了位置
        """
        # スライド移動開始位置 <- 現在位置
        self.slide_from_x, self.slide_from_y = self.pos_x, self.pos_y
        # スライド移動終了位置 <- 引数
        self.slide_to_x, self.slide_to_y = to_x, to_y
        
        self.stat = E_APP_BOX_STAT_SLIDE

    def set_stat_change(self, to_type: int):
        """箱状態を種類変更に設定

        Args:
            変更後箱タイプ
        """
        # 変更前箱タイプ <- 箱タイプ設定
        self.change_to = to_type
        # 変更後箱タイプ設定 <- 引数
        self.stat = E_APP_BOX_STAT_CHANGE

    def set_stat_fall(self, to_x: int, to_y: int):
        """箱状態を落下に設定

        Args:
            落下移動終了位置
        """
        # 落下移動開始位置 <- 現在位置
        self.fall_from_x, self.fall_from_y = self.pos_x, self.pos_y
        # 落下移動終了位置 <- 引数
        self.fall_to_x, self.fall_to_y = to_x, to_y
        
        self.stat = E_APP_BOX_STAT_FALL

    def set_stat_serve(self, to_x: int, to_y: int):
        """箱状態をサーブに設定

        Args:
            サーブ移動終了位置
        """
        # サーブ移動開始位置 <- 現在位置
        self.serve_from_x, self.serve_from_y = self.pos_x, self.pos_y
        # サーブ移動終了位置 <- 引数
        self.serve_to_x, self.serve_to_y = to_x, to_y
        
        self.stat = E_APP_BOX_STAT_SERVE

    def get_stat(self):
        """箱状態を取得

        Returns:
            箱状態
        """
        return self.stat

    def update_gui(self) -> bool:
        """GUI表示の更新処理

        Returns:
            False=更新失敗
        """
        #------------------------------------------------------------------------
        # GUIインスタンスを取得する
        mg = MidGui.get_inst()
        if not mg:
            return False

        #------------------------------------------------------------------------
        # GUI配置情報を特定する
        info = F_APP_BOX_GUI_INF[self.box_type_id]

        #------------------------------------------------------------------------
        # シーケンス分岐(場合によっては位置移動させる)
        if self.seq == E_SEQ_SLIDE:
            # シーケンス=スライド移動状態 -> 位置を変形する
            nrate = self.tm_seq * self.tm_seq
            x = self.pos_x - (self.pos_x - self.slide_from_x) * nrate // (E_TM_SLIDE * E_TM_SLIDE)
            y = self.pos_y - (self.pos_y - self.slide_from_y) * nrate // (E_TM_SLIDE * E_TM_SLIDE)
        elif self.seq == E_SEQ_FALL:
            # シーケンス=落下移動状態 -> 位置を変形する
            nrate = (E_TM_FALL * E_TM_FALL) - (E_TM_FALL - self.tm_seq) * (E_TM_FALL - self.tm_seq)
            x = self.pos_x - (self.pos_x - self.fall_from_x) * nrate // (E_TM_FALL * E_TM_FALL)
            y = self.pos_y - (self.pos_y - self.fall_from_y) * nrate // (E_TM_FALL * E_TM_FALL)
        elif self.seq == E_SEQ_SERVE:
            # シーケンス=サーブ移動状態 -> 位置を変形する
            nrate = self.tm_seq
            x = self.pos_x - (self.pos_x - self.serve_from_x) * nrate // E_TM_SERVE
            y = self.pos_y - (self.pos_y - self.serve_from_y) * nrate // E_TM_SERVE
        else:
            # シーケンス=その他 -> 変形なし
            x, y = self.pos_x, self.pos_y

        x += E_APP_BOX_GUI_OFFSET_X
        y += E_APP_BOX_GUI_OFFSET_Y

        #------------------------------------------------------------------------
        # フレーム表示
        # GUIの矩形ID未取得時は取得する
        if self.gui_rect_id < 0:
            self.gui_rect_id = mg.get_unset_rect_id(E_MIDGUI_RECT_BOX)
        # GUIのテキストID未取得時は取得する
        if self.gui_txt_id < 0:
            self.gui_txt_id = mg.get_unset_txt_id(E_MIDGUI_TXT_BOX)
        # フレーム矩形の設定と文字表示範囲の矩形の設定
        rect = (x, y, x+100, y+100)
        # GUIドライバに矩形の配置を指示する
        mg.set_rect(self.gui_rect_id, rect, info.frame_line, info.frame_fill, True)
        # GUIドライバに文字列の配置を指示する
        mg.set_txt(self.gui_txt_id, rect, info.txt_color, info.txt, info.font_size, False, True)

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
            # GUI表示の更新処理
            if not self.update_gui(): return False
            # シーケンス -> 待機状態
            self.seq = E_SEQ_IDLE
            return True

        #------------------------------------------------------------------------
        # シーケンス処理:待機状態
        if self.seq == E_SEQ_IDLE:
            # GUI表示の更新処理
            if not self.update_gui(): return False
            #--------------------------------------------------------------------
            # 箱状態がスライド移動の場合はスライド移動状態へ
            if self.stat == E_APP_BOX_STAT_SLIDE:
                # 位置 <- スライド移動終了位置
                self.pos_x, self.pos_y = self.slide_to_x, self.slide_to_y
                # シーケンス -> スライド移動状態
                self.seq = E_SEQ_SLIDE
                self.tm_seq = E_TM_SLIDE
            #--------------------------------------------------------------------
            # 箱状態が種類変更の場合は種類の変更状態へ
            if self.stat == E_APP_BOX_STAT_CHANGE:
                # 変更後箱タイプ設定 <- 引数
                self.box_type_id = self.change_to
                # シーケンス -> 種類の変更状態
                self.seq = E_SEQ_CHANGE
                self.tm_seq = E_TM_CHANGE
            #--------------------------------------------------------------------
            # 箱状態が落下の場合は落下状態へ
            if self.stat == E_APP_BOX_STAT_FALL:
                # 位置 <- 落下移動終了位置
                self.pos_x, self.pos_y = self.fall_to_x, self.fall_to_y
                # シーケンス -> 落下状態
                self.seq = E_SEQ_FALL
                self.tm_seq = E_TM_FALL
            #--------------------------------------------------------------------
            # 箱状態がサーブの場合はサーブ状態へ
            if self.stat == E_APP_BOX_STAT_SERVE:
                self.pos_x, self.pos_y = self.serve_to_x, self.serve_to_y
                self.seq = E_SEQ_SERVE
                self.tm_seq = E_TM_SERVE
            return True

        #------------------------------------------------------------------------
        # シーケンス処理:スライド移動状態
        if self.seq == E_SEQ_SLIDE:
            # GUI表示の更新処理
            if not self.update_gui(): return False
            #--------------------------------------------------------------------
            # タイマーカウント中は待機(スライド移動中)
            if self.tm_seq == 0:
                # タイマーカウント後
                self.destroy_gui()
                return False
            return True

        #------------------------------------------------------------------------
        # シーケンス処理:種類の変更状態
        # シーケンス処理:落下状態
        # シーケンス処理:サーブ状態
        if self.seq in (E_SEQ_CHANGE, E_SEQ_FALL, E_SEQ_SERVE):
            # GUI表示の更新処理
            if not self.update_gui(): return False
            # タイマーカウント中は待機
            # タイマーカウント後は待機状態へ
            if self.tm_seq == 0:
                # タイマーカウント後
                # 箱状態=アイドル
                self.stat = E_APP_BOX_STAT_IDLE
                # シーケンス -> 待機状態
                self.seq = E_SEQ_IDLE
            return True
        return True

