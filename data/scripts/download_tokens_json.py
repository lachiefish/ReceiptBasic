import requests
import json
import time
import sys
import shutil
from pathlib import Path


def print_progress(cards_so_far, total_cards, page, start_time):
    elapsed = time.time() - start_time
    if total_cards > 0:
        percent = cards_so_far / total_cards
        rate = cards_so_far / elapsed if elapsed > 0 else 0
        eta = (total_cards - cards_so_far) / rate if rate > 0 else 0
        eta_min, eta_sec = divmod(int(eta), 60)
        eta_str = f"{eta_min}m {eta_sec:02d}s" if eta_min > 0 else f"{eta_sec}s"
    else:
        percent = 0
        eta_str = "?"

    term_width = shutil.get_terminal_size().columns
    stats = (
        f" {cards_so_far}/{total_cards} | {percent:.0%} | Page {page} | ETA: {eta_str}"
    )
    bar_width = max(10, term_width - len(stats) - 2)
    filled = int(bar_width * percent)
    bar = "\u2588" * filled + "\u2591" * (bar_width - filled)

    sys.stdout.write(f"\r{bar}{stats}")
    sys.stdout.flush()


def download_tokens_json(target_folder):
    base_url = "https://api.scryfall.com/cards/search"
    params = {
        "q": "t:token in:paper",
        "unique": "cards",
        "order": "name",
    }

    all_tokens = []
    seen_names = {}  # Track name -> count to handle duplicates
    page = 1
    cards_processed = 0
    total_cards = 0
    start_time = time.time()

    print("Downloading token cards from Scryfall API...\n")

    while True:
        response = requests.get(base_url, params=params)

        if response.status_code != 200:
            print(f"Error fetching data: {response.status_code}")
            break

        data = response.json()

        if page == 1:
            total_cards = data.get("total_cards", 0)

        for card in data.get("data", []):
            cards_processed += 1
            print_progress(cards_processed, total_cards, page, start_time)

            image_url = None
            if "image_uris" in card:
                image_url = card["image_uris"]["normal"]
            elif "card_faces" in card:
                image_url = card["card_faces"][0].get("image_uris", {}).get("normal")
            if not image_url:
                continue

            name = card["name"]
            set_name = card.get("set_name", "")
            display_name = f"{name} [{set_name}]" if set_name else name

            # Only number duplicates that share the same name AND set
            if display_name in seen_names:
                seen_names[display_name] += 1
                unique_name = f"{display_name} ({seen_names[display_name]})"
            else:
                seen_names[display_name] = 1
                unique_name = display_name

            token_info = {
                "name": unique_name,
                "image_url": image_url,
            }

            all_tokens.append(token_info)

        if not data.get("has_more", False):
            break

        if "next_page" in data:
            base_url = data["next_page"]
            params = {}
            page += 1
            time.sleep(0.1)  # Be nice to Scryfall's API
        else:
            break

    with open(target_folder / "tokens.json", "w") as f:
        json.dump(all_tokens, f, indent=2)

    elapsed = time.time() - start_time
    elapsed_min, elapsed_sec = divmod(int(elapsed), 60)
    print(
        f"\n\n=== Downloaded {len(all_tokens)} tokens ({elapsed_min}m {elapsed_sec:02d}s) ==="
    )


if __name__ == "__main__":
    script_dir = Path(__file__).parent
    target_folder = script_dir
    print(f"Saving tokens.json to {target_folder / 'tokens.json'}")
    download_tokens_json(target_folder)
