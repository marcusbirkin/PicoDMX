#pragma once

#include "functions.hpp"

//-----------------------------------------------------------------------------

namespace PicoScopePrivate
{
    template <>
    const SdkFunctions& getFunctions<DeviceClass::PS4000A>()
    {
        // PicoScope 4000A
        static const SdkFunctions ps4000a = {
            .enumerateUnits = ps4000aEnumerateUnits,
            .openUnit = ps4000aOpenUnit,
            .closeUnit = ps4000aCloseUnit,
            .pingUnit = ps4000aPingUnit,
            .unitInfo = ps4000aGetUnitInfo,
            .sigGenArbitraryMinMaxValues = ps4000aSigGenArbitraryMinMaxValues,
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
                return ps4000aSetSigGenArbitrary(
                    handle,
                    offsetVoltage,
                    pkToPk,
                    startDeltaPhase,
                    stopDeltaPhase,
                    deltaPhaseIncrement,
                    dwellCount,
                    const_cast<int16_t*>(arbitraryWaveform),
                    arbitraryWaveformSize,
                    lookup<DeviceClass::PS4000A>(sweepType),
                    lookup<DeviceClass::PS4000A>(operation),
                    lookup<DeviceClass::PS4000A>(indexMode),
                    shots,
                    sweeps,
                    lookup<DeviceClass::PS4000A>(triggerType),
                    lookup<DeviceClass::PS4000A>(triggerSource),
                    extInThreshold);
            },
            .sigGenFrequencyToPhase = [](
                const Handle handle,
                const double frequency,
                const IndexMode	indexMode,
                const uint32_t bufferLength,
                uint32_t* phase)
            {
                return ps4000aSigGenFrequencyToPhase(
                    handle,
                    frequency,
                    lookup<DeviceClass::PS4000A>(indexMode),
                    bufferLength,
                    phase);
            }
        };
        return ps4000a;
    }
}
