// Copyright 2026 startup_loading_app. All rights reserved.
// main.cpp — Application entry point.
//
// Startup sequence:
//   1. Create QApplication.
//   2. Create SplashScreen (visible immediately, fade-in animation).
//   3. Build AppLoader and register all backend services.
//   4. Wire AppLoader callbacks → SplashScreen slots (thread-safe).
//   5. AppLoader::Start() — loader thread begins.
//   6. On SplashScreen::LoadingFinished → create & show MainWindow,
//      hide splash.
//   7. On SplashScreen::LoadingFailed  → show error dialog, quit.
//   8. QApplication::exec() runs the event loop.
//   9. After exec() returns, AppLoader::ShutdownAll() cleans up.

#include <memory>

#include <QApplication>
#include <QMessageBox>

#include "core/app_loader.h"
#include "services/config_service.h"
#include "services/data_service.h"
#include "ui/main_window.h"
#include "ui/splash_screen.h"

int main(int argc, char* argv[]) {
  QApplication qt_app(argc, argv);
  qt_app.setApplicationName("My Application");
  qt_app.setApplicationVersion("1.0.0");
  qt_app.setOrganizationName("MyOrg");

  // ---- 1. Splash screen ---------------------------------------------------
  auto splash = std::make_unique<app::ui::SplashScreen>();

  // ---- 2. Backend loader --------------------------------------------------
  auto loader = std::make_unique<app::core::AppLoader>();

  // Register services — order determines loading sequence.
  // Add / remove services freely; weights auto-distribute the progress bar.
  loader->RegisterService(std::make_unique<app::services::ConfigService>())
        .RegisterService(std::make_unique<app::services::DataService>());

  // ---- 3. Wire progress callback (loader thread → GUI thread) -------------
  auto progress_cb = splash->MakeProgressCallback();
  loader->OnProgress([progress_cb](app::core::LoadProgress prog) {
    progress_cb(prog.percent, prog.message);
  });

  // ---- 4. Wire error callback ---------------------------------------------
  auto error_cb = splash->MakeErrorCallback();
  loader->OnError([error_cb](std::string svc, std::string msg) {
    error_cb(svc, msg);
  });

  // ---- 5. Handle fatal load failure ---------------------------------------
  // LoadingFailed is emitted on the GUI thread (see ShowError), so this
  // direct connection is safe.
  app::ui::MainWindow* main_window_ptr = nullptr;

  QObject::connect(
      splash.get(), &app::ui::SplashScreen::LoadingFailed,
      [&qt_app, &splash](const QString& service_name,
                          const QString& error_msg) {
        QMessageBox::critical(
            nullptr,
            QStringLiteral("启动失败"),
            QStringLiteral("服务 [%1] 初始化失败:\n%2\n\n程序即将退出。")
                .arg(service_name, error_msg));
        qt_app.quit();
      });

  // ---- 6. On finish: show main window, hide splash ------------------------
  loader->OnFinished([&splash, &main_window_ptr]() {
    // This lambda is called on the loader thread; post to GUI thread.
    QMetaObject::invokeMethod(
        splash.get(),
        [&splash, &main_window_ptr]() {
          // SplashScreen::LoadingFinished is emitted by SplashScreen itself
          // after the fade-out animation; we connect to that signal below.
          // Nothing extra needed here — just let the done-timer run.
          (void)main_window_ptr;
        },
        Qt::QueuedConnection);
  });

  // When splash finishes its fade-out animation, show the main window.
  QObject::connect(
      splash.get(), &app::ui::SplashScreen::LoadingFinished,
      [&main_window_ptr]() {
        main_window_ptr = new app::ui::MainWindow();
        main_window_ptr->setAttribute(Qt::WA_DeleteOnClose);
        main_window_ptr->show();
      });

  // ---- 7. Start loader ----------------------------------------------------
  loader->Start();

  // ---- 8. Enter the Qt event loop -----------------------------------------
  const int exit_code = qt_app.exec();

  // ---- 9. Shutdown services -----------------------------------------------
  loader->WaitForCompletion();
  loader->ShutdownAll();

  return exit_code;
}

