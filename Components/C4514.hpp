///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "AComponent.hpp"
///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{
///////////////////////////////////////////////////////////////////////////////
/// \brief 4-to-16 Line Decoder/Demultiplexer (4514)
///
///////////////////////////////////////////////////////////////////////////////
class C4514 : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    unsigned int m_latchedAddress;
    bool m_previousStrobe;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    /// \param name Component name
    ///
    ///////////////////////////////////////////////////////////////////////////
    C4514(const std::string& name);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief Simulate function
    ///
    /// \param tick Simulation tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    void simulate(size_t tick) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Compute function
    ///
    /// \param pin Pin number
    ///
    /// \return Tristate logic value
    ///
    ///////////////////////////////////////////////////////////////////////////
    Tristate compute(size_t pin) override;
};
} // namespace nts::Components