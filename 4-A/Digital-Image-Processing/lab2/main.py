import sys
from PyQt6.QtWidgets import QApplication
from viewer import ImageViewer


def main():
    app = QApplication(sys.argv)
    viewer = ImageViewer()
    viewer.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
