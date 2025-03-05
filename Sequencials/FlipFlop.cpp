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
    , m_state(Tristate::False)
{
    m_pins[0] = Pin(Pin::Type::INPUT);      // Clock
    m_pins[1] = Pin(Pin::Type::INPUT);      // Data
    m_pins[2] = Pin(Pin::Type::INPUT);      // Set
    m_pins[3] = Pin(Pin::Type::INPUT);      // Reset
    m_pins[4] = Pin(Pin::Type::OUTPUT);     // Q
    m_pins[5] = Pin(Pin::Type::OUTPUT);     // Q̄
}

///////////////////////////////////////////////////////////////////////////////
Tristate FlipFlop::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    if (pin <= 3)
        return (getInputState(pin));
    if (pin == 4)
        return (~m_state);
    return (m_state);
}

///////////////////////////////////////////////////////////////////////////////
void FlipFlop::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);


    Tristate clock = getInputState(0);
    Tristate data = getInputState(1);
    Tristate set = getInputState(2);
    Tristate reset = getInputState(3);

    if (set == Tristate::True && reset != Tristate::True) {
        m_state = Tristate::True;
    } else if (reset == Tristate::True && set != Tristate::True) {
        m_state = Tristate::False;
    } else if (set == Tristate::True && reset == Tristate::True) {
        m_state = Tristate::Undefined;
    } else {
        if (m_lastClock == Tristate::False && clock == Tristate::True) {
            m_state = data;
        }
    }

    m_lastClock = clock;

    propagateOutput(4, m_state);
    propagateOutput(5, ~m_state);
}

} // namespace nts::Sequencials
