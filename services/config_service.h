// Copyright 2026 startup_loading_app. All rights reserved.
// config_service.h — Example backend service: loads application configuration.
//
// Replace the body of Initialize() with real file-read / JSON-parse / INI-load
// logic. The framework does not care what happens inside — only the bool return
// value and the weight matter.

#pragma once

#include <string>
#include "../core/backend_service.h"

namespace app {
namespace services {

class ConfigService : public core::IBackendService {
 public:
  ConfigService() = default;
  ~ConfigService() override = default;

  std::string GetName() const override { return "配置服务"; }

  // Config loading is fast → weight 1 (default).
  uint32_t GetWeight() const override { return 1u; }

  bool Initialize() override;
  void Shutdown() override;

  // --- Public API available after Initialize() returns true ---------------
  std::string GetValue(const std::string& key,
                       const std::string& default_value = "") const;

 private:
  // Simulated config store.
  // Replace with std::unordered_map<std::string, std::string> loaded from file.
  bool initialized_{false};
};

}  // namespace services
}  // namespace app
