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
    , m_shiftRegister(0xFF)
    , m_storageRegister(0xFF)
    , m_lastStrobe(Tristate::Undefined)
    , m_serialOut(Tristate::True)
    , m_previousSerialOut(Tristate::True)
{
    // Input pins
    m_pins[0] = Pin(Pin::Type::INPUT, "STROBE");    // STROBE
    m_pins[1] = Pin(Pin::Type::INPUT, "DATA");      // DATA
    m_pins[2] = Pin(Pin::Type::INPUT, "CLOCK");     // CLOCK
    m_pins[14] = Pin(Pin::Type::INPUT, "OE");       // Output Enable

    // Output pins (Q1-Q8 parallel outputs)
    m_pins[3] = Pin(Pin::Type::OUTPUT, "Q1");       // Q1
    m_pins[4] = Pin(Pin::Type::OUTPUT, "Q2");       // Q2
    m_pins[5] = Pin(Pin::Type::OUTPUT, "Q3");       // Q3
    m_pins[6] = Pin(Pin::Type::OUTPUT, "Q4");       // Q4
    m_pins[13] = Pin(Pin::Type::OUTPUT, "Q5");      // Q5
    m_pins[12] = Pin(Pin::Type::OUTPUT, "Q6");      // Q6
    m_pins[11] = Pin(Pin::Type::OUTPUT, "Q7");      // Q7
    m_pins[10] = Pin(Pin::Type::OUTPUT, "Q8");      // Q8

    // Serial outputs
    m_pins[8] = Pin(Pin::Type::OUTPUT, "QS");       // QS
    m_pins[9] = Pin(Pin::Type::OUTPUT, "QS'");      // QS'

    // Power and Ground
    m_pins[7] = Pin(Pin::Type::ELECTRICAL, "VSS");  // VSS
    m_pins[15] = Pin(Pin::Type::ELECTRICAL, "VDD"); // VDD
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4094::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();

    if (pin == 7 || pin == 15) {
        return (Tristate::Undefined);
    }

    if (pin == 0 || pin == 1 || pin == 2 || pin == 14) {
        return (getInputState(pin));
    }

    Tristate outputEnable = getInputState(14);

    if (outputEnable == Tristate::False) {
        return (Tristate::Undefined);
    }

    if (outputEnable == Tristate::Undefined) {
        return (Tristate::Undefined);
    }

    switch (pin) {
        case 3:  return ((m_storageRegister >> 0) & 0x01) ? True : False; // Q1
        case 4:  return ((m_storageRegister >> 1) & 0x01) ? True : False; // Q2
        case 5:  return ((m_storageRegister >> 2) & 0x01) ? True : False; // Q3
        case 6:  return ((m_storageRegister >> 3) & 0x01) ? True : False; // Q4
        case 13: return ((m_storageRegister >> 4) & 0x01) ? True : False; // Q5
        case 12: return ((m_storageRegister >> 5) & 0x01) ? True : False; // Q6
        case 11: return ((m_storageRegister >> 6) & 0x01) ? True : False; // Q7
        case 10: return ((m_storageRegister >> 7) & 0x01) ? True : False; // Q8
        case 8:  return m_serialOut;            // QS (Serial Output)
        case 9:  return m_previousSerialOut;    // QS' (Delayed Serial Output)
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

    if (clock == Tristate::Undefined || data == Tristate::Undefined) {
        // Don't update shift register with undefined inputs
    } else {
        if (m_lastClock == Tristate::False && clock == Tristate::True) {
            m_previousSerialOut = m_serialOut;

            m_serialOut = ((m_shiftRegister >> 7) & 0x01) ? True : False;

            m_shiftRegister = ((m_shiftRegister << 1) & 0xFE);
            if (data == Tristate::True) {
                m_shiftRegister |= 0x01;
            }
        }
        else if (m_lastClock == Tristate::True && clock == Tristate::False) {
            m_previousSerialOut = m_serialOut;
        }
    }

    if (strobe != Tristate::Undefined) {
        if (m_lastStrobe == Tristate::True && strobe == Tristate::False) {
            m_storageRegister = m_shiftRegister;
        }
    }

    m_lastClock = clock;
    m_lastStrobe = strobe;

    for (size_t pin : {3, 4, 5, 6, 8, 9, 10, 11, 12, 13}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components