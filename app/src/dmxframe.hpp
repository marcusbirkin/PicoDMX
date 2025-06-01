#pragma once

#include <QObject>

#include <chrono>
#include <limits>
#include <span>
#include <vector>

//-----------------------------------------------------------------------------

class DmxFrame : public QObject
{
    Q_OBJECT
public:
    using SampleType = int16_t;
    constexpr static auto sampleResolution = std::chrono::microseconds{1};

    struct Timing
    {
        bool operator==(const Timing& other)const = default;
        bool operator!=(const Timing& other)const = default;
        std::chrono::microseconds bit{4};
        std::chrono::microseconds spaceForBreak;
        std::chrono::microseconds markAfterBreak;
        std::chrono::microseconds markTimeBetweenSlots{0};
        std::chrono::microseconds markBeforeBreak{0};
    };

    enum class Preset
    {
        USITT_1986,
        USITT_1990,
        E1_11_2004,
        E1_20_2006,
    };

    DmxFrame(const Preset& preset = Preset::E1_11_2004);

    /*!
     * \brief Get the currently applied timing values
     */
    Timing getTiming() const { return timing; }

    /*!
     * \brief Set new timing values
     */
    void setTiming(const Timing&);

    /*!
     * \brief Set new timing values from a preset
     */
    void setTiming(const Preset&);

    /*!
     * \brief Set the DMX frame start code
     */
    void setStartCode(uint8_t value);

    /*!
     * \brief Get the currently applied start code
     */
    uint8_t getStartCode() const { return slotValues[0]; }

    /*!
     * \brief Get the number of data slots (excluding start code)
     */
    uint16_t getDataSlotCount() const { return slotValues.size() - 1; }

    /*!
     * \brief Set the number of data slots (excluding start code)
     */
    void setDataSlotCount(uint16_t);

    /*!
     * \brief Get data slot values
     */
    std::span<const uint8_t> getDataSlots() const { return {slotValues.begin() + 1, slotValues.end()}; }

    /*!
     * \brief Set data slot value
     */
    void setDataSlot(uint16_t slot, uint8_t value);

    /*!
     * \brief Set data slot values
     */
    void setDataSlots(std::span<const uint8_t>);
    void setDataSlots(std::span<const std::byte> values) { setDataSlots({reinterpret_cast<const uint8_t*>(values.data()), values.size()}); }

    /*!
     * \brief Set sample value representing logic levels
     * \param low Logic low value
     * \param high Logic high value
     */
    void setLogicValues(SampleType low, SampleType high);

    /*!
     * \brief Get duration of DMX frame
     */
    std::chrono::microseconds getDuration() const { return frame.size() * sampleResolution; }

    /*!
     * \brief Get the number of waveform samples in the DMX frame
     */
    size_t getSampleCount() const { return frame.size(); }

    /*!
     * \brief Get waveform samples representing DMX frame
     */
    const std::vector<SampleType>& getFrame() const { return frame; }

signals:
    void startCodeChanged(uint8_t oldValue, uint8_t newValue);
    void dataSlotCountChanged(uint16_t oldValue, uint16_t newValue);
    void dataSlotChanged(uint16_t slot);
    void timingChanged();
    void frameUpdated();
    void sampleCountChanged();

private:
    void recalculate();

    SampleType logicLow{0};
    SampleType logicHigh{std::numeric_limits<SampleType>::max()};

    // Complete DMX in samples
    std::vector<SampleType> frame;

    // Slot values (inc start code)
    std::vector<uint8_t> slotValues;

    Timing timing;
};
