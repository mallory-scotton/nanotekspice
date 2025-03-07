///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C2716.hpp"
#include "Errors/OutOfRangePinException.hpp"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C2716::C2716(const std::string& name)
    : AComponent(name, 24)
    , m_fileLoaded(false)
{
    m_memory.fill(0xFF);

    m_fileLoaded = loadRomFile();

    m_pins[0] = Pin(Pin::Type::INPUT);   // A7
    m_pins[1] = Pin(Pin::Type::INPUT);   // A6
    m_pins[2] = Pin(Pin::Type::INPUT);   // A5
    m_pins[3] = Pin(Pin::Type::INPUT);   // A4
    m_pins[4] = Pin(Pin::Type::INPUT);   // A3
    m_pins[5] = Pin(Pin::Type::INPUT);   // A2
    m_pins[6] = Pin(Pin::Type::INPUT);   // A1
    m_pins[7] = Pin(Pin::Type::INPUT);   // A0
    m_pins[22] = Pin(Pin::Type::INPUT);  // A8
    m_pins[21] = Pin(Pin::Type::INPUT);  // A9
    m_pins[18] = Pin(Pin::Type::INPUT);  // A10

    m_pins[8] = Pin(Pin::Type::OUTPUT);  // O0
    m_pins[9] = Pin(Pin::Type::OUTPUT);  // O1
    m_pins[10] = Pin(Pin::Type::OUTPUT); // O2
    m_pins[12] = Pin(Pin::Type::OUTPUT); // O3
    m_pins[13] = Pin(Pin::Type::OUTPUT); // O4
    m_pins[14] = Pin(Pin::Type::OUTPUT); // O5
    m_pins[15] = Pin(Pin::Type::OUTPUT); // O6
    m_pins[16] = Pin(Pin::Type::OUTPUT); // O7

    m_pins[17] = Pin(Pin::Type::INPUT);  // Enable
    m_pins[19] = Pin(Pin::Type::INPUT);  // Read

    m_pins[11] = Pin(Pin::Type::ELECTRICAL);
    m_pins[20] = Pin(Pin::Type::ELECTRICAL);
    m_pins[23] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
bool C2716::loadRomFile(void)
{
    std::ifstream file("rom.bin", std::ios::binary);
    if (!file.is_open()) {
        return (false);
    }

    file.read(reinterpret_cast<char*>(m_memory.data()), m_memory.size());

    std::streamsize bytesRead = file.gcount();

    if (bytesRead < static_cast<std::streamsize>(m_memory.size())) {
        std::fill(m_memory.begin() + bytesRead, m_memory.end(), 0xFF);
    }

    return (true);
}

///////////////////////////////////////////////////////////////////////////////
size_t C2716::getAddress(void)
{
    size_t address = 0;

    address |= (getInputState(7) == Tristate::True ? 1 : 0) << 0;   // A0
    address |= (getInputState(6) == Tristate::True ? 1 : 0) << 1;   // A1
    address |= (getInputState(5) == Tristate::True ? 1 : 0) << 2;   // A2
    address |= (getInputState(4) == Tristate::True ? 1 : 0) << 3;   // A3
    address |= (getInputState(3) == Tristate::True ? 1 : 0) << 4;   // A4
    address |= (getInputState(2) == Tristate::True ? 1 : 0) << 5;   // A5
    address |= (getInputState(1) == Tristate::True ? 1 : 0) << 6;   // A6
    address |= (getInputState(0) == Tristate::True ? 1 : 0) << 7;   // A7
    address |= (getInputState(22) == Tristate::True ? 1 : 0) << 8;  // A8
    address |= (getInputState(21) == Tristate::True ? 1 : 0) << 9;  // A9
    address |= (getInputState(18) == Tristate::True ? 1 : 0) << 10; // A10

    return (address & 0x7FF);
}

///////////////////////////////////////////////////////////////////////////////
void C2716::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    for (size_t pin : {8, 9, 10, 12, 13, 14, 15, 16}) {
        propagateOutput(pin, compute(pin));
    }
}

///////////////////////////////////////////////////////////////////////////////
Tristate C2716::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    if (m_pins[pin].getType() != Pin::Type::OUTPUT)
        return getInputState(pin);

    Tristate enable = getInputState(17);
    Tristate read = getInputState(19);

    if (enable != Tristate::False || read != Tristate::False)
        return (Tristate::Undefined);

    size_t address = getAddress();

    uint8_t data = m_memory[address];

    size_t bitPos;
    switch (pin) {
        case 8:  bitPos = 0; break;
        case 9:  bitPos = 1; break;
        case 10: bitPos = 2; break;
        case 12: bitPos = 3; break;
        case 13: bitPos = 4; break;
        case 14: bitPos = 5; break;
        case 15: bitPos = 6; break;
        case 16: bitPos = 7; break;
        default:
            return (Tristate::Undefined);
    }

    // Invert the output based on error message pattern
    return (data & (1 << bitPos)) ? Tristate::True : Tristate::False;
}

} // namespace nts::Components
