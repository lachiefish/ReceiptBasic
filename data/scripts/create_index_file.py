import os
from pathlib import Path


def create_index_for_folder(folder, output_file="index.txt"):
    """
    Create an index.txt file for a single folder listing only its direct files.
    """
    # Get only direct files in this folder (not subdirectories)
    files = [f for f in folder.iterdir() if f.is_file() and f.name != output_file]
    total_count = len(files)

    # Write to output file
    output_path = folder / output_file
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(f"{total_count}\n")
        for file in sorted(files):
            f.write(f"{file.name}\n")

    print(f"Created {output_path} with {total_count} files.")


def list_files_in_folder(folder_path, output_file="index.txt"):
    """
    Create index.txt files in folder and all subfolders.

    Args:
        folder_path: Path to the folder to scan
        output_file: Name of the output text file
    """
    folder = Path(folder_path)

    if not folder.exists():
        print(f"Error: Folder '{folder_path}' does not exist.")
        return

    # Create index for root folder
    create_index_for_folder(folder, output_file)

    # Create index for each subfolder
    for subfolder in folder.rglob("*"):
        if subfolder.is_dir():
            create_index_for_folder(subfolder, output_file)

    print(f"Done! Created index files in all folders.")


if __name__ == "__main__":
    # Target the ../cards folder relative to script location
    script_dir = Path(__file__).parent
    target_folder = script_dir / "../cards"
    list_files_in_folder(target_folder)
