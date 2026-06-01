"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : routine.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 全ルーチン管理基底クラス
"""
#
# Import
#
from __future__ import annotations

#
# Definition of Class
#
class Routine:
    def __init__(self):
        """コンストラクタ
        """
        #------------------------------------------------------------------------
        # 初期値設定
        self.child_num = 0      # 子ルーチン数 <- 0(なし)
        self.children = None    # 子ルーチンへのポインタ <- None(子ルーチンなし)
        self.child_ret = None   # 子ルーチンからの戻り値 <- None(子ルーチンなし)

        self.return_value = 0   # 親ルーチンへの戻り値 <- 0
        self.init_ok = False    # 初期化正常終了フラグ <- 未

    def set_child_num(self, n: int) -> bool:
        """子ルーチン数の設定と初期化
        
        Args:
            子ルーチン数
        
        Returns:
            子ルーチン数設定と初期化の成否
        """
        if n < 0:
            return False
        self.child_num = n

        #------------------------------------------------------------------------
        # インスタンス生成
        self.children = [None] * n if n > 0 else None
        self.child_ret = [0] * n if n > 0 else None
        return True

    def init_ok_mark(self):
        """初期化正常終了フラグセット
        """
        self.init_ok = True

    def update(self) -> bool:
        """子ルーチンの更新処理

        Returns:
            False=処理終了（オブジェクト破棄）
        """
        # 例外処理：初期化失敗
        if not self.init_ok:
            return False
        # 子ルーチン無 -> 処理不要
        if not self.children:
            return True

        # 子ルーチンが登録されていたらUpdateを呼び出す
        # Updateの戻り値でFalseを返した子ルーチンは消去する
        for i, ch in enumerate(self.children):
            if ch is None:
                continue
            # 子ルーチンあり
            if not ch.update():
                # 処理終了 -> 戻り値更新後にインスタンス破棄
                self.child_ret[i] = getattr(ch, "return_value", 0)
                try:
                    ch.destroy()
                except Exception:
                    pass
                self.children[i] = None
            else:
                # 処理継続 -> 戻り値更新
                self.child_ret[i] = getattr(ch, "return_value", 0)
        return True

    def destroy(self):
        """
        C++のデストラクタの代替：
        自分 + 子 をすべて安全に破棄する
        """

        # ① 自分のGUIを消す（あれば）
        if hasattr(self, "destroy_gui"):
            try:
                self.destroy_gui()
            except Exception:
                pass

        # ② 子を再帰的に破棄
        if self.children:
            for i, ch in enumerate(self.children):
                if ch is not None:
                    if hasattr(ch, "destroy"):
                        try:
                            ch.destroy()
                        except Exception:
                            pass
