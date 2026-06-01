#
# Import
#
import sys
from PySide6.QtGui import QFont
from PySide6.QtWidgets import QApplication
from main_window import MainWindow

def main():
    app = QApplication(sys.argv)
    app.setQuitOnLastWindowClosed(True)
    font = QFont("Noto Sans Mono CJK JP", 10)
    app.setFont(font)
    w = MainWindow()
    w.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
