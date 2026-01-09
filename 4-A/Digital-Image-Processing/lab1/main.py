from PIL import Image
import numpy as np
from typing import Literal


def save_histogram(image: Image.Image, filename: str) -> None:
    """
    保存图像的灰度直方图。
    参数:
        image (Image.Image): 输入的灰度图像。
        filename (str): 输出文件名。
    """
    import matplotlib.pyplot as plt

    img_arr = np.array(image, dtype=np.uint8)
    plt.hist(img_arr.flatten(), bins=256, range=(0, 255), color="black")
    plt.title("Grayscale Histogram")
    plt.xlabel("Pixel Value")
    plt.ylabel("Frequency")
    plt.savefig(filename)
    plt.close()


def gray_transform(
    input_img: Image.Image,
    transform_type: Literal["reverse", "linear", "log", "gamma"],
    parameters: dict,
) -> Image.Image:
    """
    对输入的灰度图像进行不同类型的灰度变换。
    参数:
        input_img (Image.Image): 输入的灰度图像。
        transform_type (Literal["reverse", "linear", "log", "gamma"]): 变换类型，可选值为 "reverse"（反色变换）、"linear"（线性变换）、"log"（对数变换）、"gamma"（伽马变换）。
        parameters (dict): 变换所需的参数字典。对于不同的变换类型，所需参数如下:
            - 反色变换 ("reverse"): 无需参数。
            - 线性变换 ("linear"): 需要参数 "a", "b", "c", "d"。
            - 对数变换 ("log"): 需要参数 "C"。
            - 伽马变换 ("gamma"): 需要参数 "C", "gamma"。
    返回:
        Image.Image: 经过灰度变换后的图像。
    异常:
        ValueError: 当 transform_type 非法时抛出。
        AssertionError: 当所需参数缺失时抛出。
    """
    img_arr = np.array(input_img, dtype=np.uint8)
    if transform_type == "reverse":
        # 实施反色变换
        out = 255 - img_arr
        return Image.fromarray(out.astype(np.uint8))
    elif transform_type == "linear":
        # 实施线性变换
        assert "a" in parameters, "a parameter is required for linear transformation"
        assert "b" in parameters, "b parameter is required for linear transformation"
        assert "c" in parameters, "c parameter is required for linear transformation"
        assert "d" in parameters, "d parameter is required for linear transformation"
        assert 0 <= parameters["a"] <= parameters["b"] <= 255, "Invalid a and b values"
        assert 0 <= parameters["c"] <= parameters["d"] <= 255, "Invalid c and d values"
        a, b, c, d = parameters["a"], parameters["b"], parameters["c"], parameters["d"]
        out = np.zeros_like(img_arr, dtype=np.float32)
        mask1 = img_arr <= a
        mask2 = (img_arr > a) & (img_arr <= b)
        mask3 = img_arr > b
        if a == 0:
            out[mask1] = c  # 避免除以零
        else:
            out[mask1] = (c / a) * img_arr[mask1]
        if a == b:
            out[mask2] = c  # 避免除以零
        else:
            out[mask2] = ((d - c) / (b - a)) * (img_arr[mask2] - a) + c
        if b == 255:
            out[mask3] = d  # 避免除以零
        else:
            out[mask3] = ((255 - d) / (255 - b)) * (img_arr[mask3] - b) + d
        return Image.fromarray(out.astype(np.uint8))
    elif transform_type == "log":
        # 实施对数变换
        assert "C" in parameters, "C parameter is required for log transformation"
        C = parameters["C"]
        out = C * np.log1p(img_arr / 255) * 255  # log1p计算log(1 + x)
        out = np.clip(out, 0, 255)  # 确保像素值在0-255范围内
        return Image.fromarray(out.astype(np.uint8))
    elif transform_type == "gamma":
        # 实施伽马变换
        assert "C" in parameters, "C parameter is required for gamma transformation"
        assert (
            "gamma" in parameters
        ), "Gamma parameter is required for gamma transformation"
        C = parameters["C"]
        gamma = parameters["gamma"]
        out = C * 255 * (img_arr / 255) ** gamma
        out = np.clip(out, 0, 255)  # 确保像素值在0-255范围内
        return Image.fromarray(out.astype(np.uint8))
    else:
        raise ValueError(f"Unknown transform type: {transform_type}")


if __name__ == "__main__":
    # Example usage
    input_image_path = "images/lena.bmp"

    # Open the input image
    img = Image.open(input_image_path)

    save_histogram(img, "original_histogram.png")
    print("Histogram of original image saved as original_histogram.png")

    # 反色
    reverse_img = gray_transform(img, "reverse", {})
    reverse_img.save("reverse.png")
    print("Reverse transformation applied and saved as reverse.png")

    save_histogram(reverse_img, "reverse_histogram.png")
    print("Histogram of reverse transformed image saved as reverse_histogram.png")

    # 线性
    linear_img = gray_transform(img, "linear", {"a": 50, "b": 180, "c": 0, "d": 255})
    linear_img.save("linear.png")
    print("Linear transformation applied and saved as linear.png")

    save_histogram(linear_img, "linear_histogram.png")
    print("Histogram of linear transformed image saved as linear_histogram.png")

    # 对数
    log_img = gray_transform(img, "log", {"C": 1 / np.log(2)})
    log_img.save("log.png")
    print("Log transformation applied and saved as log.png")

    save_histogram(log_img, "log_histogram.png")
    print("Histogram of log transformed image saved as log_histogram.png")

    # 伽马
    gamma_img = gray_transform(img, "gamma", {"C": 1, "gamma": 0.4})
    gamma_img.save("gamma0.4.png")
    print("Gamma transformation applied and saved as gamma0.4.png")

    save_histogram(gamma_img, "gamma0.4_histogram.png")
    print("Histogram of gamma transformed image saved as gamma0.4_histogram.png")

    gamma_img2 = gray_transform(img, "gamma", {"C": 1, "gamma": 2.5})
    gamma_img2.save("gamma2.5.png")
    print("Gamma transformation applied and saved as gamma2.5.png")

    save_histogram(gamma_img2, "gamma2.5_histogram.png")
    print("Histogram of gamma transformed image saved as gamma2.5_histogram.png")
