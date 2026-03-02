#include "startup_loading_app.h"

startup_loading_app::startup_loading_app(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::startup_loading_appClass())
{
    ui->setupUi(this);
}

startup_loading_app::~startup_loading_app()
{
    delete ui;
}

