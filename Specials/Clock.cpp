///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Clock.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Specials
///////////////////////////////////////////////////////////////////////////////
namespace nts::Specials
{

///////////////////////////////////////////////////////////////////////////////
Clock::Clock(const std::string& name)
    : AComponent(name, 1)
    , m_valueWasSet(false)
{
    m_pins[0] = Pin(Pin::Type::OUTPUT);
}

///////////////////////////////////////////////////////////////////////////////
void Clock::setValue(Tristate value)
{
    m_pins[0].setState(value);
    m_valueWasSet = true;
}

///////////////////////////////////////////////////////////////////////////////
void Clock::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    m_tick = tick;
    if (m_valueWasSet) {
        m_valueWasSet = false;
        return;
    }
    if (m_pins[0].getState() != Tristate::Undefined)
        m_pins[0].setState(~(m_pins[0].getState()));
}

///////////////////////////////////////////////////////////////////////////////
Tristate Clock::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    return (m_pins[0].getState());
}

} // namespace nts::Specials
