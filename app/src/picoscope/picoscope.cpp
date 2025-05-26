#include "picoscope.hpp"

#include "private/picosdk.hpp"
#include "private/unit.hpp"

#include <QByteArray>
#include <QDebug>
#include <QList>

#include <array>
#include <stdio.h>

using namespace PicoScopePrivate;

//-----------------------------------------------------------------------------

PicoScope::PicoScope(const PicoScopeDevice& device)
    : unit(new Unit(device.serial, *static_cast<const SdkFunctions*>(device.functions)))
{
    connect(unit.get(), &Unit::openned, this, &PicoScope::openned);
    connect(unit.get(), &Unit::closed, this, &PicoScope::closed);
}

//-----------------------------------------------------------------------------

PicoScope::~PicoScope()
{
    close();
}

//-----------------------------------------------------------------------------

bool PicoScope::open()
{
    return (unit->open() == PICO_OK);
}

//-----------------------------------------------------------------------------

void PicoScope::close()
{
    unit->close();
}

//-----------------------------------------------------------------------------


bool PicoScope::isOpen()
{
    return unit->isOpen();
}

//-----------------------------------------------------------------------------

std::list<PicoScopeDevice> PicoScope::enumerate()
{
    std::list<PicoScopeDevice> devices;

    const auto enumerateUnits = [&devices](const SdkFunctions& functions)
    {
        if (!functions.enumerateUnits) { return; }

        int16_t count;
        std::array<StringBuffer, BUFSIZ> strBuffer;
        StringBufferSize strBufferSize = sizeof(strBuffer);
        const auto result = functions.enumerateUnits(&count, strBuffer.data(), &strBufferSize);
        if ((result != PICO_OK) || (count == 0))
        {
            return;
        }

        const auto serials = QByteArray(reinterpret_cast<char*>(strBuffer.data()), strBufferSize).split(',');
        for (const auto &serial : serials)
        {
            if (serial.isEmpty()) { continue; }
            Unit unit(serial.cbegin(), functions);
            if (unit.open() == PICO_OK)
            {
                qDebug() << "Found picoscope serial " << serial;
                const auto awgAllowedSampleRanges = unit.getAWGAllowedSampleRange();
                devices.emplace_back(
                    serial.cbegin(),
                    unit.getVariant(),
                    awgAllowedSampleRanges.value,
                    awgAllowedSampleRanges.size,
                    static_cast<const void*>(&functions));
            }
        }
    };

    enumerateUnits(getFunctions<DeviceClass::PS2000A>());
    enumerateUnits(getFunctions<DeviceClass::PS3000A>());
    enumerateUnits(getFunctions<DeviceClass::PS4000A>());

    return devices;
}

//-----------------------------------------------------------------------------

bool PicoScope::setAwg(const std::vector<int16_t>& waveform, std::chrono::microseconds resolution, double &frequency)
{
    if (isOpen())
    {
        return (unit->setAwg(waveform, resolution, frequency) == PICO_OK);
    }
    return false;
}
