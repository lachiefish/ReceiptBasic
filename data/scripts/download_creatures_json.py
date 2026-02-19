import requests
import json
import time
import os
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


def download_creatures_json(target_folder):
    base_url = "https://api.scryfall.com/cards/search"
    params = {
        "q": r"t:/^[^\/]*Creature/ -is:unset in:paper",  # Add in:mtgo for MTGO only cards
        "unique": "cards",
        "order": "cmc",
    }

    all_creatures = {}
    page = 1
    cards_processed = 0
    total_cards = 0
    start_time = time.time()

    print("Downloading creature cards from Scryfall API...\n")

    while True:
        response = requests.get(base_url, params=params)

        if response.status_code != 200:
            print(f"Error fetching data: {response.status_code}")
            break

        data = response.json()

        if page == 1:
            total_cards = data.get("total_cards", 0)

        for card in data.get("data", []):  # If no data key, default to empty list
            cards_processed += 1
            print_progress(cards_processed, total_cards, page, start_time)
            image_url = None
            if "image_uris" in card:  # Single-faced: Grizzly Bears
                image_url = card["image_uris"]["normal"]
            elif (
                "card_faces" in card
            ):  # Double-faced: Delver of Secrets // Insectile Aberration
                image_url = card["card_faces"][0]["image_uris"]["normal"]
            if not image_url:
                continue

            if "Creature" not in card.get("type_line", ""):
                continue  # Skip non-creature cards

            cmc = int(card.get("cmc", 0))

            if cmc not in all_creatures:
                all_creatures[cmc] = []

            card_info = {
                "name": card["name"],
                "cmc": cmc,
                #'type': card['type_line'],
                #'power': card.get('power', ''),
                #'toughness': card.get('toughness', ''), # Add these if you want to include them in the JSON
                "image_url": image_url,
            }

            all_creatures[cmc].append(card_info)

        if not data.get("has_more", False):
            break  # No more pages to fetch

        # break  # For testing, only fetch the first page. Remove this line to fetch all pages.
        if "next_page" in data:
            base_url = data["next_page"]
            params = {}  # Next page URL includes all params already, so clear them out
            page += 1
            time.sleep(0.1)  # Be nice to Scryfall's API
        else:
            break  # End of while loop

    os.makedirs("data", exist_ok=True)  # Save to data/creatures.json
    with open(target_folder / "creatures.json", "w") as f:
        json.dump(all_creatures, f, indent=2)

    # Print statistics
    elapsed = time.time() - start_time
    elapsed_min, elapsed_sec = divmod(int(elapsed), 60)
    total = sum(len(cards) for cards in all_creatures.values())
    print(
        f"\n\n=== Downloaded {total} creatures ({elapsed_min}m {elapsed_sec:02d}s) ==="
    )
    for cmc in sorted(all_creatures.keys()):
        print(f"CMC {cmc}: {len(all_creatures[cmc])} cards")


if __name__ == "__main__":
    script_dir = Path(__file__).parent
    target_folder = script_dir
    print(f"Saving creatures.json to {target_folder / 'creatures.json'}")
    download_creatures_json(target_folder)
