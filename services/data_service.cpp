// Copyright 2026 startup_loading_app. All rights reserved.
// data_service.cpp — Implementation of DataService.

#include "data_service.h"

#include <chrono>
#include <thread>

namespace app {
namespace services {

// ---------------------------------------------------------------------------
bool DataService::Initialize() {
  // --- Replace this block with real heavy initialization logic ---
  // e.g. open SQLite with sqlite3_open(), scan files, load lookup tables, etc.
  //
  // Simulated 900 ms heavy I/O (weight=3 → 3× wider progress slice).
  std::this_thread::sleep_for(std::chrono::milliseconds(900));

  record_count_ = 42000;  // placeholder
  initialized_  = true;
  return true;
}

// ---------------------------------------------------------------------------
void DataService::Shutdown() {
  // Close DB handle, flush write buffers, release memory, etc.
  record_count_ = 0;
  initialized_  = false;
}

}  // namespace services
}  // namespace app
