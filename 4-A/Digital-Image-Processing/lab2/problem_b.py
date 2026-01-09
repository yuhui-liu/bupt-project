from PIL import Image
import numpy as np


def grayscale_slice(img: Image, a: int, b: int, c: int, d: int) -> Image:
    """
    对图像进行灰度切片处理。
    该函数将图像中灰度值在 [a, b] 范围内的像素线性映射到 [c, d] 范围内。范围外的像素保持不变。

    参数:
        img: 输入图像。
        a, b: 需要切片的灰度范围 [a, b]
        c, d: 切片后映射的灰度范围 [c, d]

    返回：
        处理后的图像。
    """
    assert 0 <= a < b <= 255, "a 和 b 必须在 [0, 255] 范围内且 a < b"
    assert 0 <= c < d <= 255, "c 和 d 必须在 [0, 255] 范围内且 c < d"
    img_arr = np.array(img, dtype=np.uint8)
    mask = (a <= img_arr) & (img_arr <= b)
    img_arr[mask] = (d - c) / (b - a) * (img_arr[mask] - a) + c
    return Image.fromarray(img_arr)


def adaptive_contrast_adjust(img: Image):
    """
    对比度自适应调整。
    该函数将图像的灰度值线性映射到 [0, 255] 范围内，以增强图像的对比度。
    参数:
        img: 输入图像。
    返回：
        处理后的图像。
    """
    img_arr = np.array(img, dtype=np.uint8)
    r_min, r_max = img_arr.min(), img_arr.max()
    if r_max == r_min:
        return img  # 避免除以零
    out_arr = (img_arr - r_min) / (r_max - r_min) * 255
    return Image.fromarray(out_arr.astype(np.uint8))


if __name__ == "__main__":
    input_image = Image.open("Sample-of-contrast-with-and-without.jpg")

    # Apply grayscale slicing
    sliced_image = grayscale_slice(input_image, 30, 40, 150, 180)
    sliced_image.save("sliced_image.png")

    # Apply adaptive contrast adjustment
    contrast_image = adaptive_contrast_adjust(input_image)
    contrast_image.save("contrast_image.png")
