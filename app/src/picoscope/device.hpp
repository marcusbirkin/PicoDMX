#pragma once

#include <QObject>

#include <cstdint>
#include <string>
#include <utility>

struct PicoScopeDevice
{
    /*!
     * \brief Device serial
     */
    std::string serial;

    /*!
     * \brief Scope variant (a.k.a. model)
     */
    std::string variant;

    /*!
     * \brief Allowed arbitrary waveform value
     * \return minimum value
     * \return maxmim value
     */
    using WaveformSample = int16_t;
    std::pair<WaveformSample, WaveformSample> arbitraryWaveformValue;

    /*!
     * \brief Allowed arbitrary waveform size
     * \return minimum size
     * \return maxmim size
     */
    std::pair<uint32_t, uint32_t> arbitraryWaveformSize;

    /*!
     * \internal
     * \brief SDK Function ponters
     */
    const void* functions;
};
Q_DECLARE_METATYPE(PicoScopeDevice)
