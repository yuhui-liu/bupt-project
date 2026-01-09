import numpy as np
import matplotlib.pyplot as plt
from PIL import Image  # 使用 PIL 读取图像


def compute_histogram(img, L=256):
    """计算灰度直方图"""
    rows, cols = img.shape
    hist = np.zeros(L, dtype=np.int64)
    for r in range(rows):
        for c in range(cols):
            hist[img[r, c]] += 1
    return hist


def histogram_equalization(img, L=256):
    """对灰度图像做直方图均衡化，返回均衡化后的图像和映射表、直方图等"""
    rows, cols = img.shape

    # 1. 直方图
    hist = compute_histogram(img, L=L)

    # 2. 概率密度函数 pdf
    pdf = hist / float(rows * cols)

    # 3. 累积分布函数 cdf
    cdf = np.cumsum(pdf)

    # 4. 变换函数 T(r) = (L-1)*cdf(r)
    T = np.round((L - 1) * cdf).astype(np.uint8)

    # 5. 利用 T 进行灰度映射
    img_eq = np.zeros_like(img, dtype=np.uint8)
    for r in range(rows):
        for c in range(cols):
            img_eq[r, c] = T[img[r, c]]

    # 6. 均衡化后直方图
    hist_eq = compute_histogram(img_eq, L=L)

    return img_eq, hist, hist_eq, T, pdf, cdf


def show_results(img, img_eq, hist, hist_eq, L=256):
    """显示原图/均衡化图及其直方图"""
    plt.figure(figsize=(10, 8))

    plt.subplot(2, 2, 1)
    plt.imshow(img, cmap="gray", vmin=0, vmax=255)
    plt.title("raw image")
    plt.axis("off")

    plt.subplot(2, 2, 2)
    plt.bar(np.arange(L), hist, width=1.0, color="black")
    plt.title("raw image histogram")
    plt.xlim([0, 255])

    plt.subplot(2, 2, 3)
    plt.imshow(img_eq, cmap="gray", vmin=0, vmax=255)
    plt.title("equalized image")
    plt.axis("off")

    plt.subplot(2, 2, 4)
    plt.bar(np.arange(L), hist_eq, width=1.0, color="black")
    plt.title("equalized image histogram")
    plt.xlim([0, 255])

    plt.tight_layout()
    plt.savefig("hist_equalization_result.png")  # 保存到文件
    plt.close()


def main():
    # 1. 使用 PIL 读取灰度图像
    try:
        img_pil = Image.open("pout.tif").convert("L")  # "L" 表示 8-bit 灰度
    except FileNotFoundError:
        raise FileNotFoundError("图像未找到，请确认文件名和路径。")

    # 转为 numpy 数组，类型 uint8
    img = np.array(img_pil, dtype=np.uint8)

    # 2. 直方图均衡化
    img_eq, hist, hist_eq, T, pdf, cdf = histogram_equalization(img, L=256)

    # 3. 显示结果
    show_results(img, img_eq, hist, hist_eq, L=256)


if __name__ == "__main__":
    main()
