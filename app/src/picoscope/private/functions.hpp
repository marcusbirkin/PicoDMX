#pragma once

#include "imports.hpp"
#include "lookups.hpp"

#include <functional>

//-----------------------------------------------------------------------------

namespace PicoScopePrivate
{
    using Handle = int16_t;
    using StringBuffer = signed char;
    using StringBufferSize = int16_t;

    /*!
     * \details
     *      PicoScope maintain different API for each device class,
     *      this creates a common interface
     */
    struct SdkFunctions
    {
        std::function<PICO_STATUS(int16_t*, StringBuffer*, StringBufferSize*)> enumerateUnits;
        std::function<PICO_STATUS(Handle*, StringBuffer*)> openUnit;
        std::function<PICO_STATUS(Handle)> closeUnit;
        std::function<PICO_STATUS(Handle)> pingUnit;
        std::function<PICO_STATUS(Handle, StringBuffer*, StringBufferSize, StringBufferSize*, PICO_INFO)> unitInfo;
        std::function<PICO_STATUS(Handle, int16_t*, int16_t*, uint32_t*, uint32_t*)> sigGenArbitraryMinMaxValues;
        std::function<PICO_STATUS(Handle, int32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, const int16_t*, int32_t, SweepType, ExtraOperations, IndexMode, uint32_t, uint32_t, SigGenTriggerType, SigGenTrigSource, int16_t)> setSigGenArbitrary;
        std::function<PICO_STATUS(Handle, double, IndexMode, uint32_t, uint32_t*)> sigGenFrequencyToPhase;
    };

    template <DeviceClass> static const SdkFunctions& getFunctions();
}
