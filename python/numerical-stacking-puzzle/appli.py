"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : appli.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : アプリケーション層全体制御
"""
#
# Import
#
from __future__ import annotations

import time
from routine import Routine
from define import (
    dec_val, E_APP_SAVE_ID_KEY, E_CELL_X_NUM, E_CELL_Y_NUM, E_NEXT_BOX_NUM
)
from midkey import MidKey
from midsnd import MidSnd, E_MIDSND_STACK, E_MIDSND_MERGE_1, E_MIDSND_MERGE_2, E_MIDSND_MERGE_3
from celldat import CellDat
from nextdat import NextDat
from appbox import AppBox
from appboxset import AppBoxSet
from appnextboxset import AppNextBoxSet
from appkeyindset import AppKeyIndSet
from applblset import AppLblSet
from titlelblset import TitleLblSet
from rec import Rec
from define import E_APP_SAVE_ID_NUM
from PySide6.QtWidgets import QFileDialog

#
# Definition
#

# シーケンス
E_SEQ_INIT       = 0    # 初期状態
E_SEQ_TITLE      = 1    # タイトル状態
E_SEQ_SERVE      = 2    # サーブ状態
E_SEQ_FALL_SLOW  = 3    # 落下(遅)状態
E_SEQ_FALL_FAST  = 4    # 落下(速)状態
E_SEQ_STACK      = 5    # 積上状態
E_SEQ_MERGE      = 6    # 結合動作状態
E_SEQ_LAND       = 7    # 着地動作状態
E_SEQ_BACK       = 8    # タイトルバック状態
E_SEQ_EXIT       = 9    # EXIT状態

# 子プロセス
E_RT_TITLE_LBL_SET    = 0   # タイトルラベルセット
E_RT_CELL_DAT         = 1   # セルのデータ管理
E_RT_NEXT_DAT         = 2   # 次落下セルのデータ管理
E_RT_APP_KEY_IND_SET  = 3   # キー表示オブジェクト全体管理
E_RT_APP_LBL_SET      = 4   # ラベルオブジェクト全体管理
E_RT_APP_BOX_SET      = 5   # 箱オブジェクト全体管理
E_RT_APP_NEXT_BOX_SET = 6   # 次回箱オブジェクト全体管理
E_RT_FALL_APP_BOX     = 7   # 落下箱
E_RT_NUM              = 8   # 子プロセスの上限数

# タイマー
E_TM_SERVE         = 15
E_TM_FALL_SLOW_INI = 60
E_TM_FALL_FAST     = 5
E_TM_STACK         = 10
E_TM_MERGE         = 30
E_TM_LAND          = 30

#
# Definition of Class
#
class Appli(Routine):
    def __init__(self):
        """コンストラクタ
        """
        super().__init__()
        #------------------------------------------------------------------------
        # 初期化処理
        # 子プロセス
        self.set_child_num(E_RT_NUM)

        #------------------------------------------------------------------------
        # 初期値設定
        self.seq = E_SEQ_INIT   # 初期シーケンス=初期状態
        self.tm_seq = 0         # シーケンスタイマー <- 0
        self.elapsed = -1       # 経過時間 <- -1
        self.title_back = False # タイトルバック指令
        self.exit_cmd = False   # EXIT指令

        #------------------------------------------------------------------------
        # バッファクリア
        self.merge_time = [-1] * 5
        self.merge_val  = [-1] * 5

        # falling box
        self.fall_box_x = 0
        self.fall_box_y = 0
        self.fall_box_val = 0
        self.tm_fall = E_TM_FALL_SLOW_INI

        #------------------------------------------------------------------------
        # 初期化正常終了
        self.init_ok_mark()

    def set_title_back(self):
        """タイトルバック指令                                               *
        """
        self.title_back = True

    def set_exit(self):
        """EXIT指令
        """
        self.exit_cmd = True

    def is_exit_seq(self) -> bool:
        """シーケンス状態がEXIT状態か否か取得
        """
        return self.seq == E_SEQ_EXIT

    def create_pzl_child(self) -> bool:
        """パズル系の子プロセスの生成

        returns:
            False=生成失敗
        """
        # セルデータ管理ルーチン起動
        self.children[E_RT_CELL_DAT] = CellDat()
        if self.children[E_RT_CELL_DAT] is None:
            return False
        # セルデータ管理ルーチン起動
        self.children[E_RT_NEXT_DAT] = NextDat()
        if self.children[E_RT_NEXT_DAT] is None:
            return False
        # セルデータ管理ルーチン起動
        self.children[E_RT_APP_BOX_SET] = AppBoxSet(CellDat.get_inst())
        if self.children[E_RT_APP_BOX_SET] is None:
            return False
        # 次回箱オブジェクト全体管理の生成
        self.children[E_RT_APP_NEXT_BOX_SET] = AppNextBoxSet()
        if self.children[E_RT_APP_NEXT_BOX_SET] is None:
            return False
        # キー表示オブジェクト全体管理の生成
        self.children[E_RT_APP_KEY_IND_SET] = AppKeyIndSet()
        if self.children[E_RT_APP_KEY_IND_SET] is None:
            return False
        # ラベル全体オブジェクトの生成
        self.children[E_RT_APP_LBL_SET] = AppLblSet()
        if self.children[E_RT_APP_LBL_SET] is None:
            return False

        return True

    def is_pzl_child_alive(self) -> bool:
        """子プロセスの生存判定
        
        Returns:
            すべて生存
        """
        must = (
            E_RT_CELL_DAT,
            E_RT_NEXT_DAT,
            E_RT_APP_BOX_SET,
            E_RT_APP_NEXT_BOX_SET,
            E_RT_APP_KEY_IND_SET,
            E_RT_APP_LBL_SET,
        )
        for idx in must:
            if self.children[idx] is None:
                return False
        return True

    def delete_pzl_child(self):
        """パズル系の子プロセスの破棄
        """
        # 落下箱を消す
        fb = self.children[E_RT_FALL_APP_BOX]
        if fb is not None:
            try:
                fb.destroy()
            except Exception:
                pass
        self.children[E_RT_FALL_APP_BOX] = None

        # ほかを消す（Pythonでは参照解除）
        for idx in (E_RT_CELL_DAT, E_RT_NEXT_DAT, E_RT_APP_BOX_SET, E_RT_APP_NEXT_BOX_SET, E_RT_APP_KEY_IND_SET, E_RT_APP_LBL_SET):
            ch = self.children[idx]
            if ch is not None:
                try:
                    ch.destroy()
                except Exception:
                    pass
            self.children[idx] = None

    def init_pzl_data(self) -> bool:
        """パズル系のデータの初期設定

        Returns:
            False=設定失敗
        """
        # 次落下箱管理インスタンスを取得する
        # (パズル系の子プロセスの生成時に生成される)
        nd = NextDat.get_inst()
        # ラベル全体オブジェクト
        # (パズル系の子プロセスの生成時に生成される)
        lbl = AppLblSet.get_inst()
        if nd is None or lbl is None:
            return False

        # スコア表示のリセット
        lbl.set_score_val(0)
        # 経過時間のリセット
        self.elapsed = 0
        lbl.set_elapsed_time_val(self.elapsed)
        # 落下開始位置の初期値を設定
        self.fall_box_x = 0
        self.fall_box_y = 0
        # 落下箱の値を設定
        self.fall_box_val = nd.exec_push()
        # 次回箱の表示更新
        self.tm_fall = E_TM_FALL_SLOW_INI

        return True

    def update_elapsed_time(self) -> bool:
        """経過時間の更新処理

        Return:
            False=更新失敗
        """
        if self.elapsed == 0:
            if self.seq < E_SEQ_FALL_SLOW:
                pass
            elif self.seq == E_SEQ_FALL_SLOW:
                if self.fall_box_y == 0:
                    pass
                else:
                    self.elapsed = 1
            else:
                self.elapsed = 1
        else:
            self.elapsed += 1

        lbl = AppLblSet.get_inst()
        if lbl:
            lbl.set_elapsed_time_val(self.elapsed)
        return True

    def update_merge_time(self, max_merge_val: int) -> bool:
        """マージ時間の更新処理

        Return:
            False=更新失敗
        """
        if max_merge_val >= 0:
            # 既に同じ値があるなら何もしない
            if max_merge_val not in self.merge_val:
                max_merge_time = self.elapsed
                # 値が大きい順に挿入（C++のSwapループを忠実再現）
                for i in range(5):
                    # 同じ値は無い -> 更新を行う
                    if max_merge_val < self.merge_val[i]:
                        continue
                    # swap（大きい値ほど前に来るように入れ替え）
                    max_merge_val, self.merge_val[i] = self.merge_val[i], max_merge_val
                    max_merge_time, self.merge_time[i] = self.merge_time[i], max_merge_time

        lbl = AppLblSet.get_inst()
        if lbl:
            for i in range(5):
                lbl.set_merge_time_val(i, self.merge_time[i], self.merge_val[i])
        return True

    def open_load_file(self):
        """読出ファイルの選択処理(Dialog)

        Return:
            False=選択キャンセル
        """
        filename, _ = QFileDialog.getOpenFileName(
            None,
            "ロードファイルを選択",
            "",
            "Save Files (*.txt *.csv);;All Files (*.*)"
        )

        if filename == "":
            return None

        return filename

    def save_file_op(self) -> bool:
        """ファイル保存オペレーション

        Return:
            False=保存失敗
        """
        #------------------------------------------------------------------------
        # セルデータ管理インスタンスを取得する
        cd = CellDat.get_inst()

        #------------------------------------------------------------------------
        # 次落下箱管理インスタンスを取得する
        nd = NextDat.get_inst()

        #------------------------------------------------------------------------
        # パズル系データの保存
        rec = Rec()
        # 識別情報
        rec.set_dat(E_APP_SAVE_ID_KEY, [E_APP_SAVE_ID_NUM])
        # セルデータ
        cell = []
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                c = cd.cell[x][y]
                cell.append(c.nVal)
                cell.append(c.nStackVal)
                cell.append(c.nMergeVal)
                cell.append(c.nMergeX)
                cell.append(c.nMergeY)
                cell.append(c.nLandY)
                cell.append(c.nLandVal)
        rec.set_dat("CellData", cell)
        # スコア
        rec.set_dat("Score", [cd.get_score()])
        # 次回箱データ
        rec.set_dat("NextData", [nd.get_val(i) for i in range(E_NEXT_BOX_NUM)])
        # 次回箱生成数
        rec.set_dat("NextBoxCnt", [nd.next_box_cnt])
        # 箱の値ランダムタイマ
        rec.set_dat("BoxValTime", [nd.tm_box_val])
        # 落下中の箱
        rec.set_dat("FallX", [self.fall_box_x])
        rec.set_dat("FallY", [self.fall_box_y])
        rec.set_dat("FallVal", [self.fall_box_val])
        # 経過時間
        rec.set_dat("ElapsedTime", [self.elapsed])
        # マージ時間とマージ値
        rec.set_dat("MergeTime", self.merge_time)
        rec.set_dat("MergeVal", self.merge_val)

        #------------------------------------------------------------------------
        # 現在時刻からログファイル名を決定
        filename = time.strftime("save-%Y%m%d-%H%M%S.txt", time.localtime())

        return rec.save_file(filename)

    def load_file_op(self, filename: str) -> bool:
        """ファイル読出オペレーション

        Return:
            False=読出失敗
        """
        #------------------------------------------------------------------------
        # セルデータ管理インスタンスを取得する
        cd = CellDat.get_inst()
        # 次落下箱管理インスタンスを取得する
        nd = NextDat.get_inst()
        # ラベル全体オブジェクト
        # (パズル系の子プロセスの生成時に生成される)
        lbl = AppLblSet.get_inst()

        #------------------------------------------------------------------------
        # ファイル読み出し
        rec = Rec()
        if not rec.load_file(filename):
            return False

        #------------------------------------------------------------------------
        # パズル系データの読出
        # 識別情報
        if rec.get_dat(E_APP_SAVE_ID_KEY, 1)[0] != E_APP_SAVE_ID_NUM:
            return False
        # 識別情報
        cell = rec.get_dat("CellData", E_CELL_X_NUM * E_CELL_Y_NUM * 7)
        idx = 0
        for x in range(E_CELL_X_NUM):
            for y in range(E_CELL_Y_NUM):
                c = cd.cell[x][y]
                c.nVal       = cell[idx]; idx+=1
                c.nStackVal  = cell[idx]; idx+=1
                c.nMergeVal  = cell[idx]; idx+=1
                c.nMergeX    = cell[idx]; idx+=1
                c.nMergeY    = cell[idx]; idx+=1
                c.nLandY     = cell[idx]; idx+=1
                c.nLandVal   = cell[idx]; idx+=1
        # スコア
        score = rec.get_dat("Score", 1)[0]
        cd.set_score(score)
        # 表示にも反映させる
        lbl.set_score_val(score)
        # 次回箱データ
        nd.next_vals = rec.get_dat("NextData", E_NEXT_BOX_NUM)
        # 次回箱生成数
        nd.next_box_cnt = rec.get_dat("NextBoxCnt", 1)[0]
        # 箱の値ランダムタイマ
        nd.tm_box_val = rec.get_dat("BoxValTime", 1)[0]
        # 落下中の箱
        self.fall_box_x = rec.get_dat("FallX", 1)[0]
        self.fall_box_y = rec.get_dat("FallY", 1)[0]
        self.fall_box_val = rec.get_dat("FallVal", 1)[0]
        # 経過時間
        self.elapsed = rec.get_dat("ElapsedTime", 1)[0]
        # 経過時間の更新処理
        # 表示にも反映させる
        lbl.set_elapsed_time_val(self.elapsed)
        # マージ時間とマージ値
        self.merge_time = rec.get_dat("MergeTime", 5)
        self.merge_val = rec.get_dat("MergeVal", 5)
        # マージ時間を更新する
        for i in range(5):
            lbl.set_merge_time_val(i, self.merge_time[i], self.merge_val[i])

        return True

    def seq_init(self) -> bool:
        """シーケンス処理:初期状態

        Return:
            False=処理終了（オブジェクト破棄）
        """
        # タイトル用ラベルオブジェクト全体管理の生成
        self.children[E_RT_TITLE_LBL_SET] = TitleLblSet()
        if self.children[E_RT_TITLE_LBL_SET] is None:
            return False

        #------------------------------------------------------------------------
        # EXIT指令ある場合は終了
        if self.exit_cmd:
            self.exit_cmd = False
            self.seq = E_SEQ_EXIT
            # 以降の処理なし
            return True

        # シーケンス -> タイトル状態
        self.seq = E_SEQ_TITLE
        return True

    # ----------------------------
    # シーケンス：TITLE
    # ----------------------------
    def seq_title(self) -> bool:
        mk = MidKey.get_inst()
        if mk is None:
            return False

        # タイトル用ラベルが死んでいたら強制終了（C++の枝葉生存判定に合わせる）
        if self.children[E_RT_TITLE_LBL_SET] is None:
            return False

        # 音インスタンス取得（C++と同じ）
        snd = MidSnd.get_inst()
        if snd is None:
            return False

        # ESCは終了
        if mk.get_key_down().esc:
            self.exit_cmd = True

        if self.exit_cmd:
            self.exit_cmd = False
            self.seq = E_SEQ_EXIT
            return True

        # キー入力を判定（C++の bStart / else-if 構造を忠実に）
        kd = mk.get_key_down().qwerty
        start = False
        load_start = False

        if kd[0]:
            # [Z] New Game
            start = True

        elif kd[1]:
            # X = ロード
            filename = self.open_load_file()
            if filename is not None:
                start = True
                load_start = True

        elif kd[2]:
            # [C] Sound OFF
            snd.set_disable_play(True)
        elif kd[3]:
            # [V] Sound ON + 確認音
            snd.set_disable_play(False)
            snd.set_snd(E_MIDSND_MERGE_3)

        if start:
            # タイトルラベル破棄
            tset = self.children[E_RT_TITLE_LBL_SET]
            if tset is not None:
                try:
                    tset.destroy()
                except Exception:
                    pass
            self.children[E_RT_TITLE_LBL_SET] = None

            # パズル系子生成
            if not self.create_pzl_child():
                return False

            # データ初期化
            if not self.init_pzl_data():
                return False

            # ★ここでロード
            if load_start:
                if not self.load_file_op(filename):
                    start = False

            if start:
                # 次箱表示更新
                nbs = AppNextBoxSet.get_inst()
                if nbs:
                    nbs.set_stat_push()

                # 落下箱生成（右から出てサーブ位置へ）
                self.children[E_RT_FALL_APP_BOX] = AppBox(self.fall_box_val, 1200, 0)
                if self.children[E_RT_FALL_APP_BOX] is None:
                    return False
                self.children[E_RT_FALL_APP_BOX].set_stat_serve(self.fall_box_x * 100, self.fall_box_y * 100)

                self.title_back = False
                self.tm_seq = E_TM_SERVE
                self.seq = E_SEQ_SERVE

        return True

    # ----------------------------
    # シーケンス：SERVE
    # ----------------------------
    def seq_serve(self) -> bool:
        mk = MidKey.get_inst()
        if mk is None:
            return False

        if not self.is_pzl_child_alive():
            return False

        if not self.update_elapsed_time():
            return False

        # ESC はタイトルバック
        if mk.get_key_down().esc:
            self.title_back = True

        if self.title_back or self.exit_cmd:
            self.title_back = False
            self.seq = E_SEQ_BACK
            return True

        # 一定時間後に落下開始
        if self.tm_seq == 0:
            self.tm_seq = self.tm_fall
            self.seq = E_SEQ_FALL_SLOW

        return True

    # ----------------------------
    # シーケンス：FALL_SLOW
    # ----------------------------
    def seq_fall_slow(self) -> bool:
        cd = CellDat.get_inst()
        mk = MidKey.get_inst()
        snd = MidSnd.get_inst()
        if cd is None or mk is None or snd is None:
            return False
        if not self.is_pzl_child_alive():
            return False

        if not self.update_elapsed_time():
            return False

        if mk.get_key_down().esc:
            self.title_back = True
        if self.title_back or self.exit_cmd:
            self.title_back = False
            self.seq = E_SEQ_BACK
            return True

        # 箱の降下（補間）
        pos_x = self.fall_box_x * 100
        if self.fall_box_y == 0:
            pos_y = 0
        else:
            pos_y = self.fall_box_y * 100 - (self.tm_seq * 100) // self.tm_fall

        fb = self.children[E_RT_FALL_APP_BOX]
        if fb:
            fb.set_pos(pos_x, pos_y)

        # 1セルごとにスタック判定
        if self.tm_seq == 0:
            if cd.check_en_stack(self.fall_box_x, self.fall_box_y, self.fall_box_val):
                snd.set_snd(E_MIDSND_STACK)
                self.children[E_RT_APP_BOX_SET].set_stat_stack()
                self.tm_seq = E_TM_STACK
                self.seq = E_SEQ_STACK
            else:
                # スタック不可
                if self.fall_box_y == 0 and cd.get_val(self.fall_box_x, self.fall_box_y + 1) >= 0:
                    pass
                else:
                    self.fall_box_y += 1
                    self.tm_seq = self.tm_fall

        # キー操作（空セルへ移動したらFASTへ）
        if self.seq != E_SEQ_STACK:
            for x in range(E_CELL_X_NUM):
                if mk.get_key_down().qwerty[x]:
                    if cd.get_val(x, self.fall_box_y) < 0:
                        self.fall_box_x = x
                        # C++: (tm_seq*FAST + tm_fall/2) / tm_fall
                        self.tm_seq = (self.tm_seq * E_TM_FALL_FAST + (self.tm_fall // 2)) // self.tm_fall
                        self.seq = E_SEQ_FALL_FAST
                        break

        return True

    # ----------------------------
    # シーケンス：FALL_FAST
    # ----------------------------
    def seq_fall_fast(self) -> bool:
        cd = CellDat.get_inst()
        mk = MidKey.get_inst()
        snd = MidSnd.get_inst()
        if cd is None or mk is None or snd is None:
            return False
        if not self.is_pzl_child_alive():
            return False

        if not self.update_elapsed_time():
            return False

        if mk.get_key_down().esc:
            self.title_back = True
        if self.title_back or self.exit_cmd:
            self.title_back = False
            self.seq = E_SEQ_BACK
            return True

        pos_x = self.fall_box_x * 100
        if self.fall_box_y == 0:
            pos_y = 0
        else:
            pos_y = self.fall_box_y * 100 - (self.tm_seq * 100) // E_TM_FALL_FAST

        fb = self.children[E_RT_FALL_APP_BOX]
        if fb:
            fb.set_pos(pos_x, pos_y)

        if self.tm_seq == 0:
            if cd.check_en_stack(self.fall_box_x, self.fall_box_y, self.fall_box_val):
                snd.set_snd(E_MIDSND_STACK)
                self.children[E_RT_APP_BOX_SET].set_stat_stack()
                self.tm_seq = E_TM_STACK
                self.seq = E_SEQ_STACK
            else:
                if self.fall_box_y == 0 and cd.get_val(self.fall_box_x, self.fall_box_y + 1) >= 0:
                    pass
                else:
                    self.fall_box_y += 1
                    self.tm_seq = E_TM_FALL_FAST

        # 横移動（FAST中は状態維持）
        if self.seq != E_SEQ_STACK:
            for x in range(E_CELL_X_NUM):
                if mk.get_key_down().qwerty[x]:
                    if cd.get_val(x, self.fall_box_y) < 0:
                        self.fall_box_x = x
                        break

        return True

    # ----------------------------
    # シーケンス：STACK
    # ----------------------------
    def seq_stack(self) -> bool:
        cd = CellDat.get_inst()
        nd = NextDat.get_inst()
        mk = MidKey.get_inst()
        snd = MidSnd.get_inst()
        if cd is None or nd is None or mk is None or snd is None:
            return False
        if not self.is_pzl_child_alive():
            return False

        if not self.update_elapsed_time():
            return False

        if mk.get_key_down().esc:
            self.title_back = True

        if self.tm_seq == 0:
            # 格納値積み上げ
            cd.exec_stack()

            # 得点
            if cd.calc_score():
                AppLblSet.get_inst().set_score_val(cd.get_score())

            # 落下箱破棄
            fb = self.children[E_RT_FALL_APP_BOX]
            if fb:
                try:
                    fb.destroy()
                except Exception:
                    pass
            self.children[E_RT_FALL_APP_BOX] = None

            # マージ判定
            ok, maxv, difv = cd.check_merge(self.fall_box_x, self.fall_box_y)
            if ok:
                if not self.update_merge_time(maxv):
                    return False

                if difv <= 1:
                    snd.set_snd(E_MIDSND_MERGE_1)
                elif difv == 2:
                    snd.set_snd(E_MIDSND_MERGE_2)
                else:
                    snd.set_snd(E_MIDSND_MERGE_3)

                self.children[E_RT_APP_BOX_SET].set_stat_merge()
                self.tm_seq = E_TM_MERGE
                self.seq = E_SEQ_MERGE
            else:
                # 次の落下箱
                self.fall_box_y = 0
                self.fall_box_val = nd.exec_push()

                nbs = AppNextBoxSet.get_inst()
                if nbs:
                    nbs.set_stat_push()

                self.children[E_RT_FALL_APP_BOX] = AppBox(self.fall_box_val, 1200, 0)
                self.children[E_RT_FALL_APP_BOX].set_stat_serve(self.fall_box_x*100, self.fall_box_y*100)

                self.tm_seq = E_TM_SERVE
                self.seq = E_SEQ_SERVE

        return True

    # ----------------------------
    # シーケンス：MERGE
    # ----------------------------
    def seq_merge(self) -> bool:
        cd = CellDat.get_inst()
        nd = NextDat.get_inst()
        mk = MidKey.get_inst()
        snd = MidSnd.get_inst()
        if cd is None or nd is None or mk is None or snd is None:
            return False
        if not self.is_pzl_child_alive():
            return False

        if not self.update_elapsed_time():
            return False

        if mk.get_key_down().esc:
            self.title_back = True

        if self.tm_seq == 0:
            cd.exec_merge()
            if cd.calc_score():
                AppLblSet.get_inst().set_score_val(cd.get_score())

            # 着地判定
            if cd.check_en_land():
                self.children[E_RT_APP_BOX_SET].set_stat_land()
                self.tm_seq = E_TM_LAND
                self.seq = E_SEQ_LAND
            else:
                ok, maxv, difv = cd.check_merge(self.fall_box_x, self.fall_box_y)
                if ok:
                    if not self.update_merge_time(maxv):
                        return False
                    if difv <= 1:
                        snd.set_snd(E_MIDSND_MERGE_1)
                    elif difv == 2:
                        snd.set_snd(E_MIDSND_MERGE_2)
                    else:
                        snd.set_snd(E_MIDSND_MERGE_3)

                    self.children[E_RT_APP_BOX_SET].set_stat_merge()
                    self.tm_seq = E_TM_MERGE
                    self.seq = E_SEQ_MERGE
                else:
                    # 次の落下箱
                    self.fall_box_y = 0
                    self.fall_box_val = nd.exec_push()

                    nbs = AppNextBoxSet.get_inst()
                    if nbs:
                        nbs.set_stat_push()

                    self.children[E_RT_FALL_APP_BOX] = AppBox(self.fall_box_val, 1200, 0)
                    self.children[E_RT_FALL_APP_BOX].set_stat_serve(self.fall_box_x*100, self.fall_box_y*100)

                    self.tm_seq = E_TM_SERVE
                    self.seq = E_SEQ_SERVE

        return True

    # ----------------------------
    # シーケンス：LAND
    # ----------------------------
    def seq_land(self) -> bool:
        cd = CellDat.get_inst()
        nd = NextDat.get_inst()
        mk = MidKey.get_inst()
        snd = MidSnd.get_inst()
        if cd is None or nd is None or mk is None or snd is None:
            return False
        if not self.is_pzl_child_alive():
            return False

        if not self.update_elapsed_time():
            return False

        if mk.get_key_down().esc:
            self.title_back = True

        if self.tm_seq == 0:
            cd.exec_land()
            if cd.calc_score():
                AppLblSet.get_inst().set_score_val(cd.get_score())

            ok, maxv, difv = cd.check_merge(self.fall_box_x, self.fall_box_y)
            if ok:
                if not self.update_merge_time(maxv):
                    return False
                if difv <= 1:
                    snd.set_snd(E_MIDSND_MERGE_1)
                elif difv == 2:
                    snd.set_snd(E_MIDSND_MERGE_2)
                else:
                    snd.set_snd(E_MIDSND_MERGE_3)
                self.children[E_RT_APP_BOX_SET].set_stat_merge()
                self.tm_seq = E_TM_MERGE
                self.seq = E_SEQ_MERGE
            else:
                self.fall_box_y = 0
                self.fall_box_val = nd.exec_push()

                nbs = AppNextBoxSet.get_inst()
                if nbs:
                    nbs.set_stat_push()

                self.children[E_RT_FALL_APP_BOX] = AppBox(self.fall_box_val, 1200, 0)
                self.children[E_RT_FALL_APP_BOX].set_stat_serve(self.fall_box_x*100, self.fall_box_y*100)

                self.tm_seq = E_TM_SERVE
                self.seq = E_SEQ_SERVE

        return True

    # ----------------------------
    # シーケンス：BACK（タイトルバック）
    # ----------------------------
    def seq_back(self) -> bool:
        mk = MidKey.get_inst()
        if mk is None:
            return False
        if not self.is_pzl_child_alive():
            return False

        # セーブ実行
        self.save_file_op()

        self.delete_pzl_child()

        # ESCは終了
        if mk.get_key_down().esc:
            self.exit_cmd = True

        if self.exit_cmd:
            self.exit_cmd = False
            self.seq = E_SEQ_EXIT
            return True

        # タイトルへ戻す
        self.children[E_RT_TITLE_LBL_SET] = TitleLblSet()
        if self.children[E_RT_TITLE_LBL_SET] is None:
            return False
        self.seq = E_SEQ_TITLE
        return True

    def seq_exit(self) -> bool:
        return True

    # ----------------------------
    # Update（C++と同じ）
    # ----------------------------
    def update(self) -> bool:
        if not super().update():
            return False

        self.tm_seq = dec_val(self.tm_seq, 1)

        if self.seq == E_SEQ_INIT:      return self.seq_init()
        if self.seq == E_SEQ_TITLE:     return self.seq_title()
        if self.seq == E_SEQ_SERVE:     return self.seq_serve()
        if self.seq == E_SEQ_FALL_SLOW: return self.seq_fall_slow()
        if self.seq == E_SEQ_FALL_FAST: return self.seq_fall_fast()
        if self.seq == E_SEQ_STACK:     return self.seq_stack()
        if self.seq == E_SEQ_MERGE:     return self.seq_merge()
        if self.seq == E_SEQ_LAND:      return self.seq_land()
        if self.seq == E_SEQ_BACK:      return self.seq_back()
        if self.seq == E_SEQ_EXIT:      return self.seq_exit()
        return True
