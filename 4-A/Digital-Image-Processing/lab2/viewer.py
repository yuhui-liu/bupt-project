from PIL import Image
from PyQt6.QtWidgets import (
    QMainWindow,
    QLabel,
    QFileDialog,
    QScrollArea,
    QHBoxLayout,
    QVBoxLayout,
    QWidget,
    QMessageBox,
    QComboBox,
    QPushButton,
    QSlider,
    QDoubleSpinBox,
    QSizePolicy,
)
from PyQt6.QtGui import QPixmap, QAction
from PyQt6.QtCore import Qt

from lib.gray_transform import gray_transform
from problem_b import adaptive_contrast_adjust


class ImageViewer(QMainWindow):
    """
    图片查看器类，用于显示和处理图像。
    该类继承自QMainWindow，提供了一个图形用户界面来查看图像，
    并支持多种图像处理操作，如反色、线性变换、伽马变换、对数变换和自动对比度拉伸。
    """

    def __init__(self):
        super().__init__()
        self.setWindowTitle("图片查看器")
        self.resize(900, 700)

        # 当前图片路径
        self.image_path = None

        # 创建下拉菜单
        self.combo = QComboBox()
        self.combo.addItems(
            ["无", "反色", "线性变换", "伽马变换", "对数变换", "自动对比度拉伸"]
        )
        self.combo.setToolTip("选择左侧显示的处理")
        self.combo.currentIndexChanged.connect(self.combo_changed)
        # 创建按钮
        self.apply_button = QPushButton("应用")
        self.apply_button.setToolTip("应用当前下拉选择到左侧")
        self.apply_button.setFixedWidth(80)  # 控制按钮宽度
        self.apply_button.clicked.connect(self.apply_button_clicked)
        # 将下拉和按钮放在同一行
        combo_button_layout = QHBoxLayout()
        combo_button_layout.addWidget(self.combo)
        combo_button_layout.addWidget(self.apply_button)

        # ---------- 线性变换的参数输入 ----------
        # 每个滑块上方显示当前值，滑块本身为横向（Qt.Horizontal）
        def make_slider(default: int, name: str):
            lbl = QLabel(f"{name}: {default}")
            lbl.setAlignment(Qt.AlignmentFlag.AlignCenter)
            lbl.setFixedHeight(15)  # 控制标签高度
            s = QSlider(Qt.Orientation.Horizontal)
            s.setRange(0, 255)
            s.setValue(default)
            s.valueChanged.connect(lambda v, L=lbl, N=name: L.setText(f"{N}: {v}"))
            return lbl, s

        self.slider_a_label, self.slider_a = make_slider(80, "a")
        self.slider_b_label, self.slider_b = make_slider(180, "b")
        self.slider_c_label, self.slider_c = make_slider(50, "c")
        self.slider_d_label, self.slider_d = make_slider(220, "d")

        # 把 4 个滑块放在一行，每个滑块上方有标签（垂直排列）
        sliders_layout = QHBoxLayout()
        for lbl, s in (
            (self.slider_a_label, self.slider_a),
            (self.slider_b_label, self.slider_b),
            (self.slider_c_label, self.slider_c),
            (self.slider_d_label, self.slider_d),
        ):
            col = QVBoxLayout()
            col.addWidget(lbl)
            col.addWidget(s)
            sliders_layout.addLayout(col)

        self.sliders_widget = QWidget()
        self.sliders_widget.setLayout(sliders_layout)
        self.sliders_widget.setFixedHeight(100)

        # 调整单个滑块的高度
        for s in (self.slider_a, self.slider_b, self.slider_c, self.slider_d):
            s.setFixedHeight(20)

        self.sliders_widget.hide()  # 默认隐藏，仅在 "线性变换" 时显示

        # ---------- 伽马与对数变换的参数输入面板 ----------
        self.gamma_label = QLabel("gamma:")
        self.gamma_value_label = QLabel("2.00")
        self.gamma_value_label.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.gamma_slider = QSlider(Qt.Orientation.Horizontal)
        self.gamma_slider.setRange(1, 1500)
        self.gamma_slider.setValue(200)
        self.gamma_slider.valueChanged.connect(
            lambda v: self.gamma_value_label.setText(f"{v/100.0:.2f}")
        )
        self.gamma_slider.valueChanged.connect(self.apply_gamma)

        self.gamma_C_label = QLabel("C:")
        self.gamma_C_spin = QDoubleSpinBox()
        self.gamma_C_spin.setRange(0.0, 100.0)
        self.gamma_C_spin.setSingleStep(0.1)
        self.gamma_C_spin.setValue(1.0)

        gamma_layout = QHBoxLayout()
        gamma_layout.addWidget(self.gamma_label)
        gamma_layout.addWidget(self.gamma_value_label)
        gamma_layout.addWidget(self.gamma_slider)
        gamma_layout.addWidget(self.gamma_C_label)
        gamma_layout.addWidget(self.gamma_C_spin)
        self.gamma_widget = QWidget()
        self.gamma_widget.setLayout(gamma_layout)
        self.gamma_widget.hide()  # 仅在下拉选择为 "伽马变换" 时显示

        # 对数变换：仅 C
        self.log_C_label = QLabel("C:")
        self.log_C_spin = QDoubleSpinBox()
        self.log_C_spin.setRange(0.0, 100.0)
        self.log_C_spin.setSingleStep(0.1)
        self.log_C_spin.setValue(1.0)

        log_layout = QHBoxLayout()
        log_layout.addWidget(self.log_C_label)
        log_layout.addWidget(self.log_C_spin)
        self.log_widget = QWidget()
        self.log_widget.setLayout(log_layout)
        self.log_widget.hide()  # 仅在下拉选择为 "对数变换" 时显示
        # ----- 控制 gamma_widget 与 log_widget 的高度与大小策略 -----
        # 固定高度+允许水平扩展
        self.gamma_widget.setFixedHeight(48)  # 根据需要调整像素高度
        self.gamma_widget.setSizePolicy(
            QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed
        )
        # 让输入控件看起来更紧凑
        self.gamma_slider.setFixedHeight(24)
        self.gamma_C_spin.setFixedHeight(24)
        # 调整布局内边距/间距以获得更好视觉
        gamma_layout.setContentsMargins(6, 6, 6, 6)
        gamma_layout.setSpacing(8)

        self.log_widget.setFixedHeight(40)
        self.log_widget.setSizePolicy(
            QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed
        )
        self.log_C_spin.setFixedHeight(24)
        log_layout.setContentsMargins(6, 6, 6, 6)
        log_layout.setSpacing(8)

        # --------

        # 创建图像显示标签
        self.left_label = QLabel("请选择一张图片")
        self.right_label = QLabel("请选择一张图片")
        for lbl in (self.left_label, self.right_label):
            lbl.setAlignment(Qt.AlignmentFlag.AlignCenter)
            lbl.setStyleSheet("border: 1px solid #aaa; background-color: #fafafa;")

        # 创建描述文本
        self.left_text = QLabel("原图")
        self.right_text = QLabel("处理后")
        for txt in (self.left_text, self.right_text):
            txt.setAlignment(Qt.AlignmentFlag.AlignCenter)
            txt.setStyleSheet("font-size: 14px; color: #fafafa; margin-top: 5px;")
            txt.setFixedHeight(25)

        # 创建直方图显示标签
        self.left_histogram = QLabel()
        self.right_histogram = QLabel()
        for hist in (self.left_histogram, self.right_histogram):
            hist.setAlignment(Qt.AlignmentFlag.AlignCenter)
            hist.setStyleSheet("border: 1px solid #aaa; background-color: #fafafa;")

        # 布局设置
        left_box = QVBoxLayout()
        left_box.addWidget(self.left_label)
        left_box.addWidget(self.left_text)
        left_box.addWidget(self.left_histogram)

        right_box = QVBoxLayout()
        right_box.addWidget(self.right_label)
        right_box.addWidget(self.right_text)
        right_box.addWidget(self.right_histogram)

        image_display_layout = QHBoxLayout()
        image_display_layout.addLayout(left_box)
        image_display_layout.addLayout(right_box)

        # 中心部件（QMainWindow要求必须有）
        central_widget = QWidget()
        main_layout = QVBoxLayout()
        main_layout.addLayout(combo_button_layout)
        main_layout.addWidget(self.sliders_widget)
        main_layout.addWidget(self.gamma_widget)
        main_layout.addWidget(self.log_widget)
        # 创建滚动区域以显示图像
        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        image_widget = QWidget()
        image_widget.setLayout(image_display_layout)
        scroll_area.setWidget(image_widget)
        main_layout.addWidget(scroll_area)
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

        # 初始化菜单栏
        self._create_menus()

    # --------------------------------
    #          菜单栏设置
    # --------------------------------
    def _create_menus(self):
        menu_bar = self.menuBar()

        # ---- 文件菜单 ----
        file_menu = menu_bar.addMenu("文件 (&F)")

        open_action = QAction("打开图片 (&O)", self)
        open_action.setShortcut("Ctrl+O")
        open_action.triggered.connect(self.open_image)
        file_menu.addAction(open_action)

        save_action = QAction("保存处理后图片 (&S)", self)
        save_action.setShortcut("Ctrl+S")
        save_action.triggered.connect(self.save_processed_image)
        file_menu.addAction(save_action)

        exit_action = QAction("退出 (&Q)", self)
        exit_action.setShortcut("Ctrl+Q")
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)

    # --------------------------------
    #          功能逻辑
    # --------------------------------
    def open_image(self):
        """打开文件选择对话框"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, "选择图片", "", "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif)"
        )
        if file_path:
            self.image_path = file_path
            self.load_image(file_path)

    def load_image(self, file_path: str):
        """加载图片"""
        pixmap = QPixmap(file_path)
        if pixmap.isNull():
            QMessageBox.warning(self, "错误", "无法加载图片。")
            return

        self.left_label.setPixmap(pixmap)
        self.left_histogram.setPixmap(get_histogram_image(Image.open(file_path)))

    def invert_colors(self):
        """反色处理"""
        if not self.image_path:
            QMessageBox.information(self, "提示", "请先打开一张图片。")
            return

        return gray_transform(self.image_path, "reverse", {})

    def linear_transform(self, a: int, b: int, c: int, d: int):
        """线性变换处理"""
        if not self.image_path:
            QMessageBox.information(self, "提示", "请先打开一张图片。")
            return

        params = {"a": a, "b": b, "c": c, "d": d}
        return gray_transform(self.image_path, "linear", params)

    def gamma_transform(self, gamma: float, C: float):
        """伽马变换处理"""
        if not self.image_path:
            QMessageBox.information(self, "提示", "请先打开一张图片。")
            return

        params = {"gamma": gamma, "C": C}
        return gray_transform(self.image_path, "gamma", params)

    def log_transform(self, C: float):
        """对数变换处理"""
        if not self.image_path:
            QMessageBox.information(self, "提示", "请先打开一张图片。")
            return

        params = {"C": C}
        return gray_transform(self.image_path, "log", params)

    def adaptive_contrast_adjust(self):
        """自动对比度拉伸处理"""
        if not self.image_path:
            QMessageBox.information(self, "提示", "请先打开一张图片。")
            return

        return adaptive_contrast_adjust(Image.open(self.image_path))

    # 下拉菜单响应函数
    def combo_changed(self, _):
        """根据下拉菜单选择更新选项"""
        choice = self.combo.currentText()
        if choice == "线性变换":
            self.sliders_widget.show()
        else:
            self.sliders_widget.hide()

        if choice == "伽马变换":
            self.gamma_widget.show()
        else:
            self.gamma_widget.hide()

        if choice == "对数变换":
            self.log_widget.show()
        else:
            self.log_widget.hide()

    # 应用按钮的槽函数
    def apply_button_clicked(self):
        """点击按钮时应用下拉选择（复用下拉处理函数）"""
        if not self.image_path:
            QMessageBox.information(self, "提示", "请先打开一张图片。")
            return
        # 调用已有的处理函数
        processed_image = None
        if self.combo.currentText() == "无":
            processed_image = Image.open(self.image_path)
            self.right_label.setPixmap(QPixmap(self.image_path))
        elif self.combo.currentText() == "反色":
            processed_image = self.invert_colors()
            self.right_label.setPixmap(pil_to_pixmap(processed_image))
        elif self.combo.currentText() == "线性变换":
            a = self.slider_a.value()
            b = self.slider_b.value()
            c = self.slider_c.value()
            d = self.slider_d.value()
            processed_image = self.linear_transform(a, b, c, d)
            self.right_label.setPixmap(pil_to_pixmap(processed_image))
        elif self.combo.currentText() == "伽马变换":
            gamma = self.gamma_slider.value() / 100.0
            C = float(self.gamma_C_spin.value())
            processed_image = self.gamma_transform(gamma, C)
            self.right_label.setPixmap(pil_to_pixmap(processed_image))
        elif self.combo.currentText() == "对数变换":
            C = float(self.log_C_spin.value())
            processed_image = self.log_transform(C)
            self.right_label.setPixmap(pil_to_pixmap(processed_image))
        elif self.combo.currentText() == "自动对比度拉伸":
            processed_image = self.adaptive_contrast_adjust()
            self.right_label.setPixmap(pil_to_pixmap(processed_image))

        self.right_histogram.setPixmap(get_histogram_image(processed_image))

    def apply_gamma(self):
        """实时应用伽马变换"""
        if self.combo.currentText() == "伽马变换" and self.image_path:
            gamma = self.gamma_slider.value() / 100.0
            C = float(self.gamma_C_spin.value())
            processed_image = self.gamma_transform(gamma, C)
            self.right_label.setPixmap(pil_to_pixmap(processed_image))
            self.right_histogram.setPixmap(get_histogram_image(processed_image))

    def save_processed_image(self):
        """保存当前右侧图像"""
        if self.right_label.pixmap() is None:
            QMessageBox.information(self, "提示", "没有图像可保存。")
            return

        file_path, _ = QFileDialog.getSaveFileName(
            self,
            "保存图片",
            "",
            "PNG 图片 (*.png);;JPEG 图片 (*.jpg *.jpeg);;BMP 图片 (*.bmp)",
        )
        if file_path:
            self.right_label.pixmap().save(file_path)


from PyQt6.QtGui import QPixmap, QImage


def pil_to_pixmap(image: Image.Image) -> QPixmap:
    """将 PIL.Image 转换为 QPixmap"""
    if image.mode != "RGBA":
        image = image.convert("RGBA")

    data = image.tobytes("raw", "RGBA")
    qimage = QImage(data, image.width, image.height, QImage.Format.Format_RGBA8888)
    return QPixmap.fromImage(qimage)


def pixmap_to_pil(pixmap: QPixmap) -> Image.Image:
    """将 QPixmap 转换为 PIL.Image"""
    qimage = pixmap.toImage()
    buffer = qimage.bits().asstring(qimage.byteCount())
    image = Image.frombytes(
        "RGBA", (qimage.width(), qimage.height()), buffer, "raw", "RGBA"
    )
    return image


def get_histogram_image(
    image: Image.Image, scale_x: float = 2.2, scale_y: float = 2.0
) -> QPixmap:
    """生成图像的直方图并转换为 QPixmap"""
    import numpy as np
    import math
    from PyQt6.QtGui import QPainter, QPen, QColor, QFont
    from PyQt6.QtCore import Qt

    gray = np.array(image.convert("L"), dtype=np.uint8)
    hist = np.bincount(gray.ravel(), minlength=256).astype(np.int64)
    m = int(hist.max())
    hist_norm = (hist / m) if m > 0 else np.zeros_like(hist, dtype=np.float32)

    # 绘图区尺寸
    plot_w = int(256 * scale_x)
    plot_h = int(140 * scale_y)
    margin_l, margin_r, margin_t, margin_b = 64, 20, 12, 44
    W = margin_l + plot_w + margin_r
    H = margin_t + plot_h + margin_b

    qimg = QImage(W, H, QImage.Format.Format_RGB32)
    qimg.fill(Qt.GlobalColor.white)

    # 生成“漂亮”的 y 轴刻度（绝对计数）
    def nice_ticks(max_v: int, n: int = 4):
        if max_v <= 0:
            return [0]
        step0 = max_v / max(1, n - 1)
        p = 10 ** math.floor(math.log10(step0))
        r = step0 / p
        if r < 1.5:
            step = 1 * p
        elif r < 3:
            step = 2 * p
        elif r < 7:
            step = 5 * p
        else:
            step = 10 * p
        vals = list(range(0, max_v + step, step))
        if vals[-1] != max_v:
            vals.append(max_v)
        return vals

    yticks_vals = nice_ticks(m, 4)  # 约 4 个刻度

    painter = QPainter(qimg)
    try:
        painter.setRenderHint(QPainter.RenderHint.Antialiasing, False)

        # 轴/网格/柱使用“化妆笔”，线条始终为 1px
        axis_pen = QPen(QColor(0, 0, 0))
        axis_pen.setWidth(0)
        axis_pen.setCosmetic(True)

        grid_pen = QPen(QColor(200, 200, 200))
        grid_pen.setStyle(Qt.PenStyle.DashLine)
        grid_pen.setWidth(0)
        grid_pen.setCosmetic(True)

        bar_pen = QPen(QColor(0, 0, 0))
        bar_pen.setWidth(0)
        bar_pen.setCosmetic(True)

        font = QFont()
        base_size = max(8, int(9 * scale_y))
        font.setPointSize(base_size)
        painter.setFont(font)

        x0 = margin_l
        y0 = margin_t + plot_h

        # 网格线（按绝对计数的刻度）
        if m > 0:
            painter.setPen(grid_pen)
            for tv in yticks_vals[1:-1]:
                y = margin_t + int(round(plot_h * (1.0 - tv / m)))
                painter.drawLine(x0, y, x0 + plot_w, y)

        # 坐标轴
        painter.setPen(axis_pen)
        painter.drawLine(x0, margin_t, x0, y0)
        painter.drawLine(x0, y0, x0 + plot_w, y0)

        # x 轴刻度与标签
        xticks = [0, 64, 128, 192, 255]
        for v in xticks:
            x = x0 + int(round(v * (plot_w - 1) / 255.0))
            painter.drawLine(x, y0, x, y0 + 5)
            txt = str(v)
            tw = painter.fontMetrics().horizontalAdvance(txt)
            painter.drawText(x - tw // 2, y0 + int(16 * scale_y), txt)

        # y 轴刻度与标签（绝对计数，字体稍小）
        y_font = QFont(font)
        y_font.setPointSize(max(7, int(base_size * 0.8)))  # 略小一点
        painter.setFont(y_font)
        for tv in yticks_vals:
            y = y0 if m == 0 else margin_t + int(round(plot_h * (1.0 - tv / m)))
            painter.drawLine(x0 - 5, y, x0, y)
            txt = f"{tv}"
            th = painter.fontMetrics().ascent()
            painter.drawText(6, y + th // 2 - 2, txt)
        painter.setFont(font)  # 如后续还需绘制文字，恢复原字体

        # 细竖线绘制每个 bin（跳过 0）
        painter.setPen(bar_pen)
        for x in range(256):
            v = float(hist_norm[x])
            if v <= 0.0:
                continue
            h = max(1, int(v * plot_h + 0.5))
            xL = x0 + int(round(x * plot_w / 256.0))
            xR = x0 + int(round((x + 1) * plot_w / 256.0))
            xc = (xL + xR) // 2
            painter.drawLine(xc, y0 - h, xc, y0 - 1)
    finally:
        painter.end()
    return QPixmap.fromImage(qimg)
