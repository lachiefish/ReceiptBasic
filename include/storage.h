#pragma once
#include <Arduino.h>
#include <SD_MMC.h>

class Storage
{
public:
  void begin();
  String getRandomCardPath(const String &cmc);
  String getRandomCardPath(int cmc);
  bool fileExists(const String &path);
  size_t readFileBytes(const String &path, uint8_t *&buffer);
  String readFileAsString(const String &path);
  uint64_t totalBytes();
  uint64_t usedBytes();
  fs::FS &getFS();

private:
  bool getIndexFile(File &index_file, const String &dir);
  bool getRandomIndexEntry(const String &dir_path, String &selected_card_file_name);
  int parseFileCount(File &index_file);
};