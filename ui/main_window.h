#ifndef MAIN_WINDOW_H
#deifne MAIN_WINDOW_H

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

  MainWindow(const MainWindow&) = delete;
  MainWindow& operator=(const MainWindow&) = delete;

 private:
  void SetupConnections();

  Ui::MainWindowClass* ui_;
};

}  // namespace ui
}  // namespace app

#endif // MAIN_WINDOW_H