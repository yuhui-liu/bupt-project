from pathlib import Path
from typing import Tuple

import cv2
import matplotlib.pyplot as plt
import numpy as np


DATA_DIR = Path(".")
INPUT_IMAGE = DATA_DIR / "bone.jpg"
OUTPUT_DIR = DATA_DIR / "outputs"


def ensure_output_dir() -> None:
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)


def read_grayscale(path: Path) -> np.ndarray:
    img = cv2.imread(str(path), cv2.IMREAD_GRAYSCALE)
    if img is None:
        raise FileNotFoundError(f"Could not read image at {path}")
    return img


def clahe_contrast(
    img: np.ndarray, clip_limit: float = 2.0, tile_grid_size: Tuple[int, int] = (8, 8)
) -> np.ndarray:
    clahe = cv2.createCLAHE(clipLimit=clip_limit, tileGridSize=tile_grid_size)
    return clahe.apply(img)


def denoise(img: np.ndarray, method: str = "bilateral") -> np.ndarray:
    if method == "gaussian":
        # 3x3 Gaussian filter, sigmaX chosen automatically
        return cv2.GaussianBlur(img, (0, 0), 0.5)
    if method == "median":
        return cv2.medianBlur(img, 1)
    raise ValueError(f"Unknown denoising method: {method}")


def unsharp_mask(
    img: np.ndarray,
    k: float = 1.0,
    kernel_size: Tuple[int, int] = (5, 5),
    sigma: float = 1.0,
) -> np.ndarray:
    blurred = cv2.GaussianBlur(img, kernel_size, sigma)
    mask = cv2.subtract(img, blurred)
    sharpened = cv2.addWeighted(img, 1.0, mask, k, 0)
    return np.clip(sharpened, 0, 255).astype(np.uint8)


def laplacian_sharpen(img: np.ndarray, alpha: float = 0.5) -> np.ndarray:
    lap = cv2.Laplacian(img, cv2.CV_16S, ksize=3)
    lap = cv2.convertScaleAbs(lap)
    sharpened = cv2.addWeighted(img, 1.0, lap, alpha, 0)
    return np.clip(sharpened, 0, 255).astype(np.uint8)


def save_image(name: str, img: np.ndarray) -> None:
    ensure_output_dir()
    out_path = OUTPUT_DIR / f"{name}.png"
    cv2.imwrite(str(out_path), img)


def save_comparison_figure(
    name: str, images: Tuple[np.ndarray, ...], titles: Tuple[str, ...]
) -> None:
    ensure_output_dir()
    n = len(images)
    plt.figure(figsize=(4 * n, 4))
    for i, (im, t) in enumerate(zip(images, titles), start=1):
        plt.subplot(1, n, i)
        plt.imshow(im, cmap="gray")
        plt.title(t)
        plt.axis("off")
    plt.tight_layout()
    out_path = OUTPUT_DIR / f"{name}.png"
    plt.savefig(out_path, dpi=200)
    plt.close()


def run_pipelines() -> None:
    img = read_grayscale(INPUT_IMAGE)
    save_image("0_original", img)

    # Step 1: denoising on the original image
    den_gauss = denoise(img, method="gaussian")
    den_median = denoise(img, method="median")
    save_comparison_figure(
        "1_denoise_variants",
        (img, den_gauss, den_median),
        ("Original", "Gaussian", "Median"),
    )

    base_denoised = den_median
    save_image("1b_denoised_base", base_denoised)

    # Step 2: contrast enhancement (global hist eq + CLAHE) on denoised image
    hist_eq = cv2.equalizeHist(base_denoised)
    clahe_img = clahe_contrast(base_denoised, clip_limit=2.0, tile_grid_size=(8, 8))
    clahe_stronger = clahe_contrast(
        base_denoised, clip_limit=3.0, tile_grid_size=(8, 8)
    )
    save_comparison_figure(
        "2_contrast_variants",
        (base_denoised, hist_eq, clahe_img, clahe_stronger),
        ("Denoised", "HistEq", "CLAHE 2.0", "CLAHE 3.0"),
    )

    # Choose CLAHE (clip_limit=2.0) as base for further processing
    base_contrast = clahe_img
    save_image("2b_contrast_base", base_contrast)

    # Step 3: sharpening / edge enhancement variants on contrast-enhanced image
    sharp_usm_soft = unsharp_mask(base_contrast, k=0.7, kernel_size=(5, 5), sigma=1.0)
    sharp_usm_strong = unsharp_mask(base_contrast, k=1.2, kernel_size=(5, 5), sigma=1.0)
    sharp_lap = laplacian_sharpen(base_contrast, alpha=0.7)
    save_comparison_figure(
        "3_sharpen_variants",
        (base_contrast, sharp_usm_soft, sharp_usm_strong, sharp_lap),
        ("Contrast base", "USM k=0.7", "USM k=1.2", "Laplacian"),
    )

    # Final choice: slightly conservative unsharp mask to avoid amplifying noise too much
    final_img = sharp_usm_soft
    save_image("4_final", final_img)


def main():
    run_pipelines()


if __name__ == "__main__":
    main()
