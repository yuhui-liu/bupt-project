import numpy as np
import cv2
import matplotlib.pyplot as plt


def read_gray(path):
    """用 cv2 读灰度图，返回 uint8 numpy 数组"""
    img = cv2.imread(path, cv2.IMREAD_GRAYSCALE)
    if img is None:
        raise FileNotFoundError(path)
    return img


def laplacian_sharpen(img):
    """
    拉普拉斯锐化：
    - kernel_pos_center: 中心为正 5 的模板（等价于原图减去标准拉普拉斯）
    - kernel_neg_center: 中心为负 -4 的模板（标准拉普拉斯，需要做原图减去或加负值）
    """
    # 中心为正： [0,-1,0; -1,5,-1; 0,-1,0]
    kernel_pos_center = np.array(
        [[0, -1, 0], [-1, 5, -1], [0, -1, 0]], dtype=np.float32
    )

    # 中心为负： [0,1,0; 1,-4,1; 0,1,0]
    kernel_neg_center = np.array([[0, 1, 0], [1, -4, 1], [0, 1, 0]], dtype=np.float32)

    img_f = img.astype(np.float32)

    # 方式一：直接用中心为正的核卷积（相当于 g = f - Lap(f)）
    lap_pos = cv2.filter2D(img_f, ddepth=-1, kernel=kernel_pos_center)

    # 方式二：先计算标准拉普拉斯，再做 g = f - Lap(f)
    lap = cv2.filter2D(img_f, ddepth=-1, kernel=kernel_neg_center)
    lap_neg = img_f - lap  # 原图减去拉普拉斯结果

    # 裁剪到 [0,255]
    lap_pos = np.clip(lap_pos, 0, 255).astype(np.uint8)
    lap_neg = np.clip(lap_neg, 0, 255).astype(np.uint8)

    return lap_pos, lap_neg


def sobel_sharpen(img, alpha=0.5):
    """
    Sobel 锐化：
    1. 计算水平和垂直 Sobel
    2. 得到梯度幅值 mag
    3. g = f + alpha * mag
    """
    img_f = img.astype(np.float32)

    # Sobel 水平与垂直（OpenCV 默认核大小 3）
    grad_x = cv2.Sobel(img_f, cv2.CV_32F, 1, 0, ksize=3)
    grad_y = cv2.Sobel(img_f, cv2.CV_32F, 0, 1, ksize=3)

    # 梯度幅值
    mag = cv2.magnitude(grad_x, grad_y)  # sqrt(gx^2 + gy^2)

    # 归一化（可选，使幅值范围适中）
    mag_norm = cv2.normalize(mag, None, 0, 255, cv2.NORM_MINMAX)

    # 锐化：原图加权叠加梯度幅值
    sharpened = img_f + alpha * mag_norm

    sharpened = np.clip(sharpened, 0, 255).astype(np.uint8)
    mag_norm = mag_norm.astype(np.uint8)

    return sharpened, mag_norm, grad_x, grad_y


def save_results_matplotlib(img, lap_pos, lap_neg, sobel_sharp, mag, save_path):
    """
    用 matplotlib 排版并保存多幅结果图到一张图片
    """
    plt.figure(figsize=(10, 8))

    plt.subplot(2, 3, 1)
    plt.imshow(img, cmap="gray")
    plt.title("Original")
    plt.axis("off")

    plt.subplot(2, 3, 2)
    plt.imshow(lap_pos, cmap="gray")
    plt.title("Laplacian center positive")
    plt.axis("off")

    plt.subplot(2, 3, 3)
    plt.imshow(lap_neg, cmap="gray")
    plt.title("Laplacian center negative (f - Lap)")
    plt.axis("off")

    plt.subplot(2, 3, 4)
    plt.imshow(mag, cmap="gray")
    plt.title("Sobel |grad|")
    plt.axis("off")

    plt.subplot(2, 3, 5)
    plt.imshow(sobel_sharp, cmap="gray")
    plt.title("Sobel sharpened")
    plt.axis("off")

    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"结果图已保存到: {save_path}")


def main():
    # 1. 读取一幅稍显模糊或需要突出边缘的灰度图像
    img_path = "cell.tif"
    try:
        img = read_gray(img_path)
    except FileNotFoundError:
        print(f"无法读取图像：{img_path}，请检查路径或文件名。")
        return

    # 2. 拉普拉斯锐化
    lap_pos, lap_neg = laplacian_sharpen(img)

    # 2. Sobel 锐化
    sobel_sharp, mag, grad_x, grad_y = sobel_sharpen(img, alpha=0.5)

    # 3. 用 matplotlib 保存结果（用于写报告）
    save_results_matplotlib(
        img,
        lap_pos,
        lap_neg,
        sobel_sharp,
        mag,
        save_path="task2_sharpen_results.png",
    )

    # 4. 仍然用 OpenCV 显示结果（可选）
    cv2.imshow("Original", img)
    cv2.imshow("Laplacian center positive (0,-1,0; -1,5,-1; 0,-1,0)", lap_pos)
    cv2.imshow(
        "Laplacian center negative (0,1,0; 1,-4,1; 0,1,0) via f - Lap(f)",
        lap_neg,
    )
    cv2.imshow("Sobel gradient magnitude", mag)
    cv2.imshow("Sobel sharpened (f + alpha * |grad|)", sobel_sharp)

    print("按任意键关闭窗口...")
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
