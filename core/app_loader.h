// Copyright 2026 startup_loading_app. All rights reserved.
// app_loader.h — High-performance asynchronous backend loader.
//
// Design patterns used:
//   - Observer  : progress / status / error signals propagated to Qt GUI
//   - Builder   : fluent RegisterService() chain
//   - RAII      : worker thread joined in destructor
//   - Singleton guard: NOT a singleton — instantiate once in main().
//
// Thread model:
//   - GUI thread  : owns AppLoader, SplashScreen, MainWindow
//   - Loader thread: one std::thread executing all IBackendService::Initialize()
//                    sequentially (or in parallel if parallel_mode is enabled)
//
// Coding style: Google C++ Style Guide

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "backend_service.h"

namespace app {
namespace core {

// ---------------------------------------------------------------------------
// LoadProgress  — snapshot emitted after each service completes
// ---------------------------------------------------------------------------
struct LoadProgress {
  int     percent;     // 0-100
  std::string message; // human-readable status text
};

// ---------------------------------------------------------------------------
// AppLoader
//   Register backend services with RegisterService(), then call Start().
//   Communicate results back to the Qt GUI layer via std::function callbacks
//   (bridged to Qt signals inside SplashScreen).
// ---------------------------------------------------------------------------
class AppLoader {
 public:
  // Callback types — called on the loader thread; post to Qt GUI thread via
  // QMetaObject::invokeMethod with Qt::QueuedConnection.
  using ProgressCallback = std::function<void(LoadProgress)>;
  using FinishedCallback = std::function<void()>;
  using ErrorCallback    = std::function<void(std::string /*service_name*/,
                                               std::string /*error_msg*/)>;

  AppLoader();
  ~AppLoader();

  // Non-copyable / non-movable.
  AppLoader(const AppLoader&) = delete;
  AppLoader& operator=(const AppLoader&) = delete;

  // ---- Fluent builder interface ------------------------------------------

  // Register a service to be loaded.  Ownership is transferred.
  // Call before Start().  Returns *this for chaining.
  AppLoader& RegisterService(std::unique_ptr<IBackendService> service);

  // Set callbacks (call before Start()).
  AppLoader& OnProgress(ProgressCallback cb);
  AppLoader& OnFinished(FinishedCallback cb);
  AppLoader& OnError(ErrorCallback cb);

  // ---- Lifecycle ---------------------------------------------------------

  // Launch the loader thread.  Must be called once.
  // pre-condition: at least one service registered, QApplication running.
  void Start();

  // Wait for completion (blocks calling thread).
  void WaitForCompletion();

  // Returns true if all services loaded successfully.
  bool IsSuccess() const;

  // Shutdown all services (call before QApplication exits).
  void ShutdownAll();

 private:
  // Worker function executed on loader_thread_.
  void RunLoaderThread();

  // Compute the total weight sum of all registered services.
  uint32_t ComputeTotalWeight() const;

  // Registered services — immutable after Start().
  std::vector<std::unique_ptr<IBackendService>> services_;

  // Callbacks.
  ProgressCallback progress_cb_;
  FinishedCallback finished_cb_;
  ErrorCallback    error_cb_;

  // Worker thread and sync primitives.
  std::thread  loader_thread_;
  std::mutex   mutex_;
  std::atomic<bool> success_{true};
};

}  // namespace core
}  // namespace app
