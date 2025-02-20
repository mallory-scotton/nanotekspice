///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "QuadGate.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
template <typename GateType>
QuadGate<GateType>::QuadGate(const std::string& name)
    : AComponent(name, 14)
    , m_gates{
        GateType(name + "_1"),
        GateType(name + "_2"),
        GateType(name + "_3"),
        GateType(name + "_4"),
    }
{
    m_pins[0] = Pin(Pin::Type::INPUT);
    m_pins[1] = Pin(Pin::Type::INPUT);
    m_pins[2] = Pin(Pin::Type::OUTPUT);

    m_pins[4] = Pin(Pin::Type::INPUT);
    m_pins[5] = Pin(Pin::Type::INPUT);
    m_pins[3] = Pin(Pin::Type::OUTPUT);

    m_pins[7] = Pin(Pin::Type::INPUT);
    m_pins[8] = Pin(Pin::Type::INPUT);
    m_pins[9] = Pin(Pin::Type::OUTPUT);

    m_pins[11] = Pin(Pin::Type::INPUT);
    m_pins[12] = Pin(Pin::Type::INPUT);
    m_pins[10] = Pin(Pin::Type::OUTPUT);

    m_pins[6] = Pin(Pin::Type::ELECTRICAL);
    m_pins[13] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
template <typename GateType>
Tristate QuadGate<GateType>::compute(size_t pin)
{
    switch (pin) {
        case 0:
        case 1:
        case 4:
        case 5:
        case 7:
        case 8:
        case 11:
        case 12:
            return (getInputState(pin));
        case 6:
        case 13:
            return (Tristate::Undefined);
        case 2:
        case 3:
        case 9:
        case 10:
            return (m_gates[pin].compute(2));
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
template <typename GateType>
void QuadGate<GateType>::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);
    for (auto& gate : m_gates)
        gate.simulate(tick);
    m_tick = tick;
}

} // namespace nts::Components
