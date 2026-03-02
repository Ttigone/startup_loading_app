// Copyright 2026 startup_loading_app. All rights reserved.
// config_service.cpp — Implementation of ConfigService.

#include "config_service.h"

#include <chrono>
#include <thread>

namespace app {
namespace services {

// ---------------------------------------------------------------------------
bool ConfigService::Initialize() {
  // --- Replace this block with real config-file reading logic ---
  // e.g. read from QSettings, JSON file, INI file, registry, etc.
  //
  // Simulated 300 ms I/O delay to demonstrate the progress bar movement.
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  initialized_ = true;
  return true;
}

// ---------------------------------------------------------------------------
void ConfigService::Shutdown() {
  // Flush / save config back to disk if needed.
  initialized_ = false;
}

// ---------------------------------------------------------------------------
std::string ConfigService::GetValue(const std::string& key,
                                     const std::string& default_value) const {
  // Example stub. In a real implementation query the loaded config map.
  (void)key;
  return default_value;
}

}  // namespace services
}  // namespace app
