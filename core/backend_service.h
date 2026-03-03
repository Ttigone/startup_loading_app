#ifndef BACKEND_SERVICE_H
#define BACKEND_SERVICE_H

#include <cstdint>
#include <string>

namespace app {
namespace core {

class IBackendService {
 public:
  virtual ~IBackendService() = default;

  // 显示名称信息
  virtual std::string GetName() const = 0;

  // 权重, 用于计算整体进度百分比, 默认为 1
  virtual uint32_t GetWeight() const { return 1u; }

  // 在加载线程上调用的阻塞初始化函数, 不是 GUI 线程. 成功返回 true, 失败返回 false.
  virtual bool Initialize() = 0;

  // 在 GUI 线程 exec 调用后被调用
  virtual void Shutdown() {}

 protected:
  IBackendService() = default;

  IBackendService(const IBackendService&) = delete;
  IBackendService& operator=(const IBackendService&) = delete;
};

}  // namespace core
}  // namespace app

#endif // BACKEND_SERVICE_H