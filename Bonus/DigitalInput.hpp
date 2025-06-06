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
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Input8 : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    uint8_t m_value;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    ///////////////////////////////////////////////////////////////////////////
    Input8(const std::string& name);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void innerDraw(void) override;

public:
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Input16 : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    uint16_t m_value;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    ///////////////////////////////////////////////////////////////////////////
    Input16(const std::string& name);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void innerDraw(void) override;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void setValue(uint16_t value);

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

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Input32 : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    uint32_t m_value;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    ///////////////////////////////////////////////////////////////////////////
    Input32(const std::string& name);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void innerDraw(void) override;

public:
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class InputColor : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    //
    ///////////////////////////////////////////////////////////////////////////
    uint32_t m_value;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    ///////////////////////////////////////////////////////////////////////////
    InputColor(const std::string& name);

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void innerDraw(void) override;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param value
    ///
    ///////////////////////////////////////////////////////////////////////////
    void setValue(uint32_t value);

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
