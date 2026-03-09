#include "storage.h"
#include "config.h"

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

String Storage::getRandomCardPath(int cmc)
{
  return getRandomCardPath(String(cmc));
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

  Serial.print(F("[STORAGE] Randomly selected from index "));
  Serial.print(random_index);
  Serial.print(F(": "));
  Serial.println(selected_card_file_name);

  return true;
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

bool Storage::fileExists(const String &path)
{
  File file = SD_MMC.open(path);
  if (!file)
  {
    return false;
  }
  file.close();
  return true;
}

size_t Storage::readFileBytes(const String &path, uint8_t *&buffer)
{
  buffer = nullptr;

  File file = SD_MMC.open(path);
  if (!file)
  {
    Serial.print(F("[STORAGE] Failed to open file: "));
    Serial.println(path);
    return 0;
  }

  size_t file_size = file.size();
  if (file_size == 0)
  {
    Serial.print(F("[STORAGE] File is empty: "));
    Serial.println(path);
    file.close();
    return 0;
  }

  buffer = (uint8_t *)heap_caps_malloc(file_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!buffer)
  {
    buffer = (uint8_t *)malloc(file_size); // Fallback to regular heap if PSRAM unavailable
  }
  if (!buffer)
  {
    Serial.print(F("[STORAGE] Failed to allocate buffer of size: "));
    Serial.println(file_size);
    file.close();
    return 0;
  }

  size_t bytes_read = file.read(buffer, file_size);
  file.close();

  if (bytes_read != file_size)
  {
    Serial.print(F("[STORAGE] Read mismatch — expected "));
    Serial.print(file_size);
    Serial.print(F(" but got "));
    Serial.println(bytes_read);
    free(buffer);
    buffer = nullptr;
    return 0;
  }

  Serial.print(F("[STORAGE] Read "));
  Serial.print(bytes_read);
  Serial.print(F(" bytes from: "));
  Serial.println(path);
  return bytes_read;
}

String Storage::readFileAsString(const String &path)
{
  File file = SD_MMC.open(path);
  if (!file)
  {
    Serial.print(F("[STORAGE] Failed to open file: "));
    Serial.println(path);
    return "";
  }

  String content = file.readString();
  file.close();

  Serial.print(F("[STORAGE] Read string ("));
  Serial.print(content.length());
  Serial.print(F(" chars) from: "));
  Serial.println(path);
  return content;
}

uint64_t Storage::totalBytes()
{
  return SD_MMC.totalBytes();
}

uint64_t Storage::usedBytes()
{
  return SD_MMC.usedBytes();
}

fs::FS &Storage::getFS()
{
  return SD_MMC;
}