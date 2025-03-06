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
    : AComponent(name, 16)
{
    // Input pins
    m_pins[0]  = Pin(Pin::Type::INPUT);  // Strobe
    m_pins[22]  = Pin(Pin::Type::INPUT);  // Inhibit
    m_pins[1]  = Pin(Pin::Type::INPUT);  // D (Address bit 0)
    m_pins[2]  = Pin(Pin::Type::INPUT);  // C (Address bit 1)
    m_pins[20]  = Pin(Pin::Type::INPUT);  // B (Address bit 2)
    m_pins[21]  = Pin(Pin::Type::INPUT);  // A (Address bit 3)

    // Output pins
    m_pins[10] = Pin(Pin::Type::OUTPUT); // Q0
    m_pins[8] = Pin(Pin::Type::OUTPUT); // Q1
    m_pins[11] = Pin(Pin::Type::OUTPUT); // Q2
    m_pins[7] = Pin(Pin::Type::OUTPUT); // Q3
    m_pins[6] = Pin(Pin::Type::OUTPUT); // Q4
    m_pins[5] = Pin(Pin::Type::OUTPUT); // Q5
    m_pins[4]  = Pin(Pin::Type::OUTPUT); // Q6
    m_pins[3]  = Pin(Pin::Type::OUTPUT); // Q7
    m_pins[17]  = Pin(Pin::Type::OUTPUT); // Q8
    m_pins[16]  = Pin(Pin::Type::OUTPUT); // Q9

    // Additional outputs
    m_pins[19] = Pin(Pin::Type::OUTPUT); // Q10
    m_pins[18] = Pin(Pin::Type::OUTPUT); // Q11
    m_pins[13] = Pin(Pin::Type::OUTPUT); // Q12
    m_pins[12] = Pin(Pin::Type::OUTPUT); // Q13
    m_pins[15] = Pin(Pin::Type::OUTPUT); // Q14
    m_pins[14] = Pin(Pin::Type::OUTPUT); // Q15

    // Power and Ground
    m_pins[11] = Pin(Pin::Type::ELECTRICAL); // VSS (Ground)
    m_pins[23] = Pin(Pin::Type::ELECTRICAL); // VDD (Power)
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4514::compute(size_t pin)
{
    if (pin == 11 || pin == 23) {
        return Tristate::Undefined;
    }
    if (pin == 0 || pin == 1 || pin == 2 || pin == 21 || pin == 22) {
        return getInputState(pin);
    }

    // Handle output pins
    if ((pin >= 3 && pin <= 8) || (pin >= 10 && pin <= 19)) {
        Tristate strobe = getInputState(0);
        Tristate inhibit = getInputState(22);

        if (inhibit == Tristate::True) {
            return Tristate::False;
        }
        if (strobe == Tristate::False) {
            return Tristate::Undefined;
        }

        // Calculate address (0-15)
        unsigned int address = 0;
        if (getInputState(1) == Tristate::True) address |= 1;     // D
        if (getInputState(2) == Tristate::True) address |= 2;     // C
        if (getInputState(20) == Tristate::True) address |= 4;     // B
        if (getInputState(21) == Tristate::True) address |= 8;     // A

        // Convert pin index to output index for matching
        size_t outputPins[] = {10, 8, 11, 7, 6, 5, 4, 3, 17, 16, 19, 18, 13, 12, 15, 14};

        return (pin == outputPins[address]) ? Tristate::True : Tristate::False;
    }
    throw OutOfRangePinException();
}

///////////////////////////////////////////////////////////////////////////////
void C4514::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    // Propagate all output pins
    size_t outputPins[] = {0, 1, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
    for (size_t pin : outputPins) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components