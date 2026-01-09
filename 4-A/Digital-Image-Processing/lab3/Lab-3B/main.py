import cv2
import numpy as np
import math
import matplotlib.pyplot as plt  # 新增

# =====================
# 工具函数
# =====================


def add_salt_pepper_noise(img, amount=0.02, salt_vs_pepper=0.5):
    """
    img: 灰度图 [0,255], uint8
    amount: 噪声像素占总像素比例
    salt_vs_pepper: 盐噪声比例
    """
    noisy = img.copy()
    h, w = img.shape
    num_noise = int(h * w * amount)
    num_salt = int(num_noise * salt_vs_pepper)
    num_pepper = num_noise - num_salt

    # 盐噪声（255）
    coords_salt = (np.random.randint(0, h, num_salt), np.random.randint(0, w, num_salt))
    noisy[coords_salt] = 255

    # 椒噪声（0）
    coords_pepper = (
        np.random.randint(0, h, num_pepper),
        np.random.randint(0, w, num_pepper),
    )
    noisy[coords_pepper] = 0

    return noisy


def add_gaussian_noise(img, mean=0, sigma=15):
    """
    img: 灰度图 [0,255], uint8
    返回添加高斯噪声后的图像
    """
    noise = np.random.normal(mean, sigma, img.shape).astype(np.float32)
    noisy = img.astype(np.float32) + noise
    noisy = np.clip(noisy, 0, 255).astype(np.uint8)
    return noisy


def psnr(img1, img2):
    """
    计算两幅同尺寸灰度图像的 PSNR
    """
    img1 = img1.astype(np.float64)
    img2 = img2.astype(np.float64)
    mse = np.mean((img1 - img2) ** 2)
    if mse == 0:
        return float("inf")
    PIXEL_MAX = 255.0
    return 10 * math.log10((PIXEL_MAX**2) / mse)


def save_results_matplotlib(
    img,
    img_sp,
    img_gauss,
    sp_mean_3,
    sp_mean_5,
    sp_gauss_3,
    sp_gauss_5,
    sp_med_3,
    sp_med_5,
    g_mean_3,
    g_mean_5,
    g_gauss_3,
    g_gauss_5,
    g_med_3,
    g_med_5,
    save_path="task1_denoise_results.png",
):
    """
    用 matplotlib 排版并保存多幅结果图到一张图片
    上面一行原图和两种噪声，下面几行是不同滤波结果
    """
    plt.figure(figsize=(14, 10))

    # 原图 + 噪声
    plt.subplot(4, 4, 1)
    plt.imshow(img, cmap="gray")
    plt.title("Original")
    plt.axis("off")

    plt.subplot(4, 4, 2)
    plt.imshow(img_sp, cmap="gray")
    plt.title("SP noise")
    plt.axis("off")

    plt.subplot(4, 4, 3)
    plt.imshow(img_gauss, cmap="gray")
    plt.title("Gaussian noise")
    plt.axis("off")

    # 椒盐噪声：均值、高斯、中值
    plt.subplot(4, 4, 5)
    plt.imshow(sp_mean_3, cmap="gray")
    plt.title("SP Mean 3x3")
    plt.axis("off")

    plt.subplot(4, 4, 6)
    plt.imshow(sp_mean_5, cmap="gray")
    plt.title("SP Mean 5x5")
    plt.axis("off")

    plt.subplot(4, 4, 9)
    plt.imshow(sp_gauss_3, cmap="gray")
    plt.title("SP Gaussian 3x3")
    plt.axis("off")

    plt.subplot(4, 4, 10)
    plt.imshow(sp_gauss_5, cmap="gray")
    plt.title("SP Gaussian 5x5")
    plt.axis("off")

    plt.subplot(4, 4, 13)
    plt.imshow(sp_med_3, cmap="gray")
    plt.title("SP Median 3x3")
    plt.axis("off")

    plt.subplot(4, 4, 14)
    plt.imshow(sp_med_5, cmap="gray")
    plt.title("SP Median 5x5")
    plt.axis("off")

    # 高斯噪声：均值、高斯、中值
    plt.subplot(4, 4, 7)
    plt.imshow(g_mean_3, cmap="gray")
    plt.title("G Mean 3x3")
    plt.axis("off")

    plt.subplot(4, 4, 8)
    plt.imshow(g_mean_5, cmap="gray")
    plt.title("G Mean 5x5")
    plt.axis("off")

    plt.subplot(4, 4, 11)
    plt.imshow(g_gauss_3, cmap="gray")
    plt.title("G Gaussian 3x3")
    plt.axis("off")

    plt.subplot(4, 4, 12)
    plt.imshow(g_gauss_5, cmap="gray")
    plt.title("G Gaussian 5x5")
    plt.axis("off")

    plt.subplot(4, 4, 15)
    plt.imshow(g_med_3, cmap="gray")
    plt.title("G Median 3x3")
    plt.axis("off")

    plt.subplot(4, 4, 16)
    plt.imshow(g_med_5, cmap="gray")
    plt.title("G Median 5x5")
    plt.axis("off")

    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"去噪结果图已保存到: {save_path}")


# =====================
# 主流程
# =====================


def main():
    # 1. 读取原始灰度图
    img = cv2.imread("lena.bmp", cv2.IMREAD_GRAYSCALE)
    if img is None:
        print("无法读取图像：lena.bmp，请检查路径。")
        return

    # 2. 生成噪声图像
    img_sp = add_salt_pepper_noise(img, amount=0.02)  # 椒盐噪声图像 I
    img_gauss = add_gaussian_noise(img, mean=0, sigma=15)  # 高斯噪声图像 II

    # 3. 对两种噪声分别进行各种滤波
    # ---- 椒盐噪声 ----
    sp_mean_3 = cv2.blur(img_sp, (3, 3))
    sp_mean_5 = cv2.blur(img_sp, (5, 5))
    sp_gauss_3 = cv2.GaussianBlur(img_sp, (3, 3), sigmaX=1.0)
    sp_gauss_5 = cv2.GaussianBlur(img_sp, (5, 5), sigmaX=1.0)
    sp_med_3 = cv2.medianBlur(img_sp, 3)
    sp_med_5 = cv2.medianBlur(img_sp, 5)

    # ---- 高斯噪声 ----
    g_mean_3 = cv2.blur(img_gauss, (3, 3))
    g_mean_5 = cv2.blur(img_gauss, (5, 5))
    g_gauss_3 = cv2.GaussianBlur(img_gauss, (3, 3), sigmaX=1.0)
    g_gauss_5 = cv2.GaussianBlur(img_gauss, (5, 5), sigmaX=1.0)
    g_med_3 = cv2.medianBlur(img_gauss, 3)
    g_med_5 = cv2.medianBlur(img_gauss, 5)

    # 4. 计算 PSNR（与原始无噪声图比较）
    print("===== 椒盐噪声图像 I =====")
    print("原始 vs 椒盐噪声:", psnr(img, img_sp))
    print("3x3 均值滤波:", psnr(img, sp_mean_3))
    print("5x5 均值滤波:", psnr(img, sp_mean_5))
    print("3x3 高斯滤波:", psnr(img, sp_gauss_3))
    print("5x5 高斯滤波:", psnr(img, sp_gauss_5))
    print("3x3 中值滤波:", psnr(img, sp_med_3))
    print("5x5 中值滤波:", psnr(img, sp_med_5))

    print("\n===== 高斯噪声图像 II =====")
    print("原始 vs 高斯噪声:", psnr(img, img_gauss))
    print("3x3 均值滤波:", psnr(img, g_mean_3))
    print("5x5 均值滤波:", psnr(img, g_mean_5))
    print("3x3 高斯滤波:", psnr(img, g_gauss_3))
    print("5x5 高斯滤波:", psnr(img, g_gauss_5))
    print("3x3 中值滤波:", psnr(img, g_med_3))
    print("5x5 中值滤波:", psnr(img, g_med_5))

    # 5. 保存结果图用于定性分析 / 报告
    save_results_matplotlib(
        img,
        img_sp,
        img_gauss,
        sp_mean_3,
        sp_mean_5,
        sp_gauss_3,
        sp_gauss_5,
        sp_med_3,
        sp_med_5,
        g_mean_3,
        g_mean_5,
        g_gauss_3,
        g_gauss_5,
        g_med_3,
        g_med_5,
        save_path="task1_denoise_results.png",
    )

    # 6. 显示部分结果（可选）
    # cv2.imshow("Original", img)
    # cv2.imshow("SP Noise", img_sp)
    # cv2.imshow("SP - Median 3x3", sp_med_3)
    # cv2.imshow("SP - Median 5x5", sp_med_5)
    # cv2.imshow("Gaussian Noise", img_gauss)
    # cv2.imshow("G - Gaussian 3x3", g_gauss_3)
    # cv2.imshow("G - Gaussian 5x5", g_gauss_5)

    # print("\n按任意键关闭图像窗口...")
    # cv2.waitKey(0)
    # cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
