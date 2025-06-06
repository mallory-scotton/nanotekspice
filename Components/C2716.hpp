///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "AComponent.hpp"
#include <array>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class C2716 : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private properties
    ///////////////////////////////////////////////////////////////////////////
    std::array<uint8_t, 2048> m_memory;     //<!
    bool m_fileLoaded;                      //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    ///////////////////////////////////////////////////////////////////////////
    C2716(const std::string& name);

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    bool loadRomFile(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_t getAddress(void);

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    void simulate(size_t tick) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param pin
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Tristate compute(size_t pin) override;
};

} // namespace nts::Components
