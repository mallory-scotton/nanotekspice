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

    m_pins[0] = Pin(Pin::Type::INPUT, "X7");        // addr7
    m_pins[1] = Pin(Pin::Type::INPUT, "X6");        // addr6
    m_pins[2] = Pin(Pin::Type::INPUT, "X5");        // addr5
    m_pins[3] = Pin(Pin::Type::INPUT, "X4");        // addr4
    m_pins[4] = Pin(Pin::Type::INPUT, "X3");        // addr3
    m_pins[5] = Pin(Pin::Type::INPUT, "X2");        // addr2
    m_pins[6] = Pin(Pin::Type::INPUT, "X1");        // addr1
    m_pins[7] = Pin(Pin::Type::INPUT, "X0");        // addr0
    m_pins[17] = Pin(Pin::Type::INPUT, "Enable");   // enable
    m_pins[19] = Pin(Pin::Type::INPUT, "Read");     // read
    m_pins[20] = Pin(Pin::Type::INPUT, "Write");    // write
    m_pins[21] = Pin(Pin::Type::INPUT, "X9");       // addr9
    m_pins[22] = Pin(Pin::Type::INPUT, "X8");       // addr8

    m_pins[8] = Pin(Pin::Type::OUTPUT, "O0");       // O0
    m_pins[9] = Pin(Pin::Type::OUTPUT, "O1");       // O1
    m_pins[10] = Pin(Pin::Type::OUTPUT, "O2");      // O2
    m_pins[12] = Pin(Pin::Type::OUTPUT, "O3");      // O3
    m_pins[13] = Pin(Pin::Type::OUTPUT, "O4");      // O4
    m_pins[14] = Pin(Pin::Type::OUTPUT, "O5");      // O5
    m_pins[15] = Pin(Pin::Type::OUTPUT, "O6");      // O6
    m_pins[16] = Pin(Pin::Type::OUTPUT, "O7");      // O7

    m_pins[18] = Pin(Pin::Type::ELECTRICAL);        // Ignored
    m_pins[11] = Pin(Pin::Type::ELECTRICAL);        // Ignored
    m_pins[23] = Pin(Pin::Type::ELECTRICAL);        // Ignored
}

///////////////////////////////////////////////////////////////////////////////
size_t C4801::getAddress() {
    size_t address = 0;

    // Build 11-bit address from pins
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
Tristate C4801::readDataPins() {
    uint8_t dataValue = 0;

    if (getInputState(8) == Tristate::True) dataValue |= 0x01;
    if (getInputState(9) == Tristate::True) dataValue |= 0x02;
    if (getInputState(10) == Tristate::True) dataValue |= 0x04;
    if (getInputState(12) == Tristate::True) dataValue |= 0x08;
    if (getInputState(13) == Tristate::True) dataValue |= 0x10;
    if (getInputState(14) == Tristate::True) dataValue |= 0x20;
    if (getInputState(15) == Tristate::True) dataValue |= 0x40;
    if (getInputState(16) == Tristate::True) dataValue |= 0x80;

    return (static_cast<Tristate>(dataValue));
}

///////////////////////////////////////////////////////////////////////////////
void C4801::writeToMemory() {
    size_t address = getAddress();

    if (address < m_memory.size()) {
        m_memory[address] = readDataPins();
    }
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4801::readFromMemory() {
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

    bool enabled = (getInputState(17) == Tristate::True);
    bool readEnabled = (getInputState(19) == Tristate::True);
    bool writeEnabled = (getInputState(20) == Tristate::True);

    if (enabled) {
        if (writeEnabled && !readEnabled) {
            writeToMemory();
        } else if (readEnabled && !writeEnabled) {
            Tristate data = readFromMemory();

            int dataValue = static_cast<int>(data);
            propagateOutput(8, (dataValue & 0x01) ? Tristate::True : Tristate::False);
            propagateOutput(9, (dataValue & 0x02) ? Tristate::True : Tristate::False);
            propagateOutput(10, (dataValue & 0x04) ? Tristate::True : Tristate::False);
            propagateOutput(12, (dataValue & 0x08) ? Tristate::True : Tristate::False);
            propagateOutput(13, (dataValue & 0x10) ? Tristate::True : Tristate::False);
            propagateOutput(14, (dataValue & 0x20) ? Tristate::True : Tristate::False);
            propagateOutput(15, (dataValue & 0x40) ? Tristate::True : Tristate::False);
            propagateOutput(16, (dataValue & 0x80) ? Tristate::True : Tristate::False);
        } else {
            for (size_t pin : {8, 9, 10, 12, 13, 14, 15, 16}) {
                propagateOutput(pin, Tristate::Undefined);
            }
        }
    } else {
        for (size_t pin : {8, 9, 10, 12, 13, 14, 15, 16}) {
            propagateOutput(pin, Tristate::Undefined);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4801::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    if (m_pins[pin].getType() != Pin::Type::OUTPUT)
        return (getInputState(pin));

    bool enabled = (getInputState(17) == Tristate::True);
    bool readEnabled = (getInputState(18) == Tristate::True);
    bool writeEnabled = (getInputState(19) == Tristate::True);

    if (enabled && readEnabled && !writeEnabled) {
        Tristate data = readFromMemory();
        int dataValue = static_cast<int>(data);

        switch (pin) {
            case 8:  return (dataValue & 0x01) ? Tristate::True : Tristate::False;
            case 9:  return (dataValue & 0x02) ? Tristate::True : Tristate::False;
            case 10: return (dataValue & 0x04) ? Tristate::True : Tristate::False;
            case 12: return (dataValue & 0x08) ? Tristate::True : Tristate::False;
            case 13: return (dataValue & 0x10) ? Tristate::True : Tristate::False;
            case 14: return (dataValue & 0x20) ? Tristate::True : Tristate::False;
            case 15: return (dataValue & 0x40) ? Tristate::True : Tristate::False;
            case 16: return (dataValue & 0x80) ? Tristate::True : Tristate::False;
            default: return (Tristate::Undefined);
        }
    }
    return (Tristate::Undefined);
}

} // namespace nts::Components
