// Copyright 2026 startup_loading_app. All rights reserved.
// main_window.h — Application main window, shown after all services load.
//
// This file replaces the original startup_loading_app.h.
// The main window is fully decoupled from the loading mechanism:
// it is constructed AFTER AppLoader completes so that all backend
// services are guaranteed to be initialized when this window appears.

#pragma once

#include <QtWidgets/QMainWindow>

// Forward-declare generated UI class.
namespace Ui {
class MainWindowClass;
}

namespace app {
namespace ui {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

  // Non-copyable / non-movable.
  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

 private:
  void SetupConnections();

  Ui::MainWindowClass* ui_;
};

}  // namespace ui
}  // namespace app
