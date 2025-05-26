#pragma once

#include "functions.hpp"

//-----------------------------------------------------------------------------

namespace PicoScopePrivate
{
    template <>
    const SdkFunctions& getFunctions<DeviceClass::PS2000A>()
    {
        // PicoScope 2000A
        static const SdkFunctions ps2000a = {
            .enumerateUnits = ps2000aEnumerateUnits,
            .openUnit = ps2000aOpenUnit,
            .closeUnit = ps2000aCloseUnit,
            .pingUnit = ps2000aPingUnit,
            .unitInfo = ps2000aGetUnitInfo,
            .sigGenArbitraryMinMaxValues = ps2000aSigGenArbitraryMinMaxValues,
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
                return ps2000aSetSigGenArbitrary(
                    handle,
                    offsetVoltage,
                    pkToPk,
                    startDeltaPhase,
                    stopDeltaPhase,
                    deltaPhaseIncrement,
                    dwellCount,
                    const_cast<int16_t*>(arbitraryWaveform),
                    arbitraryWaveformSize,
                    lookup<DeviceClass::PS2000A>(sweepType),
                    lookup<DeviceClass::PS2000A>(operation),
                    lookup<DeviceClass::PS2000A>(indexMode),
                    shots,
                    sweeps,
                    lookup<DeviceClass::PS2000A>(triggerType),
                    lookup<DeviceClass::PS2000A>(triggerSource),
                    extInThreshold);
            },
            .sigGenFrequencyToPhase = [](
                const Handle handle,
                const double frequency,
                const IndexMode	indexMode,
                const uint32_t bufferLength,
                uint32_t* phase)
            {
                return ps2000aSigGenFrequencyToPhase(
                    handle,
                    frequency,
                    lookup<DeviceClass::PS2000A>(indexMode),
                    bufferLength,
                    phase);
            }
        };
        return ps2000a;
    }
}
