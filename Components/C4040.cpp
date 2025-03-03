///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4040.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4040::C4040(const std::string& name)
    : AComponent(name, 16)
    , m_count(0)
    , m_lastClock(Tristate::Undefined)
{
    m_pins[9] = Pin(Pin::Type::INPUT);
    m_pins[10] = Pin(Pin::Type::INPUT);

    m_pins[0]  = Pin(Pin::Type::OUTPUT);
    m_pins[1]  = Pin(Pin::Type::OUTPUT);
    m_pins[2]  = Pin(Pin::Type::OUTPUT);
    m_pins[3]  = Pin(Pin::Type::OUTPUT);
    m_pins[4]  = Pin(Pin::Type::OUTPUT);
    m_pins[5]  = Pin(Pin::Type::OUTPUT);
    m_pins[6]  = Pin(Pin::Type::OUTPUT);
    m_pins[8]  = Pin(Pin::Type::OUTPUT);
    m_pins[11] = Pin(Pin::Type::OUTPUT);
    m_pins[12] = Pin(Pin::Type::OUTPUT);
    m_pins[13] = Pin(Pin::Type::OUTPUT);
    m_pins[14] = Pin(Pin::Type::OUTPUT);

    m_pins[7]  = Pin(Pin::Type::ELECTRICAL);
    m_pins[15] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4040::compute(size_t pin)
{
    int bitPosition = -1;
    switch (pin) {
        case 7:
        case 15:
            return (Tristate::Undefined);
        case 0: bitPosition = 11 ; break;
        case 1: bitPosition = 5 ; break;
        case 2: bitPosition = 4 ; break;
        case 3: bitPosition = 6 ; break;
        case 4: bitPosition = 3 ; break;
        case 5: bitPosition = 2 ; break;
        case 6: bitPosition = 1 ; break;
        case 8: bitPosition = 0 ; break;
        case 11: bitPosition = 8 ; break;
        case 12: bitPosition = 7 ; break;
        case 13: bitPosition = 9 ; break;
        case 14: bitPosition = 10 ; break;
        case 9:
        case 10:
            return (getInputState(pin));
        default:
            throw OutOfRangePinException();
    }

    return ((m_count >> bitPosition) & 0x01) ? Tristate::True : Tristate::False;
}

///////////////////////////////////////////////////////////////////////////////
void C4040::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    Tristate clock = getInputState(9);
    Tristate reset = getInputState(10);

    if (reset == Tristate::True) {
        m_count = 0;
    } else {
        if (m_lastClock == Tristate::True && clock == Tristate::False) {
            m_count++;
        }
    }

    m_lastClock = clock;

}

} // namespace nts::Components