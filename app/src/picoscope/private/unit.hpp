#pragma once

#include "functions.hpp"

#include <QDebug>
#include <QObject>

#include <utility>

//-----------------------------------------------------------------------------

namespace PicoScopePrivate
{
    class Unit : public QObject
    {
        Q_OBJECT
        static constexpr PicoScopePrivate::Handle invalidHandle = INT16_MAX;
    public:
        Unit(const std::string& serial, const PicoScopePrivate::SdkFunctions&);
        ~Unit() { close(); }

        PICO_STATUS open();
        void close();
        bool isOpen() const { return (handle != invalidHandle); }

        std::string getVariant() const;

        struct AllowedSampleRange
        {
            /*!
             * \brief Allowed sample value range
             * \return minimum
             * \return maximum
             */
            std::pair<int16_t, int16_t> value {0,0};

            /*!
             * \brief Allowed sample size range
             * \return minimum
             * \return maximum
             */
            std::pair<uint32_t, uint32_t> size {0,0};
        };

        /*!
         * \brief Get the ranges accepted by the AWG
         * \return Allowed ranges
         */
        AllowedSampleRange getAWGAllowedSampleRange() const;

        /*!
         * \brief Set the AWG to repeatedly output a waveform
         * \param waveform Samples used to create waveform
         * \param resolution Sample width
         * \param[out] frequency Resultent waveform output frequency (Hz)
         * \return Function status
         */
        PICO_STATUS setAwg(const std::vector<int16_t>& waveform, std::chrono::microseconds resolution, double &frequency);

    signals:
        /*!
         * \brief Unit has just opened
         */
        void openned();

        /*!
         * \brief Unit has just closed
         */
        void closed();

    private:
        void timerEvent(QTimerEvent*) override;

        const PicoScopePrivate::SdkFunctions& functions;
        const std::string serial;
        PicoScopePrivate::Handle handle{invalidHandle};
    };
}
