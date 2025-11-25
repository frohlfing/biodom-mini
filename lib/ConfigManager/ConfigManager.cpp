#include "ConfigManager.h"
#include <LittleFS.h>

ConfigManager::ConfigManager(const char* filename)
    : _filename(filename) {}
