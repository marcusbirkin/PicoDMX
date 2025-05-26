#include "unit.hpp"

#include <QDebug>

#include <array>
#include <stdio.h>

using namespace PicoScopePrivate;

//-----------------------------------------------------------------------------

Unit::Unit(const std::string& serial, const SdkFunctions& functions)
    : functions(functions), serial(serial)
{
    QObject::startTimer(std::chrono::seconds(1));
}

//-----------------------------------------------------------------------------

void Unit::timerEvent(QTimerEvent*)
{
    if (!isOpen()) { return; }
    if (const auto result = functions.pingUnit(handle);
        (result != PICO_OK) && (result != PICO_BUSY))
    {
        qDebug() << "Ping fail, reason" << result;
        handle = invalidHandle;
        emit closed();
    }
}

//-----------------------------------------------------------------------------

PICO_STATUS Unit::open()
{
    const auto result = functions.openUnit(
        &handle,
        const_cast<StringBuffer*>(reinterpret_cast<const StringBuffer*>(serial.c_str())));
    if (result != PICO_OK)
    {
        qDebug() << "Failed to open" << serial << "reason:" << result;
        handle = invalidHandle;
    }
    else
    {
        emit openned();
    }
    return result;
}

//-----------------------------------------------------------------------------

void Unit::close()
{
    if (handle != invalidHandle)
    {
        functions.closeUnit(handle);
        handle = invalidHandle;
        emit closed();
    }
}

//-----------------------------------------------------------------------------

std::string Unit::getVariant() const
{
    if (!isOpen())
    {
        return {};
    }

    std::array<StringBuffer, BUFSIZ> strBuffer;
    StringBufferSize strBufferSize = sizeof(strBuffer);
    if (PICO_OK == functions.unitInfo(handle, strBuffer.data(), strBufferSize, &strBufferSize, PICO_VARIANT_INFO))
    {
        const auto strLength = std::min(strlen(reinterpret_cast<char*>(strBuffer.data())), static_cast<size_t>(strBufferSize));
        return std::string{reinterpret_cast<char*>(strBuffer.data()), strLength};
    }
    return {};
}

//-----------------------------------------------------------------------------

Unit::AllowedSampleRange Unit::getAWGAllowedSampleRange() const
{
    if (!isOpen())
    {
        return {};
    }
    int16_t minValue, maxValue;
    uint32_t minSize, maxSize;
    if (PICO_OK == functions.sigGenArbitraryMinMaxValues(handle, &minValue, &maxValue, &minSize, &maxSize))
    {
        return {{minValue, maxValue}, {minSize, maxSize}};
    }
    return {};
}

//-----------------------------------------------------------------------------

PICO_STATUS Unit::setAwg(const std::vector<int16_t>& waveform, std::chrono::microseconds resolution, double &frequency)
{
    if (!isOpen())
    {
        return PICO_INVALID_HANDLE;
    }

    // [TODO] Pass as param to setAwg()
    constexpr uint32_t pkToPk = 4 * std::micro::den; // Peak to Peak (µV) = 4v peak to peak, +2v peak

    using namespace std::chrono_literals;
    constexpr auto ticksInOneSecond = std::chrono::duration_cast<decltype(resolution)>(1s).count();
    frequency = (resolution * ticksInOneSecond).count() / static_cast<double>(waveform.size());
    qDebug() << "Using AWG frequency" << frequency << "Hz";

    PICO_STATUS result;
    uint32_t deltaPhase;
    constexpr auto indexMode = IndexMode::single;
    result = functions.sigGenFrequencyToPhase(handle, frequency, indexMode, waveform.size(), &deltaPhase);
    if (result == PICO_OK)
    {
        result = functions.setSigGenArbitrary(
            handle,
            0, // Offset Voltage (µV)
            pkToPk,
            deltaPhase, // Start
            deltaPhase, // Stop
            0, // Delta Phase Increment
            0, // Dwell count
            waveform.data(),
            waveform.size(),
            SweepType::up,
            ExtraOperations::off,
            indexMode,
            std::numeric_limits<uint32_t>::max(), // Shots
            std::numeric_limits<uint32_t>::max(), // Sweeps
            SigGenTriggerType::rising,
            SigGenTrigSource::none,
            0); // External in threshold
    }

    if (result != PICO_OK)
    {
        qDebug() << "Failed to set AWG, reason:" << result;
    }
    return result;
}


