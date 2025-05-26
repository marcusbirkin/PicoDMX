#pragma once

#include "imports.hpp"

#include <stdexcept>

//-----------------------------------------------------------------------------

namespace PicoScopePrivate
{
    enum class SweepType
    {
        up,
        down,
        updown,
        downup,
    };

    template<DeviceClass T>
    constexpr auto lookup(const SweepType sweepType)
    {
        if constexpr (T == DeviceClass::PS2000A)
        {
            switch (sweepType)
            {
            case SweepType::up: return PS2000A_UP;
            case SweepType::down: return PS2000A_DOWN;
            case SweepType::updown: return PS2000A_UPDOWN;
            case SweepType::downup: return PS2000A_DOWNUP;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS3000A)
        {
            switch (sweepType)
            {
            case SweepType::up: return PS3000A_UP;
            case SweepType::down: return PS3000A_DOWN;
            case SweepType::updown: return PS3000A_UPDOWN;
            case SweepType::downup: return PS3000A_DOWNUP;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS4000A)
        {
            switch (sweepType)
            {
            case SweepType::up: return PS4000A_UP;
            case SweepType::down: return PS4000A_DOWN;
            case SweepType::updown: return PS4000A_UPDOWN;
            case SweepType::downup: return PS4000A_DOWNUP;
            default: break;
            }
        }
        throw std::out_of_range("Unhandled SweepType");
    }

    //-------------------------------------------------------------------------

    enum class ExtraOperations
    {
        off,
        whitenoise,
        prbs,
    };

    template<DeviceClass T>
    constexpr auto lookup(const ExtraOperations extraOperations)
    {
        if constexpr (T == DeviceClass::PS2000A)
        {
            switch (extraOperations)
            {
            case ExtraOperations::off: return PS2000A_ES_OFF;
            case ExtraOperations::whitenoise: return PS2000A_WHITENOISE;
            case ExtraOperations::prbs: return PS2000A_PRBS;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS3000A)
        {
            switch (extraOperations)
            {
            case ExtraOperations::off: return PS3000A_ES_OFF;
            case ExtraOperations::whitenoise: return PS3000A_WHITENOISE;
            case ExtraOperations::prbs: return PS3000A_PRBS;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS4000A)
        {
            switch (extraOperations)
            {
            case ExtraOperations::off: return PS4000A_ES_OFF;
            case ExtraOperations::whitenoise: return PS4000A_WHITENOISE;
            case ExtraOperations::prbs: return PS4000A_PRBS;
            default: break;
            }
        }
        throw std::out_of_range("Unhandled ExtraOperations");
    }

    //-------------------------------------------------------------------------

    enum class IndexMode
    {
        single,
        dual,
        quad,
    };

    template<DeviceClass T>
    constexpr auto lookup(const IndexMode indexMode)
    {
        if constexpr (T == DeviceClass::PS2000A)
        {
            switch (indexMode)
            {
            case IndexMode::single: return PS2000A_SINGLE;
            case IndexMode::dual: return PS2000A_DUAL;
            case IndexMode::quad: return PS2000A_QUAD;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS3000A)
        {
            switch (indexMode)
            {
            case IndexMode::single: return PS3000A_SINGLE;
            case IndexMode::dual: return PS3000A_DUAL;
            case IndexMode::quad: return PS3000A_QUAD;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS4000A)
        {
            switch (indexMode)
            {
            case IndexMode::single: return PS4000A_SINGLE;
            case IndexMode::dual: return PS4000A_DUAL;
            case IndexMode::quad: return PS4000A_QUAD;
            default: break;
            }
        }
        throw std::out_of_range("Unhandled IndexMode");
    }

    //-------------------------------------------------------------------------

    enum class SigGenTriggerType
    {
        rising,
        falling,
        gateHigh,
        gateLow,
    };

    template<DeviceClass T>
    constexpr auto lookup(const SigGenTriggerType triggerType)
    {
        if constexpr (T == DeviceClass::PS2000A)
        {
            switch (triggerType)
            {
            case SigGenTriggerType::rising: return PS2000A_SIGGEN_RISING;
            case SigGenTriggerType::falling: return PS2000A_SIGGEN_FALLING;
            case SigGenTriggerType::gateHigh: return PS2000A_SIGGEN_GATE_HIGH;
            case SigGenTriggerType::gateLow: return PS2000A_SIGGEN_GATE_LOW;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS3000A)
        {
            switch (triggerType)
            {
            case SigGenTriggerType::rising: return PS3000A_SIGGEN_RISING;
            case SigGenTriggerType::falling: return PS3000A_SIGGEN_FALLING;
            case SigGenTriggerType::gateHigh: return PS3000A_SIGGEN_GATE_HIGH;
            case SigGenTriggerType::gateLow: return PS3000A_SIGGEN_GATE_LOW;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS4000A)
        {
            switch (triggerType)
            {
            case SigGenTriggerType::rising: return PS4000A_SIGGEN_RISING;
            case SigGenTriggerType::falling: return PS4000A_SIGGEN_FALLING;
            case SigGenTriggerType::gateHigh: return PS4000A_SIGGEN_GATE_HIGH;
            case SigGenTriggerType::gateLow: return PS4000A_SIGGEN_GATE_LOW;
            default: break;
            }
        }
        throw std::out_of_range("Unhandled SigGenTriggerType");
    }

    //-------------------------------------------------------------------------

    enum class SigGenTrigSource
    {
        none,
        scope,
        auxIn,
        extIn,
        soft,
    };

    template<DeviceClass T>
    constexpr auto lookup(const SigGenTrigSource triggerSource)
    {
        if constexpr (T == DeviceClass::PS2000A)
        {
            switch (triggerSource)
            {
            case SigGenTrigSource::none: return PS2000A_SIGGEN_NONE;
            case SigGenTrigSource::scope: return PS2000A_SIGGEN_SCOPE_TRIG;
            case SigGenTrigSource::auxIn: return PS2000A_SIGGEN_AUX_IN;
            case SigGenTrigSource::extIn: return PS2000A_SIGGEN_EXT_IN;
            case SigGenTrigSource::soft: return PS2000A_SIGGEN_SOFT_TRIG;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS3000A)
        {
            switch (triggerSource)
            {
            case SigGenTrigSource::none: return PS3000A_SIGGEN_NONE;
            case SigGenTrigSource::scope: return PS3000A_SIGGEN_SCOPE_TRIG;
            case SigGenTrigSource::auxIn: return PS3000A_SIGGEN_AUX_IN;
            case SigGenTrigSource::extIn: return PS3000A_SIGGEN_EXT_IN;
            case SigGenTrigSource::soft: return PS3000A_SIGGEN_SOFT_TRIG;
            default: break;
            }
        }
        else if constexpr (T == DeviceClass::PS4000A)
        {
            switch (triggerSource)
            {
            case SigGenTrigSource::none: return PS4000A_SIGGEN_NONE;
            case SigGenTrigSource::scope: return PS4000A_SIGGEN_SCOPE_TRIG;
            case SigGenTrigSource::auxIn: return PS4000A_SIGGEN_AUX_IN;
            case SigGenTrigSource::extIn: return PS4000A_SIGGEN_EXT_IN;
            case SigGenTrigSource::soft: return PS4000A_SIGGEN_SOFT_TRIG;
            default: break;
            }
        }
        throw std::out_of_range("Unhandled SigGenTrigSource");
    }
}
