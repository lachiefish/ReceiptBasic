#include "storage.h"
#include "config.h"

Storage storage;

void Storage::begin()
{
  SD_MMC.setPins(SD_CLK_PIN, SD_CMD_PIN, SD_D0_PIN);
  if (!SD_MMC.begin(SD_MOUNT_PATH, SD_1_BIT_MODE, SD_FORMAT_IF_MOUNT_FAILED, SDMMC_FREQ_DEFAULT, SD_MAX_FILES_OPEN))
  {
    Serial.println(F("[STORAGE] SD card mount failed"));
    return;
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

  int num_files = getNumFilesInDir(dir_path);

  if (num_files == 0)
  {
    Serial.print(F("[STORAGE] No files found in: "));
    Serial.println(dir_path);
    return "";
  }

  int random_index = random(0, num_files);
  String selected_file = getFileNameFromDir(dir_path, random_index);

  if (selected_file.length() == 0)
  {
    Serial.println(F("[STORAGE] Failed to get filename"));
    return "";
  }

  Serial.print(F("[STORAGE] Random card selected: "));
  Serial.println(selected_file);
  return dir_path + "/" + selected_file;
}

String Storage::getRandomCardPath(int cmc)
{
  return getRandomCardPath(String(cmc));
}

int Storage::getNumFilesInDir(const String &dir_path)
{
  File index_file;
  if (!getIndexFile(index_file, dir_path))
  {
    return 0;
  }

  String first_line = index_file.readStringUntil('\n');
  index_file.close();

  first_line.trim();
  if (first_line.length() == 0)
  {
    Serial.println(F("[STORAGE] index.txt is empty"));
    return 0;
  }
  for (int i = 0; i < first_line.length(); i++) // Ensure the first line read was an int
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

String Storage::getFileNameFromDir(const String &dir_path, int index)
{
  File index_file;
  if (!getIndexFile(index_file, dir_path))
  {
    return "";
  }

  index_file.readStringUntil('\n'); // Skip first line (file count)

  String line = "";
  for (int i = 0; i <= index; i++)
  {
    line = index_file.readStringUntil('\n');
  }

  index_file.close();
  line.trim();

  Serial.print(F("[STORAGE] From index "));
  Serial.print(index);
  Serial.print(F(": "));
  Serial.println(line);

  return line;
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