///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4017.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4017::C4017(const std::string& name)
    : AComponent(name, 16)
    , m_count(0)
{
    m_pins[12] = Pin(Pin::Type::INPUT, "CP0");
    m_pins[13] = Pin(Pin::Type::INPUT, "CP1");
    m_pins[14] = Pin(Pin::Type::INPUT, "MR");

    m_pins[0]  = Pin(Pin::Type::OUTPUT, "Q5");
    m_pins[1]  = Pin(Pin::Type::OUTPUT, "Q1");
    m_pins[2]  = Pin(Pin::Type::OUTPUT, "Q0");
    m_pins[3]  = Pin(Pin::Type::OUTPUT, "Q2");
    m_pins[4]  = Pin(Pin::Type::OUTPUT, "Q6");
    m_pins[5]  = Pin(Pin::Type::OUTPUT, "Q7");
    m_pins[6]  = Pin(Pin::Type::OUTPUT, "Q3");
    m_pins[8]  = Pin(Pin::Type::OUTPUT, "Q8");
    m_pins[9]  = Pin(Pin::Type::OUTPUT, "Q4");
    m_pins[10] = Pin(Pin::Type::OUTPUT, "Q9");
    m_pins[11] = Pin(Pin::Type::OUTPUT, "Q5-9");

    m_pins[7]  = Pin(Pin::Type::ELECTRICAL, "VSS");
    m_pins[15] = Pin(Pin::Type::ELECTRICAL, "QDD");
}

Tristate C4017::decoder(size_t pin)
{
    if ((pin == 0 || pin == 11) && m_count == 5) // count 5
        return (Tristate::True);
    if ((pin == 1 || pin == 11) && m_count == 1) // count 1
        return (Tristate::True);
    if ((pin == 2 || pin == 11) && m_count == 0) // count 0
        return (Tristate::True);
    if (pin == 3 && m_count == 2) // count 2
        return (Tristate::True);
    if (pin == 4 && m_count == 6) // count 6
        return (Tristate::True);
    if (pin == 5 && m_count == 7) // count 7
        return (Tristate::True);
    if ((pin == 6 || pin == 11) && m_count == 3) // count 3
        return (Tristate::True);
    if (pin == 8 && m_count == 8) // count 8
        return (Tristate::True);
    if ((pin == 9 || pin == 11) && m_count == 4) // count 4
        return (Tristate::True);
    if (pin == 10 && m_count == 9) // count 9
        return (Tristate::True);
    return Tristate::False;
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4017::compute(size_t pin)
{
    switch (pin) {
        case 7:
        case 15:
            return Tristate::Undefined;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 11:
            return (decoder(pin));
        case 12:
        case 13:
        case 14:
            return getInputState(pin);
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4017::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);
    Tristate cp0 = getInputState(12);
    Tristate cp1 = getInputState(13);
    Tristate mr = getInputState(14);

    if (mr == Tristate::True) {
        m_count = 0;
    } else if (cp0 == Tristate::True && cp1 == Tristate::False && m_lastCp0 != Tristate::True) {
        m_count += m_count < 9 ? 1 : -9;
    } else if (cp0 == Tristate::True && cp1 == Tristate::True && m_lastCp1 != Tristate::True) {
        m_count += m_count < 9 ? 1 : -9;
    }
    m_lastCp0 = cp0;
    m_lastCp1 = cp1;

    for (size_t pin : {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components