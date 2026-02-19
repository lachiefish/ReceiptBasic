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
  int getNumFilesInDir(const String &dir);
  String getFileNameFromDir(const String &dir, int index);
};

extern Storage storage;