// Copyright 2026 startup_loading_app. All rights reserved.
// data_service.h — Example backend service: heavy data / DB initialisation.
//
// Represents an expensive service (weight = 3) such as:
//   - Loading a large local SQLite database
//   - Pre-computing lookup tables
//   - Reading large binary asset files
//   - Establishing network connections / caches

#pragma once

#include <cstdint>
#include <string>
#include "../core/backend_service.h"

namespace app {
namespace services {

class DataService : public core::IBackendService {
 public:
  DataService() = default;
  ~DataService() override = default;

  std::string GetName() const override { return "数据服务"; }

  // Heavier service: allocates more of the progress bar.
  uint32_t GetWeight() const override { return 3u; }

  bool Initialize() override;
  void Shutdown() override;

  // --- Public API available after Initialize() returns true ---------------
  // Add your real data-access methods here.
  uint64_t GetRecordCount() const { return record_count_; }

 private:
  bool     initialized_{false};
  uint64_t record_count_{0};
};

}  // namespace services
}  // namespace app
