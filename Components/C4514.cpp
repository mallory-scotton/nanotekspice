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
    m_pins[0]   = Pin(Pin::Type::INPUT, "Strobe");  // Strobe
    m_pins[22]  = Pin(Pin::Type::INPUT, "Inhibit");  // Inhibit
    m_pins[1]   = Pin(Pin::Type::INPUT, "A");  // A (Address bit 0)
    m_pins[2]   = Pin(Pin::Type::INPUT, "B");  // B (Address bit 1)
    m_pins[20]  = Pin(Pin::Type::INPUT, "C");  // C (Address bit 2)
    m_pins[21]  = Pin(Pin::Type::INPUT, "D");  // D (Address bit 3)

    // Output pins
    m_pins[10]  = Pin(Pin::Type::OUTPUT, "Q0"); // Q0
    m_pins[8]   = Pin(Pin::Type::OUTPUT, "Q1"); // Q1
    m_pins[9]   = Pin(Pin::Type::OUTPUT, "Q2"); // Q2
    m_pins[7]   = Pin(Pin::Type::OUTPUT, "Q3"); // Q3
    m_pins[6]   = Pin(Pin::Type::OUTPUT, "Q4"); // Q4
    m_pins[5]   = Pin(Pin::Type::OUTPUT, "Q5"); // Q5
    m_pins[4]   = Pin(Pin::Type::OUTPUT, "Q6"); // Q6
    m_pins[3]   = Pin(Pin::Type::OUTPUT, "Q7"); // Q7
    m_pins[17]  = Pin(Pin::Type::OUTPUT, "Q8"); // Q8
    m_pins[16]  = Pin(Pin::Type::OUTPUT, "Q9"); // Q9

    // Additional outputs
    m_pins[19]  = Pin(Pin::Type::OUTPUT, "Q10"); // Q10
    m_pins[18]  = Pin(Pin::Type::OUTPUT, "Q11"); // Q11
    m_pins[13]  = Pin(Pin::Type::OUTPUT, "Q12"); // Q12
    m_pins[12]  = Pin(Pin::Type::OUTPUT, "Q13"); // Q13
    m_pins[15]  = Pin(Pin::Type::OUTPUT, "Q14"); // Q14
    m_pins[14]  = Pin(Pin::Type::OUTPUT, "Q15"); // Q15

    // Power and Ground
    m_pins[11]  = Pin(Pin::Type::ELECTRICAL, "VSS"); // VSS (Ground)
    m_pins[23]  = Pin(Pin::Type::ELECTRICAL, "VDD"); // VDD (Power)
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

    unsigned int address = 0;
    if (strobe == Tristate::True) {
        address = m_latchedAddress;
    } else {
        if (getInputState(1) == Tristate::True)     address |= 1;     // A
        if (getInputState(2) == Tristate::True)     address |= 2;     // B
        if (getInputState(20) == Tristate::True)    address |= 4;     // C
        if (getInputState(21) == Tristate::True)    address |= 8;     // D
    }

    size_t outputPins[] = {10,8,9,7,6,5,4,3,17,16,19,18,13,12,15,14};

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

    for (size_t pin : {10,8,11,7,6,5,4,3,17,16,19,18,13,12,15,14}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components