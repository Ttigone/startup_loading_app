// Copyright 2026 startup_loading_app. All rights reserved.
// splash_screen.h — Frameless, animated startup splash screen.
//
// Responsibilities:
//   1. Display a progress bar driven by AppLoader signals.
//   2. Emit LoadingFinished() when progress reaches 100 % and the
//      brief "done" animation completes.
//   3. Bridge std::function callbacks (from AppLoader's loader thread)
//      to Qt slots via QMetaObject::invokeMethod (thread-safe).
//
// Design patterns:
//   - Mediator : SplashScreen mediates between AppLoader and MainWindow.
//   - Observer  : Qt signals/slots for inter-object communication.
//
// Coding style: Google C++ Style Guide

#pragma once

#include <functional>
#include <string>

#include <QPropertyAnimation>
#include <QTimer>
#include <QWidget>

// Forward-declare generated UI class.
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

  // Non-copyable / non-movable.
  SplashScreen(const SplashScreen&) = delete;
  SplashScreen& operator=(const SplashScreen&) = delete;

  // --- Callback factories (return lambdas safe to capture in AppLoader) ----

  // Returns a ProgressCallback that posts UpdateProgress to the GUI thread.
  std::function<void(int /*percent*/, std::string /*message*/)>
  MakeProgressCallback();

  // Returns an ErrorCallback that posts ShowError to the GUI thread.
  std::function<void(std::string /*service*/, std::string /*msg*/)>
  MakeErrorCallback();

 signals:
  // Emitted when the splash animation completes and the main window should open.
  void LoadingFinished();

  // Emitted when a backend service fails to load.
  void LoadingFailed(const QString& service_name, const QString& error_msg);

 public slots:
  // Thread-safe: may be called from any thread via QMetaObject::invokeMethod.
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

  QPropertyAnimation* fade_in_anim_;   // opacity 0 → 1 on show
  QPropertyAnimation* fade_out_anim_;  // opacity 1 → 0 before hiding
  QTimer*             done_timer_;     // brief pause at 100 % before fade out

  int current_percent_;  // last reported percent
};

}  // namespace ui
}  // namespace app
