// Copyright 2026 startup_loading_app. All rights reserved.
// app_loader.cpp — Implementation of AppLoader.

#include "app_loader.h"

#include <cassert>
#include <stdexcept>

namespace app {
namespace core {

// ---------------------------------------------------------------------------
AppLoader::AppLoader() = default;

AppLoader::~AppLoader() {
  WaitForCompletion();
}

// ---------------------------------------------------------------------------
AppLoader& AppLoader::RegisterService(std::unique_ptr<IBackendService> service) {
  assert(service && "RegisterService: null service pointer");
  std::lock_guard<std::mutex> lock(mutex_);
  services_.push_back(std::move(service));
  return *this;
}

AppLoader& AppLoader::OnProgress(ProgressCallback cb) {
  progress_cb_ = std::move(cb);
  return *this;
}

AppLoader& AppLoader::OnFinished(FinishedCallback cb) {
  finished_cb_ = std::move(cb);
  return *this;
}

AppLoader& AppLoader::OnError(ErrorCallback cb) {
  error_cb_ = std::move(cb);
  return *this;
}

// ---------------------------------------------------------------------------
void AppLoader::Start() {
  assert(!loader_thread_.joinable() && "AppLoader::Start called more than once");
  loader_thread_ = std::thread(&AppLoader::RunLoaderThread, this);
}

// ---------------------------------------------------------------------------
void AppLoader::WaitForCompletion() {
  if (loader_thread_.joinable()) {
    loader_thread_.join();
  }
}

// ---------------------------------------------------------------------------
bool AppLoader::IsSuccess() const {
  return success_.load(std::memory_order_acquire);
}

// ---------------------------------------------------------------------------
void AppLoader::ShutdownAll() {
  // Called on the GUI thread after QApplication::exec() returns.
  // Iterate in reverse so that services shut down in opposite registration order.
  for (auto it = services_.rbegin(); it != services_.rend(); ++it) {
    (*it)->Shutdown();
  }
}

// ---------------------------------------------------------------------------
uint32_t AppLoader::ComputeTotalWeight() const {
  uint32_t total = 0u;
  for (const auto& svc : services_) {
    total += svc->GetWeight();
  }
  return total == 0u ? 1u : total;
}

// ---------------------------------------------------------------------------
void AppLoader::RunLoaderThread() {
  const uint32_t total_weight = ComputeTotalWeight();
  uint32_t accumulated_weight = 0u;

  const auto emit_progress = [&](const std::string& msg) {
    if (!progress_cb_) return;
    // accumulated_weight already updated before this call
    const int pct = static_cast<int>(
        (static_cast<double>(accumulated_weight) / total_weight) * 100.0);
    LoadProgress prog;
    prog.percent = pct;
    prog.message = msg;
    progress_cb_(prog);
  };

  // Emit "starting" 0%
  emit_progress("正在初始化...");

  for (auto& svc : services_) {
    const std::string svc_name = svc->GetName();

    // Emit "starting this service" progress (before init)
    {
      LoadProgress prog;
      prog.percent = static_cast<int>(
          (static_cast<double>(accumulated_weight) / total_weight) * 100.0);
      prog.message = "正在加载: " + svc_name;
      if (progress_cb_) progress_cb_(prog);
    }

    bool ok = false;
    try {
      ok = svc->Initialize();
    } catch (const std::exception& ex) {
      ok = false;
      if (error_cb_) error_cb_(svc_name, ex.what());
    } catch (...) {
      ok = false;
      if (error_cb_) error_cb_(svc_name, "未知异常");
    }

    if (!ok) {
      success_.store(false, std::memory_order_release);
      if (error_cb_) {
        error_cb_(svc_name, "初始化返回失败");
      }
      // Stop loading on first failure.
      return;
    }

    accumulated_weight += svc->GetWeight();
    emit_progress(svc_name + " 加载完成");
  }

  // Guarantee 100% on success.
  {
    LoadProgress prog;
    prog.percent = 100;
    prog.message = "加载完成，正在进入主界面...";
    if (progress_cb_) progress_cb_(prog);
  }

  if (finished_cb_) finished_cb_();
}

}  // namespace core
}  // namespace app
