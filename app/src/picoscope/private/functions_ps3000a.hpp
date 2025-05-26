#pragma once

#include "functions.hpp"

//-----------------------------------------------------------------------------

namespace PicoScopePrivate
{
    template <>
    const SdkFunctions& getFunctions<DeviceClass::PS3000A>()
    {
        static const SdkFunctions ps3000a = {
            .enumerateUnits = ps3000aEnumerateUnits,
            .openUnit = ps3000aOpenUnit,
            .closeUnit = ps3000aCloseUnit,
            .pingUnit = ps3000aPingUnit,
            .unitInfo = ps3000aGetUnitInfo,
            .sigGenArbitraryMinMaxValues = ps3000aSigGenArbitraryMinMaxValues,
            .setSigGenArbitrary = [](
                const Handle handle,
                const int32_t offsetVoltage,
                const uint32_t pkToPk,
                const uint32_t startDeltaPhase,
                const uint32_t stopDeltaPhase,
                const uint32_t deltaPhaseIncrement,
                const uint32_t dwellCount,
                const int16_t* arbitraryWaveform,
                const int32_t arbitraryWaveformSize,
                const SweepType sweepType,
                const ExtraOperations operation,
                const IndexMode indexMode,
                const uint32_t shots,
                const uint32_t sweeps,
                const SigGenTriggerType triggerType,
                const SigGenTrigSource triggerSource,
                const int16_t extInThreshold)
            {
                return ps3000aSetSigGenArbitrary(
                    handle,
                    offsetVoltage,
                    pkToPk,
                    startDeltaPhase,
                    stopDeltaPhase,
                    deltaPhaseIncrement,
                    dwellCount,
                    const_cast<int16_t*>(arbitraryWaveform),
                    arbitraryWaveformSize,
                    lookup<DeviceClass::PS3000A>(sweepType),
                    lookup<DeviceClass::PS3000A>(operation),
                    lookup<DeviceClass::PS3000A>(indexMode),
                    shots,
                    sweeps,
                    lookup<DeviceClass::PS3000A>(triggerType),
                    lookup<DeviceClass::PS3000A>(triggerSource),
                    extInThreshold);
            },
            .sigGenFrequencyToPhase = [](
                const Handle handle,
                const double frequency,
                const IndexMode	indexMode,
                const uint32_t bufferLength,
                uint32_t* phase)
            {
                return ps3000aSigGenFrequencyToPhase(
                    handle,
                    frequency,
                    lookup<DeviceClass::PS3000A>(indexMode),
                    bufferLength,
                    phase);
            }
        };
        return ps3000a;
    }
}
