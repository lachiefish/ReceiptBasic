from pathlib import Path
import requests
import json
import time
import re
import sys
import shutil


def sanitize_filename(name):
    name = re.sub(r'[<>:"/\\|?*]', "", name)
    name = name.strip(". ")
    return name


def print_progress(current, total, downloaded, skipped, failed, start_time):
    elapsed = time.time() - start_time
    percent = current / total if total > 0 else 1
    rate = current / elapsed if elapsed > 0 else 0
    eta = (total - current) / rate if rate > 0 else 0

    eta_min, eta_sec = divmod(int(eta), 60)
    eta_str = f"{eta_min}m {eta_sec:02d}s" if eta_min > 0 else f"{eta_sec}s"

    term_width = shutil.get_terminal_size().columns
    stats = f" {current}/{total} | {percent:.0%} | ETA: {eta_str} | \u2193{downloaded} \u2192{skipped} \u2717{failed}"
    bar_width = max(10, term_width - len(stats) - 2)  # 2 for brackets
    filled = int(bar_width * percent)
    bar = "\u2588" * filled + "\u2591" * (bar_width - filled)

    sys.stdout.write(f"\r{bar}{stats}")
    sys.stdout.flush()


def download_creature_images(target_folder):
    creatures_file = target_folder / "creatures.json"
    if not creatures_file.exists():
        print(f"creatures.json not found at {creatures_file}")
        print("Run download_creatures_json.py first.")
        return

    with open(creatures_file, "r") as f:
        all_creatures = json.load(f)

    cards_dir = target_folder.parent / "cards_fullres"
    total = sum(len(cards) for cards in all_creatures.values())
    downloaded = 0
    skipped = 0
    failed = 0

    print(f"Downloading {total} creature images...\n")
    start_time = time.time()

    for cmc, cards in sorted(all_creatures.items(), key=lambda x: int(x[0])):
        cmc_dir = cards_dir / str(cmc)
        cmc_dir.mkdir(parents=True, exist_ok=True)

        for card in cards:
            name = sanitize_filename(card["name"])
            image_path = cmc_dir / f"{name}.jpg"

            if image_path.exists():
                skipped += 1
                print_progress(
                    downloaded + skipped + failed,
                    total,
                    downloaded,
                    skipped,
                    failed,
                    start_time,
                )
                continue

            try:
                response = requests.get(card["image_url"], timeout=30)
                if response.status_code == 200:
                    with open(image_path, "wb") as img_file:
                        img_file.write(response.content)
                    downloaded += 1
                else:
                    sys.stdout.write(
                        f"\r\033[K  Failed ({response.status_code}): {card['name']}\n"
                    )
                    failed += 1
            except requests.RequestException as e:
                sys.stdout.write(f"\r\033[K  Error downloading {card['name']}: {e}\n")
                failed += 1

            print_progress(
                downloaded + skipped + failed,
                total,
                downloaded,
                skipped,
                failed,
                start_time,
            )
            time.sleep(0.1)  # Be nice to Scryfall's API

    elapsed = time.time() - start_time
    elapsed_min, elapsed_sec = divmod(int(elapsed), 60)
    print(f"\n\n=== Download Complete ({elapsed_min}m {elapsed_sec:02d}s) ===")
    print(f"  Downloaded: {downloaded}")
    print(f"  Skipped (already exists): {skipped}")
    print(f"  Failed: {failed}")


if __name__ == "__main__":
    script_dir = Path(__file__).parent
    target_folder = script_dir
    print(f"Downloading images to {target_folder.parent / 'cards_fullres'}")
    download_creature_images(target_folder)
