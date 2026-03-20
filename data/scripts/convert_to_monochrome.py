from pathlib import Path
from PIL import Image, ImageEnhance
import sys
import shutil
import time

THERMAL_WIDTH = 384  # QR204 thermal printer width in dots


def print_progress(current, total, skipped, start_time):
    elapsed = time.time() - start_time
    percent = current / total if total > 0 else 1
    rate = current / elapsed if elapsed > 0 else 0
    eta = (total - current) / rate if rate > 0 else 0

    eta_min, eta_sec = divmod(int(eta), 60)
    eta_str = f"{eta_min}m {eta_sec:02d}s" if eta_min > 0 else f"{eta_sec}s"

    converted = current - skipped
    term_width = shutil.get_terminal_size().columns
    stats = f" {current}/{total} | {percent:.0%} | ETA: {eta_str} | converted:{converted} skipped:{skipped}"
    bar_width = max(10, term_width - len(stats) - 2)
    filled = int(bar_width * percent)
    bar = "\u2588" * filled + "\u2591" * (bar_width - filled)

    sys.stdout.write(f"\r{bar}{stats}")
    sys.stdout.flush()


def convert_to_monochrome(source_dir, output_dir):
    if not source_dir.exists():
        print(f"Source directory not found: {source_dir}")
        print("Run download_creature_images.py first.")
        return

    # Gather all source images
    images = sorted(source_dir.rglob("*.jpg"))
    total = len(images)
    if total == 0:
        print("No .jpg files found in cards_fullres/")
        return

    print(
        f"Converting {total} images to {THERMAL_WIDTH}px wide monochrome RAW binary...\n"
    )

    converted = 0
    skipped = 0
    failed = 0
    start_time = time.time()

    for img_path in images:
        # Preserve cmc/filename structure: cards_fullres/3/Bear.jpg -> cards/3/Bear.bin
        relative = img_path.relative_to(source_dir)
        out_path = output_dir / relative.with_suffix(".bin")

        # if out_path.exists():
        #     skipped += 1
        #     print_progress(converted + skipped + failed, total, skipped, start_time)
        #     continue

        try:
            out_path.parent.mkdir(parents=True, exist_ok=True)

            with Image.open(img_path) as img:
                # Calculate height to maintain aspect ratio
                aspect = img.height / img.width
                new_height = int(THERMAL_WIDTH * aspect)

                # Resize with high-quality resampling
                img_resized = img.resize((THERMAL_WIDTH, new_height), Image.LANCZOS)

                # Convert to greyscale
                img_gray = img_resized.convert("L")

                # Enhance contrast for crisp, readable text
                enhancer = ImageEnhance.Contrast(img_gray)
                img_gray = enhancer.enhance(1.75)  # Contrast boost

                # Convert to 1-bit monochrome with fixed threshold
                img_mono = img_gray.convert("1")

                # Save as raw binary (1 bit per pixel, 8 pixels per byte)
                img_array = img_mono.tobytes()
                with open(out_path, "wb") as f:
                    f.write(img_array)

            converted += 1
        except Exception as e:
            sys.stdout.write(f"\r\033[K  Error converting {img_path.name}: {e}\n")
            failed += 1

        print_progress(converted + skipped + failed, total, skipped, start_time)

    elapsed = time.time() - start_time
    elapsed_min, elapsed_sec = divmod(int(elapsed), 60)
    print(f"\n\n=== Conversion Complete ({elapsed_min}m {elapsed_sec:02d}s) ===")
    print(f"  Converted: {converted}")
    print(f"  Skipped (already exists): {skipped}")
    print(f"  Failed: {failed}")


if __name__ == "__main__":
    script_dir = Path(__file__).parent
    data_dir = script_dir.parent  # /data

    # Convert creature cards
    source_dir = data_dir / "cards_fullres"
    output_dir = data_dir / "cards"
    print(f"Source: {source_dir}")
    print(f"Output: {output_dir}\n")
    convert_to_monochrome(source_dir, output_dir)

    # Convert tokens
    tokens_source = data_dir / "tokens_fullres"
    tokens_output = data_dir / "tokens"
    if tokens_source.exists():
        print(f"\nSource: {tokens_source}")
        print(f"Output: {tokens_output}\n")
        convert_to_monochrome(tokens_source, tokens_output)
    else:
        print(f"\nSkipping tokens (no {tokens_source} folder found)")
