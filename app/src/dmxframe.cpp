#include "dmxframe.hpp"

#include <algorithm>
#include <cassert>
#include <utility>

using namespace std::chrono_literals;

static const DmxFrame::Timing presets[] =
{
    // USITT DMX512/1986
    {
        .spaceForBreak = 88us,
        .markAfterBreak = 4us,
    },

    // USITT DMX512/1990
    {
        .spaceForBreak = 88us,
        .markAfterBreak = 8us,
    },

    // ANSI E1.11:2004 (DMX512-A)
    {
        .spaceForBreak = 92us,
        .markAfterBreak = 12us,
    },

    // ANSI E1.20:2006 (RDM)
    {
        .spaceForBreak = 176us,
        .markAfterBreak = 12us,
    }
};
constexpr size_t presetCount = sizeof(presets) / sizeof(DmxFrame::Timing);

//-----------------------------------------------------------------------------

DmxFrame::DmxFrame(const Preset& preset)
{
    slotValues.resize(513);
    std::fill(slotValues.begin(), slotValues.end(), 0);
    setTiming(preset);
}

//-----------------------------------------------------------------------------

void DmxFrame::setTiming(const Timing& value)
{
    if (timing != value)
    {
        timing = value;
        emit timingChanged();
        recalculate();
    }
}

//-----------------------------------------------------------------------------

void DmxFrame::setTiming(const Preset& preset)
{
    const auto idx = static_cast<size_t>(preset);
    if (idx > presetCount) { throw std::out_of_range("DMX frame preset not defined"); };
    setTiming(presets[idx]);
}

//-----------------------------------------------------------------------------

void DmxFrame::setStartCode(const uint8_t value)
{
    const auto oldValue = std::exchange(slotValues[0], value);
    if (oldValue != value)
    {
        slotValues[0] = value;
        emit startCodeChanged(oldValue, value);
        recalculate();
    }
}

//-----------------------------------------------------------------------------

void DmxFrame::setDataSlotCount(const uint16_t value)
{
    const auto oldValue = getDataSlotCount();
    if (oldValue != value)
    {
        slotValues.resize(value + 1);
        emit dataSlotCountChanged(oldValue, value);
        recalculate();
    }
}

//-----------------------------------------------------------------------------

void DmxFrame::setDataSlot(const uint16_t slot, const uint8_t value)
{
    if (slot >= slotValues.size())
    {
        return;
    }
    if (slotValues[slot + 1] != value)
    {
        slotValues[slot + 1] = value;
        emit dataSlotChanged(slot);
        recalculate();
    }
}

//-----------------------------------------------------------------------------

void DmxFrame::setDataSlots(const std::span<const uint8_t> values)
{
    const auto count = std::min(static_cast<uint16_t>(values.size()), getDataSlotCount());
    for (uint16_t slot = 0; slot < count; ++slot)
    {
        if (slotValues[slot + 1] != values[slot])
        {
            slotValues[slot + 1] = values[slot];
            emit dataSlotChanged(slot);
        }
    }
    recalculate();
}

//-----------------------------------------------------------------------------

void DmxFrame::setLogicValues(const SampleType low, const SampleType high)
{
    logicLow = low;
    logicHigh = high;
    recalculate();
}

//-----------------------------------------------------------------------------

void DmxFrame::recalculate()
{
    const auto frameDuration =
        timing.markBeforeBreak
        + timing.spaceForBreak
        + timing.markAfterBreak
        + ((timing.bit * 11) * slotValues.size())
        + (timing.markTimeBetweenSlots * (slotValues.size() - 1));
    frame.resize(frameDuration / sampleResolution);

    // Mark before break
    const auto mmbEnd = frame.begin() + (timing.markBeforeBreak / sampleResolution);
    std::fill(frame.begin(), mmbEnd, logicHigh);

    // Break
    const auto spaceForBreakEnd = mmbEnd + (timing.spaceForBreak / sampleResolution);
    std::fill(mmbEnd, spaceForBreakEnd, logicLow);

    // Mark after break
    const auto mabEnd = spaceForBreakEnd + (timing.markAfterBreak / sampleResolution);
    std::fill(spaceForBreakEnd, mabEnd, logicHigh);

    // Slots
    auto iFrame = mabEnd;
    const auto slotFill = [&](const std::chrono::microseconds duration, const SampleType value)
    {
        if (duration <= 0us)
        {
            return iFrame;
        }
        const auto iSlotEnd = iFrame + (duration / sampleResolution);
        std::fill(iFrame, iSlotEnd, value);
        return iSlotEnd;
    };
    auto iSlotValue = slotValues.cbegin();
    while(iSlotValue != slotValues.cend())
    {
        iFrame = slotFill(timing.bit, logicLow); // Start bit
        for (uint8_t bit = 0; bit < 8; ++bit) // Slot value
        {
            const auto fillValue = (*iSlotValue & (1U << bit)) ? logicHigh : logicLow;
            iFrame = slotFill(timing.bit, fillValue);
        }
        iFrame = slotFill(timing.bit * 2, logicHigh); // Stop bits
        if (++iSlotValue != slotValues.cend())
        {
            iFrame = slotFill(timing.markTimeBetweenSlots, logicHigh); // Mark time between slots
        }
    }
    assert(iFrame == frame.cend());
    emit frameUpdated();
}
