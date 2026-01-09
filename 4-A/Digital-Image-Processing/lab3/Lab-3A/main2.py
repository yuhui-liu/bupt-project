import numpy as np
import matplotlib.pyplot as plt
from PIL import Image


def compute_histogram(img, L=256):
    """计算灰度直方图"""
    rows, cols = img.shape
    hist = np.zeros(L, dtype=np.int64)
    for r in range(rows):
        for c in range(cols):
            hist[img[r, c]] += 1
    return hist


def compute_cdf_from_hist(hist):
    """由直方图计算 CDF（累积分布函数），范围 [0,1]"""
    total = hist.sum()
    pdf = hist / float(total)
    cdf = np.cumsum(pdf)
    return cdf


def histogram_matching(imgA, imgB, L=256):
    """
    直方图规定化：将图像 A 的直方图匹配到图像 B 的直方图
    返回：匹配后的图像 Ah，以及 A/B 的直方图、CDF、映射表等
    """
    # 直方图
    histA = compute_histogram(imgA, L=L)
    histB = compute_histogram(imgB, L=L)

    # CDF
    cdfA = compute_cdf_from_hist(histA)
    cdfB = compute_cdf_from_hist(histB)

    # 建立映射：对 A 中每个灰度 rA，找到 B 中 CDF 最接近的灰度 rB
    # mapping[rA] = rB
    mapping = np.zeros(L, dtype=np.uint8)
    for rA in range(L):
        # 对应的目标 CDF 值
        target = cdfA[rA]
        # 找到 B 的 CDF 中与 target 最接近的位置
        # 等价于 argmin |cdfB - target|
        idx = np.argmin(np.abs(cdfB - target))
        mapping[rA] = idx

    # 用 mapping 对 A 做像素级映射，得到规定化后的图像 Ah
    rows, cols = imgA.shape
    imgA_h = np.zeros_like(imgA, dtype=np.uint8)
    for r in range(rows):
        for c in range(cols):
            imgA_h[r, c] = mapping[imgA[r, c]]

    # 匹配后图像直方图
    histA_h = compute_histogram(imgA_h, L=L)

    return imgA_h, histA, histB, histA_h, cdfA, cdfB, mapping


def show_matching_results(imgA, imgB, imgA_h, histA, histB, histA_h, L=256):
    """显示 A / B / A规定化后图像及其直方图"""
    plt.figure(figsize=(12, 8))

    # 第一行：三幅图像
    plt.subplot(2, 3, 1)
    plt.imshow(imgA, cmap="gray", vmin=0, vmax=255)
    plt.title("A: source image")
    plt.axis("off")

    plt.subplot(2, 3, 2)
    plt.imshow(imgB, cmap="gray", vmin=0, vmax=255)
    plt.title("B: reference image")
    plt.axis("off")

    plt.subplot(2, 3, 3)
    plt.imshow(imgA_h, cmap="gray", vmin=0, vmax=255)
    plt.title("A after histogram matching")
    plt.axis("off")

    # 第二行：直方图
    plt.subplot(2, 3, 4)
    plt.bar(np.arange(L), histA, width=1.0, color="black")
    plt.title("histogram of A")
    plt.xlim([0, 255])

    plt.subplot(2, 3, 5)
    plt.bar(np.arange(L), histB, width=1.0, color="black")
    plt.title("histogram of B (reference)")
    plt.xlim([0, 255])

    plt.subplot(2, 3, 6)
    plt.bar(np.arange(L), histA_h, width=1.0, color="black")
    plt.title("histogram of A after matching")
    plt.xlim([0, 255])

    plt.tight_layout()
    plt.savefig("hist_matching_result.png")
    plt.close()


def main():
    # 1. 使用 PIL 读取两幅灰度图像
    # A: 待处理图像；B: 参考图像
    try:
        imgA_pil = Image.open("tire.tif").convert("L")  # 源图像 A
        imgB_pil = Image.open("pout.tif").convert("L")  # 参考图像 B
    except FileNotFoundError:
        raise FileNotFoundError(
            "图像文件未找到，请确认 tire.tif 和 pout.tif 在当前目录。"
        )

    imgA = np.array(imgA_pil, dtype=np.uint8)
    imgB = np.array(imgB_pil, dtype=np.uint8)

    # 2 & 3. 直方图规定化
    imgA_h, histA, histB, histA_h, cdfA, cdfB, mapping = histogram_matching(
        imgA, imgB, L=256
    )

    # 4. 显示（保存）结果
    show_matching_results(imgA, imgB, imgA_h, histA, histB, histA_h, L=256)


if __name__ == "__main__":
    main()
