///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4801.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4801::C4801(const std::string& name)
    : AComponent(name, 24)
{
    m_memory.resize(1024, Tristate::Undefined);

    // Address pins (A0-A9)
    m_pins[7] = Pin(Pin::Type::INPUT, "A0");        // addr0
    m_pins[6] = Pin(Pin::Type::INPUT, "A1");        // addr1
    m_pins[5] = Pin(Pin::Type::INPUT, "A2");        // addr2
    m_pins[4] = Pin(Pin::Type::INPUT, "A3");        // addr3
    m_pins[3] = Pin(Pin::Type::INPUT, "A4");        // addr4
    m_pins[2] = Pin(Pin::Type::INPUT, "A5");        // addr5
    m_pins[1] = Pin(Pin::Type::INPUT, "A6");        // addr6
    m_pins[0] = Pin(Pin::Type::INPUT, "A7");        // addr7
    m_pins[22] = Pin(Pin::Type::INPUT, "A8");       // addr8
    m_pins[21] = Pin(Pin::Type::INPUT, "A9");       // addr9

    // Control pins
    m_pins[17] = Pin(Pin::Type::INPUT, "E");        // Enable (active high)
    m_pins[19] = Pin(Pin::Type::INPUT, "G");        // Read (active high)
    m_pins[20] = Pin(Pin::Type::INPUT, "W");        // Write (active high)

    // Data pins
    m_pins[8] = Pin(Pin::Type::OUTPUT, "IO0");      // Data0
    m_pins[9] = Pin(Pin::Type::OUTPUT, "IO1");      // Data1
    m_pins[10] = Pin(Pin::Type::OUTPUT, "IO2");     // Data2
    m_pins[12] = Pin(Pin::Type::OUTPUT, "IO3");     // Data3
    m_pins[13] = Pin(Pin::Type::OUTPUT, "IO4");     // Data4
    m_pins[14] = Pin(Pin::Type::OUTPUT, "IO5");     // Data5
    m_pins[15] = Pin(Pin::Type::OUTPUT, "IO6");     // Data6
    m_pins[16] = Pin(Pin::Type::OUTPUT, "IO7");     // Data7

    // Electrical pins
    m_pins[11] = Pin(Pin::Type::ELECTRICAL);        // Ignored
    m_pins[18] = Pin(Pin::Type::ELECTRICAL);        // Ignored
    m_pins[23] = Pin(Pin::Type::ELECTRICAL);        // Ignored
}

///////////////////////////////////////////////////////////////////////////////
size_t C4801::getAddress(void)
{
    size_t address = 0;

    if (getInputState(7) == Tristate::True) address |= 0x001;  // A0
    if (getInputState(6) == Tristate::True) address |= 0x002;  // A1
    if (getInputState(5) == Tristate::True) address |= 0x004;  // A2
    if (getInputState(4) == Tristate::True) address |= 0x008;  // A3
    if (getInputState(3) == Tristate::True) address |= 0x010;  // A4
    if (getInputState(2) == Tristate::True) address |= 0x020;  // A5
    if (getInputState(1) == Tristate::True) address |= 0x040;  // A6
    if (getInputState(0) == Tristate::True) address |= 0x080;  // A7
    if (getInputState(22) == Tristate::True) address |= 0x100; // A8
    if (getInputState(21) == Tristate::True) address |= 0x200; // A9

    return (address);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4801::readDataPins()
{
    int value = 0;

    if (getInputState(8) == Tristate::True) value |= 0x01;
    if (getInputState(9) == Tristate::True) value |= 0x02;
    if (getInputState(10) == Tristate::True) value |= 0x04;
    if (getInputState(12) == Tristate::True) value |= 0x08;
    if (getInputState(13) == Tristate::True) value |= 0x10;
    if (getInputState(14) == Tristate::True) value |= 0x20;
    if (getInputState(15) == Tristate::True) value |= 0x40;
    if (getInputState(16) == Tristate::True) value |= 0x80;

    return (static_cast<Tristate>(value));
}

///////////////////////////////////////////////////////////////////////////////
void C4801::writeToMemory(void)
{
    size_t address = getAddress();

    if (address < m_memory.size()) {
        int value = 0;

        if (getInputState(8) == Tristate::True) value |= 0x01;
        if (getInputState(9) == Tristate::True) value |= 0x02;
        if (getInputState(10) == Tristate::True) value |= 0x04;
        if (getInputState(12) == Tristate::True) value |= 0x08;
        if (getInputState(13) == Tristate::True) value |= 0x10;
        if (getInputState(14) == Tristate::True) value |= 0x20;
        if (getInputState(15) == Tristate::True) value |= 0x40;
        if (getInputState(16) == Tristate::True) value |= 0x80;

        m_memory[address] = static_cast<Tristate>(value);
    }
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4801::readFromMemory(void)
{
    size_t address = getAddress();

    if (address < m_memory.size()) {
        return (m_memory[address]);
    }

    return (Tristate::Undefined);
}

///////////////////////////////////////////////////////////////////////////////
void C4801::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    Tristate enable = getInputState(17);
    Tristate read = getInputState(19);
    Tristate write = getInputState(20);

    if (enable == Tristate::True) {
        if (write == Tristate::True && read == Tristate::False) {
            writeToMemory();
        }
        else if (read == Tristate::True && write == Tristate::False) {
            size_t address = getAddress();

            if (address < m_memory.size()) {
                Tristate storedValue = m_memory[address];

                int value = (storedValue == Tristate::True) ? 0xFF :
                           (storedValue == Tristate::False) ? 0x00 : 0;

                propagateOutput(8, (value & 0x01) ? True : False);
                propagateOutput(9, (value & 0x02) ? True : False);
                propagateOutput(10, (value & 0x04) ? True : False);
                propagateOutput(12, (value & 0x08) ? True : False);
                propagateOutput(13, (value & 0x10) ? True : False);
                propagateOutput(14, (value & 0x20) ? True : False);
                propagateOutput(15, (value & 0x40) ? True : False);
                propagateOutput(16, (value & 0x80) ? True : False);
            }
        }
        else {
            for (size_t pin : {8, 9, 10, 12, 13, 14, 15, 16}) {
                propagateOutput(pin, Tristate::Undefined);
            }
        }
    }
    else {
        for (size_t pin : {8, 9, 10, 12, 13, 14, 15, 16}) {
            propagateOutput(pin, Tristate::Undefined);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4801::compute(size_t pin) {
    if (pin >= m_pins.size())
        throw OutOfRangePinException();

    if (m_pins[pin].getType() != Pin::Type::OUTPUT)
        return getInputState(pin);

    Tristate enable = getInputState(17);
    Tristate read = getInputState(19);
    Tristate write = getInputState(20);

    if (
        enable == Tristate::True &&
        read == Tristate::True &&
        write == Tristate::False
    ) {
        size_t address = getAddress();

        if (address < m_memory.size()) {
            Tristate storedValue = m_memory[address];
            int value = (storedValue == Tristate::True) ? 0xFF :
                       (storedValue == Tristate::False) ? 0x00 : 0;

            switch (pin) {
                case 8:  return ((value & 0x01) ? True : False);
                case 9:  return ((value & 0x02) ? True : False);
                case 10: return ((value & 0x04) ? True : False);
                case 12: return ((value & 0x08) ? True : False);
                case 13: return ((value & 0x10) ? True : False);
                case 14: return ((value & 0x20) ? True : False);
                case 15: return ((value & 0x40) ? True : False);
                case 16: return ((value & 0x80) ? True : False);
                default: return (Tristate::Undefined);
            }
        }
    }

    return (Tristate::Undefined);
}

} // namespace nts::Components
