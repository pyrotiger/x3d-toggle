import sys
from PyQt5.QtWidgets import QApplication, QSystemTrayIcon, QMenu
from PyQt5.QtGui import QIcon

app = QApplication(sys.argv)
icon = QIcon.fromTheme("document-new")
tray = QSystemTrayIcon(icon)
menu = QMenu()
menu.addAction("Test")
tray.setContextMenu(menu)
tray.show()
sys.exit(app.exec_())
