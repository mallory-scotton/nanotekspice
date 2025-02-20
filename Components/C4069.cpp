///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4069.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4069::C4069(const std::string& name)
    : AComponent(name, 14)
    , m_gates{
        Gates::Not(name + "_1"),
        Gates::Not(name + "_2"),
        Gates::Not(name + "_3"),
        Gates::Not(name + "_4"),
        Gates::Not(name + "_5"),
        Gates::Not(name + "_6")
    }
{
    m_pins[0] = Pin(Pin::Type::INPUT);
    m_pins[1] = Pin(Pin::Type::OUTPUT);
    
    m_pins[2] = Pin(Pin::Type::INPUT);
    m_pins[3] = Pin(Pin::Type::OUTPUT);

    m_pins[4] = Pin(Pin::Type::INPUT);
    m_pins[5] = Pin(Pin::Type::OUTPUT);

    m_pins[8] = Pin(Pin::Type::INPUT);
    m_pins[7] = Pin(Pin::Type::OUTPUT);

    m_pins[10] = Pin(Pin::Type::INPUT);
    m_pins[9] = Pin(Pin::Type::OUTPUT);

    m_pins[12] = Pin(Pin::Type::INPUT);
    m_pins[11] = Pin(Pin::Type::OUTPUT);

    m_pins[6] = Pin(Pin::Type::ELECTRICAL);
    m_pins[13] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4069::compute(size_t pin)
{
    switch (pin) {
        case 6:
        case 13:
            return (Tristate::Undefined);
        case 0:
        case 2:
        case 4:
        case 8:
        case 10:
        case 12:
            return (getInputState(pin));
        case 1:
        case 3:
        case 5:
            return (~(getInputState(pin - 1)));
        case 7:
        case 9:
        case 11:
            return (~(getInputState(pin + 1)));
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4069::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);
    propagateOutput(1, compute(1));
    propagateOutput(3, compute(3));
    propagateOutput(5, compute(5));
    propagateOutput(7, compute(7));
    propagateOutput(9, compute(9));
    propagateOutput(11, compute(11));
}

} // namespace nts::Components
