///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4514.hpp"
#include "Errors/OutOfRangePinException.hpp"
///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{
///////////////////////////////////////////////////////////////////////////////
C4514::C4514(const std::string& name)
    : AComponent(name, 24)
    , m_latchedAddress(0)
    , m_previousStrobe(false)
{
    // Input pins
    m_pins[0]   = Pin(Pin::Type::INPUT);  // Strobe
    m_pins[22]  = Pin(Pin::Type::INPUT);  // Inhibit
    m_pins[1]   = Pin(Pin::Type::INPUT);  // A (Address bit 0)
    m_pins[2]   = Pin(Pin::Type::INPUT);  // B (Address bit 1)
    m_pins[20]  = Pin(Pin::Type::INPUT);  // C (Address bit 2)
    m_pins[21]  = Pin(Pin::Type::INPUT);  // D (Address bit 3)

    // Output pins
    m_pins[10]  = Pin(Pin::Type::OUTPUT); // Q0
    m_pins[8]   = Pin(Pin::Type::OUTPUT); // Q1
    m_pins[9]   = Pin(Pin::Type::OUTPUT); // Q2
    m_pins[7]   = Pin(Pin::Type::OUTPUT); // Q3
    m_pins[6]   = Pin(Pin::Type::OUTPUT); // Q4
    m_pins[5]   = Pin(Pin::Type::OUTPUT); // Q5
    m_pins[4]   = Pin(Pin::Type::OUTPUT); // Q6
    m_pins[3]   = Pin(Pin::Type::OUTPUT); // Q7
    m_pins[17]  = Pin(Pin::Type::OUTPUT); // Q8
    m_pins[16]  = Pin(Pin::Type::OUTPUT); // Q9

    // Additional outputs
    m_pins[19]  = Pin(Pin::Type::OUTPUT); // Q10
    m_pins[18]  = Pin(Pin::Type::OUTPUT); // Q11
    m_pins[13]  = Pin(Pin::Type::OUTPUT); // Q12
    m_pins[12]  = Pin(Pin::Type::OUTPUT); // Q13
    m_pins[15]  = Pin(Pin::Type::OUTPUT); // Q14
    m_pins[14]  = Pin(Pin::Type::OUTPUT); // Q15

    // Power and Ground
    m_pins[11]  = Pin(Pin::Type::ELECTRICAL); // VSS (Ground)
    m_pins[23]  = Pin(Pin::Type::ELECTRICAL); // VDD (Power)
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4514::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    if (m_pins[pin].getType() != Pin::Type::OUTPUT) {
        return (getInputState(pin));
    }

    Tristate strobe = getInputState(0);
    Tristate inhibit = getInputState(22);

    if (inhibit == Tristate::True) {
        return (Tristate::False);
    }

    // Calculate address (0-15)
    unsigned int address = 0;
    if (strobe == Tristate::True) {
        if (getInputState(1) == Tristate::True)     address |= 1;     // A
        if (getInputState(2) == Tristate::True)     address |= 2;     // B
        if (getInputState(20) == Tristate::True)    address |= 4;     // C
        if (getInputState(21) == Tristate::True)    address |= 8;     // D
    } else {
        address = m_latchedAddress;
    }

    size_t outputPins[] = {
        10, 8, 9, 7, 6, 5, 4, 3, 17, 16, 19, 18, 13, 12, 15, 14
    };

    return (pin == outputPins[address]) ? Tristate::True : Tristate::False;
}

///////////////////////////////////////////////////////////////////////////////
void C4514::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    Tristate currentStrobe = getInputState(0);

    if (m_previousStrobe && currentStrobe == Tristate::False) {
        m_latchedAddress = 0;
        if (getInputState(1) == Tristate::True)  m_latchedAddress |= 1; // A
        if (getInputState(2) == Tristate::True)  m_latchedAddress |= 2; // B
        if (getInputState(20) == Tristate::True) m_latchedAddress |= 4; // C
        if (getInputState(21) == Tristate::True) m_latchedAddress |= 8; // D
    }
    m_previousStrobe = (currentStrobe == Tristate::True);

    // Propagate all output pins
    size_t outputPins[] = {10, 8, 11, 7, 6, 5, 4, 3, 17, 16, 19, 18, 13, 12, 15, 14};
    for (size_t pin : outputPins) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components