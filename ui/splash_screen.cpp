// Copyright 2026 startup_loading_app. All rights reserved.
// splash_screen.cpp — Implementation of SplashScreen.

#include "splash_screen.h"
#include "ui_splash_screen.h"

#include <QGraphicsOpacityEffect>
#include <QMetaObject>
#include <QScreen>
#include <QGuiApplication>
#include <QMessageBox>

namespace app {
namespace ui {

// ---------------------------------------------------------------------------
SplashScreen::SplashScreen(QWidget* parent)
    : QWidget(parent,
              Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint |
              Qt::SplashScreen),
      ui_(new Ui::SplashScreenClass()),
      fade_in_anim_(nullptr),
      fade_out_anim_(nullptr),
      done_timer_(nullptr),
      current_percent_(0) {
  SetupUi();
  StartFadeIn();
}

// ---------------------------------------------------------------------------
SplashScreen::~SplashScreen() {
  delete ui_;
}

// ---------------------------------------------------------------------------
void SplashScreen::SetupUi() {
  ui_->setupUi(this);
  setAttribute(Qt::WA_TranslucentBackground);

  // Center on primary screen.
  if (QScreen* screen = QGuiApplication::primaryScreen()) {
    const QRect screen_rect = screen->availableGeometry();
    move(screen_rect.center() - rect().center());
  }

  // Set initial progress bar state.
  ui_->progressBar->setRange(0, 100);
  ui_->progressBar->setValue(0);
  ui_->statusLabel->setText(QStringLiteral("正在启动..."));
  ui_->percentLabel->setText(QStringLiteral("0%"));

  // Opacity effect for fade animation.
  auto* opacity_effect = new QGraphicsOpacityEffect(this);
  opacity_effect->setOpacity(0.0);
  setGraphicsEffect(opacity_effect);

  // Fade-in animation.
  fade_in_anim_ = new QPropertyAnimation(opacity_effect, "opacity", this);
  fade_in_anim_->setDuration(500);
  fade_in_anim_->setStartValue(0.0);
  fade_in_anim_->setEndValue(1.0);
  fade_in_anim_->setEasingCurve(QEasingCurve::InOutQuad);
  connect(fade_in_anim_, &QPropertyAnimation::finished,
          this, &SplashScreen::OnFadeInFinished);

  // Fade-out animation.
  fade_out_anim_ = new QPropertyAnimation(opacity_effect, "opacity", this);
  fade_out_anim_->setDuration(400);
  fade_out_anim_->setStartValue(1.0);
  fade_out_anim_->setEndValue(0.0);
  fade_out_anim_->setEasingCurve(QEasingCurve::InOutQuad);
  connect(fade_out_anim_, &QPropertyAnimation::finished,
          this, &SplashScreen::OnFadeOutFinished);

  // "Done" pause timer — 600 ms pause at 100 % before fade-out starts.
  done_timer_ = new QTimer(this);
  done_timer_->setSingleShot(true);
  done_timer_->setInterval(600);
  connect(done_timer_, &QTimer::timeout,
          this, &SplashScreen::OnDoneTimerTimeout);
}

// ---------------------------------------------------------------------------
void SplashScreen::StartFadeIn() {
  show();
  fade_in_anim_->start();
}

// ---------------------------------------------------------------------------
void SplashScreen::StartFadeOut() {
  fade_out_anim_->start();
}

// ---------------------------------------------------------------------------
void SplashScreen::OnFadeInFinished() {
  // Nothing extra to do; the splash is now fully visible.
}

// ---------------------------------------------------------------------------
void SplashScreen::OnDoneTimerTimeout() {
  StartFadeOut();
}

// ---------------------------------------------------------------------------
void SplashScreen::OnFadeOutFinished() {
  hide();
  emit LoadingFinished();
}

// ---------------------------------------------------------------------------
void SplashScreen::UpdateProgress(int percent, const QString& message) {
  // This slot is always called on the GUI thread (see MakeProgressCallback).
  current_percent_ = percent;

  // Animate progress bar smoothly if the QSS supports it; otherwise direct.
  ui_->progressBar->setValue(percent);
  ui_->statusLabel->setText(message);
  ui_->percentLabel->setText(QString::number(percent) + QStringLiteral("%"));

  if (percent >= 100) {
    done_timer_->start();
  }
}

// ---------------------------------------------------------------------------
void SplashScreen::ShowError(const QString& service_name,
                              const QString& error_msg) {
  // Stop any pending timers / animations.
  done_timer_->stop();
  fade_out_anim_->stop();

  ui_->statusLabel->setText(
      QStringLiteral("加载失败: ") + service_name + QStringLiteral(" — ") +
      error_msg);

  // Signal consumers (main.cpp) so they can handle graceful shutdown.
  emit LoadingFailed(service_name, error_msg);
}

// ---------------------------------------------------------------------------
// Callback factories — bridge AppLoader (loader thread) → Qt (GUI thread)
// ---------------------------------------------------------------------------

std::function<void(int, std::string)> SplashScreen::MakeProgressCallback() {
  // Capture a raw pointer: SplashScreen outlives the AppLoader.
  SplashScreen* self = this;
  return [self](int percent, std::string message) {
    // Cross-thread safe: QueuedConnection via invokeMethod.
    QMetaObject::invokeMethod(
        self,
        [self, percent, msg = QString::fromStdString(message)]() {
          self->UpdateProgress(percent, msg);
        },
        Qt::QueuedConnection);
  };
}

std::function<void(std::string, std::string)>
SplashScreen::MakeErrorCallback() {
  SplashScreen* self = this;
  return [self](std::string service_name, std::string error_msg) {
    QMetaObject::invokeMethod(
        self,
        [self,
         svc  = QString::fromStdString(service_name),
         msg  = QString::fromStdString(error_msg)]() {
          self->ShowError(svc, msg);
        },
        Qt::QueuedConnection);
  };
}

}  // namespace ui
}  // namespace app
