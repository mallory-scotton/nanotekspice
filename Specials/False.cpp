///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "False.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Specials
///////////////////////////////////////////////////////////////////////////////
namespace nts::Specials
{

///////////////////////////////////////////////////////////////////////////////
False::False(const std::string& name)
    : AComponent(name, 1)
{
    m_pins[0] = Pin(Pin::Type::OUTPUT);
    m_pins[0].setState(Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
Tristate False::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    return (Tristate::False);
}

} // namespace nts::Specials
