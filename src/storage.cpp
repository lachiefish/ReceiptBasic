#include "storage.h"
#include "config.h"

Storage storage;

void Storage::begin()
{
  SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN);
  if (!SD_MMC.begin(SD_MOUNT_PATH, SD_1_BIT_MODE, SD_FORMAT_IF_MOUNT_FAILED, SDMMC_FREQ_DEFAULT, SD_MAX_FILES_OPEN))
  {
    Serial.println(F("[STORAGE] SD card mount failed"));
    for (;;)
      ;
  }
  Serial.println(F("[STORAGE] SD card mounted"));

  File root = SD_MMC.open("/");
  if (root)
  {
    Serial.println(F("[STORAGE] Root directory contents:"));
    File entry;
    while ((entry = root.openNextFile()))
    {
      Serial.print(F("  "));
      Serial.println(entry.name());
      entry.close();
    }
    root.close();
  }
}

String Storage::getRandomCardPath(const String &cmc)
{
  String dir_path = "/cards/" + cmc;
  String selected_card_file_name = "";

  if (!getRandomIndexEntry(dir_path, selected_card_file_name))
  {
    Serial.print(F("[STORAGE] Failed to get random index entry for: "));
    Serial.println(dir_path);
    return "";
  }
  return dir_path + "/" + selected_card_file_name;
}

bool Storage::getRandomIndexEntry(const String &dir_path, String &selected_card_file_name)
{
  File index_file;
  if (!getIndexFile(index_file, dir_path))
  {
    return false;
  }

  int num_files = parseFileCount(index_file);

  if (num_files <= 0)
  {
    Serial.println(F("[STORAGE] Found zero files"));
    return false;
  }

  int random_index = random(0, num_files);

  for (int i = 0; i < random_index; i++) // Skip lines until 1 before the random index
  {
    index_file.readStringUntil('\n');
  }
  selected_card_file_name = index_file.readStringUntil('\n'); // Read the random index line

  index_file.close();
  selected_card_file_name.trim();

  if (selected_card_file_name == "")
  {
    Serial.println(F("[STORAGE] Selected file name is empty"));
    return false;
  }

  Serial.print(F("[STORAGE] From index "));
  Serial.print(random_index);
  Serial.print(F(": "));
  Serial.println(selected_card_file_name);

  return true;
}

String Storage::getRandomCardPath(int cmc)
{
  return getRandomCardPath(String(cmc));
}

bool Storage::getIndexFile(File &index_file, const String &dir_path)
{
  index_file = SD_MMC.open(dir_path + "/index.txt");
  if (!index_file)
  {
    Serial.print(F("[STORAGE] Failed to open index.txt in directory: "));
    Serial.println(dir_path);
    return false;
  }

  Serial.print(F("[STORAGE] Opened index.txt in directory: "));
  Serial.println(dir_path);
  return true;
}

int Storage::parseFileCount(File &index_file)
{
  String first_line = index_file.readStringUntil('\n');
  first_line.trim();

  if (first_line.length() == 0)
  {
    Serial.println(F("[STORAGE] index.txt is empty"));
    return 0;
  }

  for (unsigned int i = 0; i < first_line.length(); i++) // Ensure the first line read was an int
  {
    if (!isDigit(first_line.charAt(i)))
    {
      Serial.print(F("[STORAGE] Invalid first line number in index.txt: "));
      Serial.println(first_line);
      return 0;
    }
  }

  int num_files = first_line.toInt();
  Serial.print(F("[STORAGE] Number of files found: "));
  Serial.println(num_files);
  return num_files;
}