#pragma once

#include "dmxframe.hpp"

#include <QMainWindow>

#include <memory>

class PicoScope;

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

//-----------------------------------------------------------------------------

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void saveSettings();
    void loadSettings();

    void refreshConnectionWidgets();
    void refreshFrameSetupWidgets();

    void populateDeviceList();
    void updateFrameTiming();
    void setAwg();

    void connectDevice();
    void disconnectDevice();

    Ui::MainWindow *ui;
    DmxFrame frame;
    std::unique_ptr<PicoScope> picoscope;
};
