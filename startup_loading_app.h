#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_startup_loading_app.h"

QT_BEGIN_NAMESPACE
namespace Ui { class startup_loading_appClass; };
QT_END_NAMESPACE

class startup_loading_app : public QMainWindow
{
    Q_OBJECT

public:
    startup_loading_app(QWidget *parent = nullptr);
    ~startup_loading_app();

private:
    Ui::startup_loading_appClass *ui;
};

