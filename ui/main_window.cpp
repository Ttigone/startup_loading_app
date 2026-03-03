#include "main_window.h"
#include "ui_main_window.h"

namespace app {
namespace ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui_(new Ui::MainWindowClass()) {
  ui_->setupUi(this);
  SetupConnections();
}

// ---------------------------------------------------------------------------
MainWindow::~MainWindow() {
  delete ui_;
}

// ---------------------------------------------------------------------------
void MainWindow::SetupConnections() {
  // Connect menu/toolbar/button signals to slots here.
  // All backend services are guaranteed initialized at this point.
}

}  // namespace ui
}  // namespace app
