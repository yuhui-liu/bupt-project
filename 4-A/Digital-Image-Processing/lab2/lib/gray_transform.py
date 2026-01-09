from PIL import Image
import numpy as np
from typing import Literal


def gray_transform(
    img_path: str,
    transform_type: Literal["reverse", "linear", "log", "gamma"],
    parameters: dict,
) -> Image.Image:
    """
    对输入的灰度图像进行不同类型的灰度变换。
    参数:
        img_path (str): 输入的灰度图像路径。
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
    input_img = Image.open(img_path)#.convert("L")
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
