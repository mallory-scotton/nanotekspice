///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4094.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4094::C4094(const std::string& name)
    : AComponent(name, 16)
    , m_lastClock(Tristate::Undefined)
    , m_shiftRegister(0)
    , m_storageRegister(0)
    , m_lastStrobe(Tristate::Undefined)
    , m_serialOut(Tristate::False)
{
    m_pins[0] = Pin(Pin::Type::INPUT);
    m_pins[1] = Pin(Pin::Type::INPUT);
    m_pins[2] = Pin(Pin::Type::INPUT);
    m_pins[14] = Pin(Pin::Type::INPUT);

    m_pins[3] = Pin(Pin::Type::OUTPUT);
    m_pins[4] = Pin(Pin::Type::OUTPUT);
    m_pins[5] = Pin(Pin::Type::OUTPUT);
    m_pins[6] = Pin(Pin::Type::OUTPUT);
    m_pins[13] = Pin(Pin::Type::OUTPUT);
    m_pins[12] = Pin(Pin::Type::OUTPUT);
    m_pins[11] = Pin(Pin::Type::OUTPUT);
    m_pins[10] = Pin(Pin::Type::OUTPUT);
    m_pins[8] = Pin(Pin::Type::OUTPUT);
    m_pins[9] = Pin(Pin::Type::OUTPUT);

    m_pins[7] = Pin(Pin::Type::ELECTRICAL);
    m_pins[15] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4094::compute(size_t pin)
{
    Tristate outputEnable = getInputState(14);

    if (pin == 7 || pin == 15) {
        return (Tristate::Undefined);
    }
    if (pin == 0 || pin == 1 || pin == 2 || pin == 14) {
        return (getInputState(pin));
    }
    if (outputEnable == Tristate::False) {
        if (pin != 8 && pin != 9) {
            return (Tristate::Undefined);
        }
    }
    switch (pin) {
        case 3:  // Q1
            return ((m_storageRegister >> 0) & 0x01) ? Tristate::True : Tristate::False;
        case 4:  // Q2
            return ((m_storageRegister >> 1) & 0x01) ? Tristate::True : Tristate::False;
        case 5:  // Q3
            return ((m_storageRegister >> 2) & 0x01) ? Tristate::True : Tristate::False;
        case 6:  // Q4
            return ((m_storageRegister >> 3) & 0x01) ? Tristate::True : Tristate::False;
        case 13: // Q5
            return ((m_storageRegister >> 4) & 0x01) ? Tristate::True : Tristate::False;
        case 12: // Q6
            return ((m_storageRegister >> 5) & 0x01) ? Tristate::True : Tristate::False;
        case 11: // Q7
            return ((m_storageRegister >> 6) & 0x01) ? Tristate::True : Tristate::False;
        case 10: // Q8
            return ((m_storageRegister >> 7) & 0x01) ? Tristate::True : Tristate::False;
        case 8:  // QS (Serial output)
            return (m_serialOut);
        case 9:  // QS' (Complement of serial output)
            return (~m_serialOut);
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4094::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    Tristate strobe = getInputState(0);
    Tristate data = getInputState(1);
    Tristate clock = getInputState(2);

    // Serial out is always the last bit of the shift register
    m_serialOut = ((m_shiftRegister >> 7) & 0x01) ? Tristate::True : Tristate::False;

    if (m_lastClock == Tristate::False && clock == Tristate::True) {
        m_shiftRegister = m_shiftRegister << 1;

        if (data == Tristate::True) {
            m_shiftRegister |= 0x01;
        } else {
            m_shiftRegister &= ~0x01;
        }
        m_shiftRegister &= 0xFF;
    }
    if (m_lastStrobe == Tristate::False && strobe == Tristate::True) {
        m_storageRegister = m_shiftRegister;
    }
    m_lastClock = clock;
    m_lastStrobe = strobe;

    for (size_t pin : {3, 4, 5, 6, 8, 9, 10, 11, 12, 13}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components