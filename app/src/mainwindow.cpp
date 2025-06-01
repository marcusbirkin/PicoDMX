#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "dmxframe_dataslots_delegate.hpp"
#include "dmxframe_dataslots_model.hpp"

#include "picoscope/picoscope.hpp"

#include <QMessageBox>
#include <QSettings>
#include <QSignalBlocker>
#include <QStringLiteral>

#include <optional>

enum DeviceListRoles
{
    PicoScopeDevice = Qt::UserRole
};

struct SettingsKey
{
    static constexpr auto geometry = "MainWindow/geometry";
    static constexpr auto bitTimeEdit = "DMXFrame/bitTime";
    static constexpr auto breakTimeEdit = "DMXFrame/breakTime";
    static constexpr auto mabTimeEdit = "DMXFrame/mabTime";
    static constexpr auto mtbsTimeEdit = "DMXFrame/mtbsTime";
    static constexpr auto mbbTimeEdit = "DMXFrame/mbbTime";
    static constexpr auto dataSlotCountEdit = "DMXFrame/dataSlotCount";
    static constexpr auto startCodeEdit = "DMXFrame/startCode";
    static constexpr auto dataSlotValues = "DMXFrame/dataSlotValues";
};

//-----------------------------------------------------------------------------

class ScopedWaitCursor
{
public:
    ScopedWaitCursor() { QApplication::setOverrideCursor(Qt::WaitCursor); }
    ~ScopedWaitCursor() { QApplication::restoreOverrideCursor(); }
};

//-----------------------------------------------------------------------------

QSettings getSettings()
{
    return QSettings{QApplication::organizationName(), QApplication::applicationName()};
}

//-----------------------------------------------------------------------------

const auto toMicrosecondCount = [](const auto& chronoValue)
{
    return std::chrono::duration_cast<std::chrono::microseconds>(chronoValue).count();
};

//-----------------------------------------------------------------------------

std::optional<struct PicoScopeDevice> getSelectedDevice(const QListWidget* deviceList)
{
    const auto selected = deviceList->selectedItems();
    if (selected.empty())
    {
        return {};
    }
    return selected.front()->data(DeviceListRoles::PicoScopeDevice).value<struct PicoScopeDevice>();
}

//=============================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("%1 %2").arg(QApplication::applicationName(), QApplication::applicationVersion()));

    ui->dataSlotsTable->setModel(new DmxFrameDataSlotsModel(frame));
    ui->dataSlotsTable->setItemDelegate(new DmxFrameDataSlotsDelegate(this));
    ui->dataSlotsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->dataSlotsTable->model(), &QAbstractTableModel::dataChanged, this, &MainWindow::setAwg);

    connect(ui->bitTimeEdit, &QSpinBox::valueChanged, this, &MainWindow::updateFrameTiming);
    connect(ui->breakTimeEdit, &QSpinBox::valueChanged, this, &MainWindow::updateFrameTiming);
    connect(ui->mabTimeEdit, &QSpinBox::valueChanged, this, &MainWindow::updateFrameTiming);
    connect(ui->mtbsTimeEdit, &QSpinBox::valueChanged, this, &MainWindow::updateFrameTiming);
    connect(ui->mbbTimeEdit, &QSpinBox::valueChanged, this, &MainWindow::updateFrameTiming);
    connect(ui->dataSlotCountEdit, &QSpinBox::valueChanged, this, [&frame = frame](const int value) { frame.setDataSlotCount(value); });
    connect(ui->startCodeEdit, &QSpinBox::valueChanged, this, [&frame = frame](const int value) { frame.setStartCode(value); });

    connect(ui->deviceList, &QListWidget::itemSelectionChanged, this, &MainWindow::refreshConnectionWidgets);

    connect(ui->pbRefresh, &QPushButton::clicked, this, &MainWindow::populateDeviceList);
    connect(ui->pbConnect, &QPushButton::clicked, this, &MainWindow::connectDevice);
    connect(ui->pbDisconnect, &QPushButton::clicked, this, &MainWindow::disconnectDevice);

    connect(ui->pbPreset1986, &QPushButton::clicked, this, [&frame = frame] { frame.setTiming(DmxFrame::Preset::USITT_1986); });
    connect(ui->pbPreset1990, &QPushButton::clicked, this, [&frame = frame] { frame.setTiming(DmxFrame::Preset::USITT_1990); });
    connect(ui->pbPresetDMX512A, &QPushButton::clicked, this, [&frame = frame] { frame.setTiming(DmxFrame::Preset::E1_11_2004); });
    connect(ui->pbPresetRDM, &QPushButton::clicked, this, [&frame = frame] { frame.setTiming(DmxFrame::Preset::E1_20_2006); });

    connect(ui->pbSlotPatternFull, &QPushButton::clicked,
        this, [&frame = frame]
        {
            std::vector<uint8_t> values(frame.getDataSlotCount());
            std::fill(values.begin(), values.end(), 255);
            frame.setDataSlots({values.cbegin(), values.cend()});
        });
    connect(ui->pbSlotPatternZero, &QPushButton::clicked,
        this, [&frame = frame]
        {
            std::vector<uint8_t> values(frame.getDataSlotCount());
            std::fill(values.begin(), values.end(), 0);
            frame.setDataSlots({values.cbegin(), values.cend()});
        });
    connect(ui->pbSlotPatternIncreasing, &QPushButton::clicked,
        this, [&frame = frame]
        {
            std::vector<uint8_t> values(frame.getDataSlotCount());
            std::iota(values.begin(), values.end(), 0);
            frame.setDataSlots({values.cbegin(), values.cend()});
        });
    connect(ui->pbSlotPatternDecreasing, &QPushButton::clicked,
        this, [&frame = frame]
        {
            std::vector<uint8_t> values(frame.getDataSlotCount());
            std::iota(values.rbegin(), values.rend(), 0);
            frame.setDataSlots({values.cbegin(), values.cend()});
        });

    connect(&frame, &DmxFrame::frameUpdated, this, [&] { setAwg(); });
    connect(&frame, &DmxFrame::timingChanged, this, [&] { refreshFrameSetupWidgets(); });
    connect(&frame, &DmxFrame::startCodeChanged, this, [&] { refreshFrameSetupWidgets(); });
    connect(&frame, &DmxFrame::dataSlotCountChanged, this, [&] { refreshFrameSetupWidgets(); });
    connect(&frame, &DmxFrame::sampleCountChanged, this, [&] { refreshFrameSetupWidgets(); });

    connect(ui->actionAboutQt, &QAction::triggered, this, [&]{ QMessageBox::aboutQt(this); });

    loadSettings();
    populateDeviceList();
    refreshConnectionWidgets();
    refreshFrameSetupWidgets();

    // Auto connect if only a single device found
    if (ui->deviceList->count() == 1)
    {
        ui->deviceList->setCurrentRow(0);
        connectDevice();
    }
}

//-----------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    saveSettings();

    if (picoscope)
    {
        picoscope->close();
    }
    delete ui;
}

//-----------------------------------------------------------------------------

void MainWindow::saveSettings()
{
    auto settings = getSettings();
    settings.setValue(SettingsKey::geometry, saveGeometry());
    settings.setValue(SettingsKey::bitTimeEdit, ui->bitTimeEdit->value());
    settings.setValue(SettingsKey::breakTimeEdit, ui->breakTimeEdit->value());
    settings.setValue(SettingsKey::mabTimeEdit, ui->mabTimeEdit->value());
    settings.setValue(SettingsKey::mtbsTimeEdit, ui->mtbsTimeEdit->value());
    settings.setValue(SettingsKey::mbbTimeEdit, ui->mbbTimeEdit->value());
    settings.setValue(SettingsKey::dataSlotCountEdit, ui->dataSlotCountEdit->value());
    settings.setValue(SettingsKey::startCodeEdit, ui->startCodeEdit->value());
    settings.setValue(SettingsKey::dataSlotValues, QByteArray(frame.getDataSlots()));
}

//-----------------------------------------------------------------------------

void MainWindow::loadSettings()
{
    const auto settings = getSettings();
    restoreGeometry(settings.value(SettingsKey::geometry).toByteArray());

    const auto setValue = [&settings](QSpinBox *widget, const QString key)
    {
        bool ok;
        if (const auto value = settings.value(key).toInt(&ok); ok)
        {
            widget->setValue(value);
        }
    };
    setValue(ui->bitTimeEdit, SettingsKey::bitTimeEdit);
    setValue(ui->breakTimeEdit, SettingsKey::breakTimeEdit);
    setValue(ui->mabTimeEdit, SettingsKey::mabTimeEdit);
    setValue(ui->mtbsTimeEdit, SettingsKey::mtbsTimeEdit);
    setValue(ui->mbbTimeEdit, SettingsKey::mbbTimeEdit);
    setValue(ui->dataSlotCountEdit, SettingsKey::dataSlotCountEdit);
    setValue(ui->startCodeEdit, SettingsKey::startCodeEdit);

    const auto baDataSlotValues = settings.value(SettingsKey::dataSlotValues).toByteArray();
    if (!baDataSlotValues.isEmpty())
    {
        frame.setDataSlots(
            std::as_bytes(std::span<const char>{baDataSlotValues.cbegin(), baDataSlotValues.cend()}));
    }
}

//-----------------------------------------------------------------------------

void MainWindow::refreshConnectionWidgets()
{
    if (picoscope)
    {
        const auto isOpen = picoscope->isOpen();
        ui->pbRefresh->setDisabled(isOpen);
        ui->pbConnect->setDisabled(isOpen);
        ui->pbDisconnect->setEnabled(isOpen);
        ui->deviceList->setDisabled(isOpen);
        ui->groupBoxFrameSetup->setEnabled(isOpen);
    }
    else
    {
        ui->pbRefresh->setEnabled(true);
        ui->pbConnect->setEnabled(ui->deviceList->selectedItems().size());
        ui->pbDisconnect->setEnabled(false);
        ui->deviceList->setEnabled(true);
        ui->groupBoxFrameSetup->setEnabled(false);
    }
}

//-----------------------------------------------------------------------------

void MainWindow::refreshFrameSetupWidgets()
{
    const auto setValue = [](QSpinBox *widget, int value)
    {
        const QSignalBlocker signalBlock(widget);
        widget->setValue(value);
    };
    const auto timing = frame.getTiming();
    setValue(ui->frameTimeDisplay, toMicrosecondCount(frame.getDuration()));
    setValue(ui->bitTimeEdit, toMicrosecondCount(timing.bit));
    setValue(ui->breakTimeEdit, toMicrosecondCount(timing.spaceForBreak));
    setValue(ui->mabTimeEdit, toMicrosecondCount(timing.markAfterBreak));
    setValue(ui->mtbsTimeEdit, toMicrosecondCount(timing.markTimeBetweenSlots));
    setValue(ui->mbbTimeEdit, toMicrosecondCount(timing.markBeforeBreak));
    setValue(ui->startCodeEdit, frame.getStartCode());
    setValue(ui->dataSlotCountEdit, frame.getDataSlotCount());
}

//-----------------------------------------------------------------------------

void MainWindow::populateDeviceList()
{
    const auto waitCursor = ScopedWaitCursor();
    ui->pbRefresh->setEnabled(false);
    ui->deviceList->clear();
    QApplication::processEvents();
    const auto devices = PicoScope::enumerate();
    for (const auto& device : devices)
    {
        auto item = new QListWidgetItem();
        item->setText(QString("PicoScope %1 (%2)").arg(device.variant, device.serial));
        item->setData(DeviceListRoles::PicoScopeDevice, QVariant::fromValue(device));
        ui->deviceList->addItem(item);
    }

    ui->statusbar->showMessage(QString("Found %1 device%2").arg(devices.size()).arg(devices.size() > 1 ? "s" : ""));
    refreshConnectionWidgets();
}

//-----------------------------------------------------------------------------

void MainWindow::updateFrameTiming()
{
    frame.setTiming({
        .bit = std::chrono::microseconds{ui->bitTimeEdit->value()},
        .spaceForBreak = std::chrono::microseconds{ui->breakTimeEdit->value()},
        .markAfterBreak = std::chrono::microseconds{ui->mabTimeEdit->value()},
        .markTimeBetweenSlots = std::chrono::microseconds{ui->mtbsTimeEdit->value()},
        .markBeforeBreak = std::chrono::microseconds{ui->mbbTimeEdit->value()} });

    ui->frameTimeDisplay->setValue(toMicrosecondCount(frame.getDuration()));
}

//-----------------------------------------------------------------------------

void MainWindow::setAwg()
{
    if (picoscope)
    {
        double frequency;
        if (picoscope->setAwg(frame.getFrame(), DmxFrame::sampleResolution, frequency))
        {
            ui->statusbar->showMessage(QString("Refresh rate %1 Hz").arg(frequency));
        }
        else
        {
            if (const auto device = getSelectedDevice(ui->deviceList))
            {
                const auto durationSamples = (frame.getDuration() / DmxFrame::sampleResolution);
                if ((durationSamples < device->arbitraryWaveformSize.first) ||
                    (durationSamples > device->arbitraryWaveformSize.second))
                {
                    ui->frameTimeDisplay->setStyleSheet("background-color: red; color: white");
                    ui->statusbar->showMessage(QStringLiteral("Failed to set AWG. Total frame duration too large."));
                    return;
                }
            }
            ui->statusbar->showMessage(QStringLiteral("Failed to set AWG. Unspported?"));
        }
    }
    ui->frameTimeDisplay->setStyleSheet("");
}

//-----------------------------------------------------------------------------

void MainWindow::connectDevice()
{
    auto device = getSelectedDevice(ui->deviceList);
    if (!device)
    {
        return;
    }

    if (picoscope = std::make_unique<PicoScope>(*device); picoscope)
    {
        connect(picoscope.get(), &PicoScope::openned,
            this, [&]()
            {
                ui->statusbar->showMessage(QStringLiteral("Connected"));
                refreshConnectionWidgets();
                frame.setLogicValues(0, device->arbitraryWaveformValue.second);
                setAwg();
            });

        connect(picoscope.get(), &PicoScope::closed,
            this, [&]()
            {
                ui->statusbar->showMessage(QStringLiteral("Disconnected"));
                refreshConnectionWidgets();
            });

        const auto waitCursor = ScopedWaitCursor();
        ui->statusbar->showMessage(QString("Connecting to %1").arg(device->serial));
        QApplication::processEvents();
        if (!picoscope->open())
        {
            ui->statusbar->showMessage(QString("Connection failed"));
        }
    }
}

//-----------------------------------------------------------------------------

void MainWindow::disconnectDevice()
{
    picoscope.reset();
}
