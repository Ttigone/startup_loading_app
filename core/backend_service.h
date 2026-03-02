// Copyright 2026 startup_loading_app. All rights reserved.
// backend_service.h — Abstract interface for all backend services.
//
// Design pattern:
//   - Strategy / Template Method: each concrete service implements Initialize()
//   - The AppLoader uses this interface to drive progress-aware loading
//
// Coding style: Google C++ Style Guide
//   - Member functions: UpperCamelCase
//   - Member variables: lower_with_trailing_underscore_
//   - Local / param:    lower_with_underscore (no trailing)

#pragma once

#include <cstdint>
#include <string>

namespace app {
namespace core {

// ---------------------------------------------------------------------------
// IBackendService
//   Pure-abstract interface every backend service must implement.
//   Designed to be owned through std::unique_ptr<IBackendService>.
// ---------------------------------------------------------------------------
class IBackendService {
 public:
  virtual ~IBackendService() = default;

  // Human-readable name shown in the splash screen status label.
  virtual std::string GetName() const = 0;

  // Relative weight used to distribute progress-bar increments.
  // A heavier service gets a proportionally larger slice of [0, 100].
  // Default weight is 1.  Override for expensive services.
  virtual uint32_t GetWeight() const { return 1u; }

  // Blocking initialization called on the loader thread (NOT the GUI thread).
  // Return true on success, false on failure.
  // On failure the AppLoader will abort and propagate the error.
  virtual bool Initialize() = 0;

  // Optional teardown called when the application shuts down.
  // Called on the GUI thread after QApplication::exec() returns.
  virtual void Shutdown() {}

 protected:
  IBackendService() = default;

  // Non-copyable, non-movable (unique resource ownership model).
  IBackendService(const IBackendService&) = delete;
  IBackendService& operator=(const IBackendService&) = delete;
};

}  // namespace core
}  // namespace app
