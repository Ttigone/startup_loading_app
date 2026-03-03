#pragma once

#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>
#include <functional>
#include <string>

namespace Ui {
class SplashScreenClass;
}

namespace app {
namespace ui {

class SplashScreen : public QWidget {
  Q_OBJECT

 public:
  explicit SplashScreen(QWidget* parent = nullptr);
  ~SplashScreen() override;

  SplashScreen(const SplashScreen&) = delete;
  SplashScreen& operator=(const SplashScreen&) = delete;

  // Returns a ProgressCallback that posts UpdateProgress to the GUI thread.
  std::function<void(int /*percent*/, std::string /*message*/)>
  MakeProgressCallback();

  // Returns an ErrorCallback that posts ShowError to the GUI thread.
  std::function<void(std::string /*service*/, std::string /*msg*/)>
  MakeErrorCallback();

 signals:
  // 登录成功，准备进入主界面
  void LoadingFinished();
  // 后端服务失败
  void LoadingFailed(const QString& service_name, const QString& error_msg);

 public slots:
  void UpdateProgress(int percent, const QString& message);
  void ShowError(const QString& service_name, const QString& error_msg);

 private slots:
  void OnFadeInFinished();
  void OnDoneTimerTimeout();
  void OnFadeOutFinished();

 private:
  void SetupUi();
  void StartFadeIn();
  void StartFadeOut();

  Ui::SplashScreenClass* ui_;

  QPropertyAnimation* fade_in_anim_;
  QPropertyAnimation* fade_out_anim_;
  QTimer* done_timer_;
  int current_percent_;
};

}  // namespace ui
}  // namespace app
