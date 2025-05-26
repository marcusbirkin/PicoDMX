#pragma once

#include "device.hpp"

#include <QObject>

#include <chrono>
#include <list>
#include <memory>

namespace PicoScopePrivate
{
    class Unit;
}

//-----------------------------------------------------------------------------

class PicoScope : public QObject
{
    Q_OBJECT
public:
    PicoScope(const PicoScopeDevice&);
    ~PicoScope();

    /*!
     * \brief Open PicoScope device
     * \return Success?
     */
    bool open();

    /*!
     * \brief Close PicoScope device
     */
    void close();

    /*!
     * \brief Check if PicoScope device is open
     * \return Is open?
     */
    bool isOpen();

    /*!
     * \brief Obtain a list of avaliable, ready to open, PicoScope device
     * \return Device list
     */
    static std::list<PicoScopeDevice> enumerate();

    /*!
     * \brief Set the AWG to repeatedly output a waveform
     * \param waveform Samples used to create waveform
     * \param resolution Sample width
     * \param[out] frequency Resultent waveform output frequency (Hz)
     * \return Success?
     */
    bool setAwg(const std::vector<int16_t>& waveform, std::chrono::microseconds resolution, double &frequency);

signals:
    /*!
     * \brief Device has just openned
     */
    void openned();

    /*!
     * \brief Device has just closed
     */
    void closed();

private:
    std::unique_ptr<PicoScopePrivate::Unit> unit;
};

