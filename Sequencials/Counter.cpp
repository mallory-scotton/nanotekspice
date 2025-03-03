///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Counter.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Sequencials
///////////////////////////////////////////////////////////////////////////////
namespace nts::Sequencials
{

///////////////////////////////////////////////////////////////////////////////
Counter::Counter(const std::string& name)
    : AComponent(name, 4)
    , m_count(0)
    , m_lastClock(Tristate::Undefined)
{
    m_pins[0] = Pin(Pin::Type::INPUT);
    m_pins[1] = Pin(Pin::Type::INPUT);
    m_pins[2] = Pin(Pin::Type::OUTPUT);
    m_pins[3] = Pin(Pin::Type::OUTPUT);
}

///////////////////////////////////////////////////////////////////////////////
Tristate Counter::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    if (pin == 2)
        return static_cast<Tristate>(m_count % 2);
    if (pin == 3)
        return static_cast<Tristate>(m_count == 9 ?
            Tristate::True : Tristate::False);
    return getInputState(pin);
}

///////////////////////////////////////////////////////////////////////////////
void Counter::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    Tristate clock = getInputState(0);
    Tristate reset = getInputState(1);

    if (reset == Tristate::True) {
        m_count = 0;
    } else if (m_lastClock == Tristate::False && clock == Tristate::True) {
        m_count = (m_count + 1) % 10;
    }

    m_lastClock = clock;

    propagateOutput(2, compute(2));
    propagateOutput(3, compute(3));
}

} // namespace nts::Sequencials
