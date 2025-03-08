///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "FlipFlop.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Sequencials
///////////////////////////////////////////////////////////////////////////////
namespace nts::Sequencials
{

///////////////////////////////////////////////////////////////////////////////
FlipFlop::FlipFlop(const std::string& name)
    : AComponent(name, 6)
    , m_lastClock(Tristate::Undefined)
    , m_q1state(Tristate::False)
    , m_q2state(Tristate::True)
{
    m_pins[0] = Pin(Pin::Type::INPUT, "CLK");
    m_pins[1] = Pin(Pin::Type::INPUT, "D");
    m_pins[2] = Pin(Pin::Type::INPUT, "MS");
    m_pins[3] = Pin(Pin::Type::INPUT, "MR");
    m_pins[4] = Pin(Pin::Type::OUTPUT, "Q");
    m_pins[5] = Pin(Pin::Type::OUTPUT, "Q|");
}

///////////////////////////////////////////////////////////////////////////////
Tristate FlipFlop::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    if (pin <= 3)
        return (getInputState(pin));
    if (pin == 4)
        return (m_q1state);
    return (m_q2state);
}

///////////////////////////////////////////////////////////////////////////////
void FlipFlop::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    Tristate clock  = getInputState(0);
    Tristate data   = getInputState(1);
    Tristate set    = getInputState(2);
    Tristate reset  = getInputState(3);

    if (reset && set) {
        m_q1state = Tristate::True;
        m_q2state = Tristate::True;
    } else if (reset == Tristate::True) {
        m_q1state = Tristate::False;
        m_q2state = Tristate::True;
    } else if (set == Tristate::True) {
        m_q1state = Tristate::True;
        m_q2state = Tristate::False;
    } else {
        if (m_lastClock == Tristate::False && clock == Tristate::True) {
            m_q1state = data;
            m_q2state = ~data;
        }
    }

    m_lastClock = clock;

    propagateOutput(4, m_q1state);
    propagateOutput(5, m_q2state);
}

} // namespace nts::Sequencials
