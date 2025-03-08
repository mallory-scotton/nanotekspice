///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4512.hpp"
#include "Errors/OutOfRangePinException.hpp"
///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{
///////////////////////////////////////////////////////////////////////////////
C4512::C4512(const std::string& name)
    : AComponent(name, 16)
{
    m_pins[0] = Pin(Pin::Type::INPUT, "X0");   // X0 (Data input 0)
    m_pins[1] = Pin(Pin::Type::INPUT, "X1");   // X1 (Data input 1)
    m_pins[2] = Pin(Pin::Type::INPUT, "X2");   // X2 (Data input 2)
    m_pins[3] = Pin(Pin::Type::INPUT, "X3");   // X3 (Data input 3)
    m_pins[4] = Pin(Pin::Type::INPUT, "X4");   // X4 (Data input 4)
    m_pins[5] = Pin(Pin::Type::INPUT, "X5");   // X5 (Data input 5)
    m_pins[6] = Pin(Pin::Type::INPUT, "X6");   // X6 (Data input 6)
    m_pins[8] = Pin(Pin::Type::INPUT, "X7");   // X7 (Data input 7)
    m_pins[9] = Pin(Pin::Type::INPUT, "Inhibit");   // Inhibit
    m_pins[10] = Pin(Pin::Type::INPUT, "A");  // A (Address bit 0)
    m_pins[11] = Pin(Pin::Type::INPUT, "B");  // B (Address bit 1)
    m_pins[12] = Pin(Pin::Type::INPUT, "C");  // C (Address bit 2)
    m_pins[14] = Pin(Pin::Type::INPUT, "Strobe");  // Strobe

    m_pins[13] = Pin(Pin::Type::OUTPUT);

    m_pins[7] = Pin(Pin::Type::ELECTRICAL);
    m_pins[15] = Pin(Pin::Type::ELECTRICAL);
}
///////////////////////////////////////////////////////////////////////////////
Tristate C4512::compute(size_t pin)
{
    if (pin == 7 || pin == 15) {
        return Tristate::Undefined;
    }
    if (pin != 7 && pin != 15 && pin != 13) {
        return getInputState(pin);
    }
    if (pin == 13) {
        Tristate inhibit = getInputState(9);
        Tristate strobe = getInputState(14);

        if (inhibit == Tristate::True) {
            return Tristate::False;
        }
        Tristate abit = getInputState(10);
        Tristate bbit = getInputState(11);
        Tristate cbit = getInputState(12);

        if (strobe == Tristate::True ||
            (abit == Undefined && bbit == Undefined && cbit == Undefined)) {
            return Tristate::Undefined;
        }
        unsigned int address = 0;

        if (cbit == Tristate::True)
            address |= 1;
        if (bbit == Tristate::True)
            address |= 2;
        if (abit == Tristate::True)
            address |= 4;

        return (getInputState(address == 7 ? 8 : address));
    }

    throw OutOfRangePinException();
}
///////////////////////////////////////////////////////////////////////////////
void C4512::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    propagateOutput(13, compute(13));
}

} // namespace nts::Components