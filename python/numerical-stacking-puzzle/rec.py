"""
Copyright 2026 Miyabi

Project Name  : Numerical Stacking Puzzle (Python)
File Name     : rec.py
Author        : Miyabi
Create        : 2026/06/01
Purpose       : 記録データ読出保存ライブラリ
"""

#
# Definition of Class
#
class Rec:
    def __init__(self):
        """コンストラクタ
        """
        self.data = {}  # key -> list[int]

    def set_dat(self, key: str, dat: list[int]):
        """保存データをセット

        Args:
            情報キー、情報データ(&情報サイズ)
        """
        self.data[key] = list(dat)

    def get_dat(self, key: str, size: int):
        """保存データを取得
        
        Args:
            情報キー、情報サイズ
        
        Returns:
            保存データの取得処理の結果(OK/NG)
            情報データ(ポインタ引数返し)
        """
        if key not in self.data:
            return None
        if len(self.data[key]) != size:
            return None
        return self.data[key]

    def save_file(self, filename: str) -> bool:
        """保存データの書き込み

        Args:
            保存ファイル名

        Returns:
            保存処理の結果(OK/NG)
        """
        try:
            # 保存ファイルオープン
            with open(filename, "w", encoding="utf-8") as f:
                # データを結合する。データサイズ分繰り返す
                for key, dat in self.data.items():
                    f.write(f"key:{key}\n")
                    f.write(f"size:{len(dat)}\n")

                    for i, v in enumerate(dat):
                        f.write(str(v))
                        f.write(",")

                        if (i & 0x07) == 0x07:
                            f.write("\n")
                    f.write("\n")

                f.write("end:\n")
            # 保存ファイル書き込み、クローズ(自動)
        except Exception:
            return False
        return True

    def load_file(self, filename: str) -> bool:
        """保存データの読み出し

        Args:
            読出ファイル名

        Returns:
            読出処理の結果(OK/NG)
        """
        try:
            # 読出ファイルオープン
            with open(filename, "r", encoding="utf-8") as f:
                lines = f.readlines()
        except Exception:
            return False
        # データを抽出する。データサイズ分繰り返す
        self.data = {}
        i = 0
        while i < len(lines):
            # カンマに続く数値を抽出してデータにセット
            line = lines[i].strip()
            if line == "end:":
                break
            if line.startswith("key:"):
                key = line[4:]

                i += 1
                size = int(lines[i].strip()[5:])  # size:

                vals = []
                i += 1

                while len(vals) < size and i < len(lines):
                    parts = lines[i].strip().split(",")
                    for p in parts:
                        if p == "":
                            continue
                        vals.append(int(p))
                        if len(vals) >= size:
                            break
                    i += 1

                self.data[key] = vals
            else:
                i += 1

        return True
