#
# Import
#
from PySide6.QtWidgets import QWidget, QApplication
from PySide6.QtCore import QTimer, Qt
from PySide6.QtGui import QPainter

from mainfrm import MainFrm

#
# Definition of Class
#
class MainWindow(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Numerical Stacking Puzzle (Python)")
        self.resize(1200, 675)
        self.setMouseTracking(True)
        self.setFocusPolicy(Qt.StrongFocus)

        self.mainfrm = MainFrm(self)

        self._timer = QTimer(self)
        self._timer.timeout.connect(self._loop)
        self._timer.start(0)

    def _loop(self):
        ok = self.mainfrm.update()
        if not ok:
            self._timer.stop()
            QApplication.quit()
            return
        self.update()
        # ? KeyDownはupdateで生成するのでここでのクリアは不要
        # self.mainfrm.end_of_frame()  ← 削除！

    def paintEvent(self, ev):
        p = QPainter(self)
        self.mainfrm.draw(p)

    def resizeEvent(self, ev):
        self.mainfrm.on_resize(self.width(), self.height())
        super().resizeEvent(ev)

    def mouseMoveEvent(self, ev):
        self.mainfrm.set_mouse_pos(ev.position().x(), ev.position().y(), self.width(), self.height())
        super().mouseMoveEvent(ev)

    def mousePressEvent(self, ev):
        if ev.button() == Qt.LeftButton:
            self.grabMouse()
            self.mainfrm.set_mouse_l(True)
        super().mousePressEvent(ev)

    def mouseReleaseEvent(self, ev):
        if ev.button() == Qt.LeftButton:
            self.releaseMouse()
            self.mainfrm.set_mouse_l(False)
        super().mouseReleaseEvent(ev)

    def keyPressEvent(self, ev):
        if ev.isAutoRepeat():
            return
        k = ev.key()
        if k == Qt.Key_Escape:
            self.mainfrm.key_press("ESC")
        elif k == Qt.Key_Z:
            self.mainfrm.key_press("Z")
        elif k == Qt.Key_X:
            self.mainfrm.key_press("X")
        elif k == Qt.Key_C:
            self.mainfrm.key_press("C")
        elif k == Qt.Key_V:
            self.mainfrm.key_press("V")
        elif k == Qt.Key_B:
            self.mainfrm.key_press("B")
        elif k == Qt.Key_N:
            self.mainfrm.key_press("N")
        elif k == Qt.Key_M:
            self.mainfrm.key_press("M")
        elif k == Qt.Key_Comma:
            self.mainfrm.key_press("COMMA")
        elif k == Qt.Key_Period:
            self.mainfrm.key_press("PERIOD")
        elif k == Qt.Key_Slash:
            self.mainfrm.key_press("SLASH")
        elif k == Qt.Key_Backslash:
            self.mainfrm.key_press("BACKSLASH")
        super().keyPressEvent(ev)

    def keyReleaseEvent(self, ev):
        if ev.isAutoRepeat():
            return
        k = ev.key()
        if k == Qt.Key_Escape:
            self.mainfrm.key_release("ESC")
        elif k == Qt.Key_Z:
            self.mainfrm.key_release("Z")
        elif k == Qt.Key_X:
            self.mainfrm.key_release("X")
        elif k == Qt.Key_C:
            self.mainfrm.key_release("C")
        elif k == Qt.Key_V:
            self.mainfrm.key_release("V")
        elif k == Qt.Key_B:
            self.mainfrm.key_release("B")
        elif k == Qt.Key_N:
            self.mainfrm.key_release("N")
        elif k == Qt.Key_M:
            self.mainfrm.key_release("M")
        elif k == Qt.Key_Comma:
            self.mainfrm.key_release("COMMA")
        elif k == Qt.Key_Period:
            self.mainfrm.key_release("PERIOD")
        elif k == Qt.Key_Slash:
            self.mainfrm.key_release("SLASH")
        elif k == Qt.Key_Backslash:
            self.mainfrm.key_release("BACKSLASH")
        super().keyReleaseEvent(ev)

    def closeEvent(self, ev):
        self.mainfrm.close_request()
        ev.accept()
