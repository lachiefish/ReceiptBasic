#pragma once
#include <Arduino.h>
#include <SD_MMC.h>

class Storage
{
public:
  void begin();
  String getRandomCardPath(const String &cmc);
  String getRandomCardPath(int cmc);

private:
  bool getIndexFile(File &index_file, const String &dir);
  bool getRandomIndexEntry(const String &dir_path, String &selected_card_file_name);
  int parseFileCount(File &index_file);
};

extern Storage storage;