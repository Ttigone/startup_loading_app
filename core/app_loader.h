#ifndef APP_LOADER_H
#define APP_LOADER_H

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

struct LoadProgress {
  int percent;
  std::string message;
};

class AppLoader {
 public:
  using ProgressCallback = std::function<void(LoadProgress)>;
  using FinishedCallback = std::function<void()>;
  using ErrorCallback = std::function<void(std::string /*service_name*/,
                                           std::string /*error_msg*/)>;

  AppLoader();
  ~AppLoader();

  AppLoader(const AppLoader&) = delete;
  AppLoader& operator=(const AppLoader&) = delete;

  // 注册加载服务, 所有权转移, 在 Start 前调用
  AppLoader& RegisterService(std::unique_ptr<IBackendService> service);

  // 设置回调, 在 Start 前调用
  AppLoader& OnProgress(ProgressCallback cb);
  AppLoader& OnFinished(FinishedCallback cb);
  AppLoader& OnError(ErrorCallback cb);

  // 只能调用一次
  void Start();

  // 等待加载完成, 供外部调用以阻塞等待加载完成, 也可在回调中处理逻辑,
  // 例如进入主界面 NOTE 会阻塞调用线程
  void WaitForCompletion();

  // 是否成功完成加载, 任务是否全部成功, 供外部调用以决定是否进入主界面
  bool IsSuccess() const;

  // 终止所有服务, 无论成功与否, 供外部调用以确保资源清理, exit 前调用
  void ShutdownAll();

 private:
  // 加载线程函数
  void RunLoaderThread();

  // 计算总权重, 用于计算整体进度百分比
  uint32_t ComputeTotalWeight() const;

  // 后端服务列表
  std::vector<std::unique_ptr<IBackendService>> services_;

  // 进度回调
  ProgressCallback progress_cb_;

  // 完成回调
  FinishedCallback finished_cb_;

  // 错误回调
  ErrorCallback error_cb_;

  // 工作线程
  std::thread loader_thread_;
  std::mutex mutex_;
  std::atomic<bool> success_{true};
};

}  // namespace core
}  // namespace app

#endif  // APP_LOADER_H