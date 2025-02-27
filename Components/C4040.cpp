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
    , m_counter(name + "_counter")
{
    m_pins[10] = Pin(Pin::Type::INPUT);
    m_pins[11] = Pin(Pin::Type::INPUT);

    m_pins[1]  = Pin(Pin::Type::OUTPUT);
    m_pins[2]  = Pin(Pin::Type::OUTPUT);
    m_pins[3]  = Pin(Pin::Type::OUTPUT);
    m_pins[4]  = Pin(Pin::Type::OUTPUT);
    m_pins[5]  = Pin(Pin::Type::OUTPUT);
    m_pins[6]  = Pin(Pin::Type::OUTPUT);
    m_pins[7]  = Pin(Pin::Type::OUTPUT);
    m_pins[9]  = Pin(Pin::Type::OUTPUT);
    m_pins[12] = Pin(Pin::Type::OUTPUT);
    m_pins[13] = Pin(Pin::Type::OUTPUT);
    m_pins[14] = Pin(Pin::Type::OUTPUT);
    m_pins[15] = Pin(Pin::Type::OUTPUT);

    m_pins[0] = Pin(Pin::Type::ELECTRICAL);
    m_pins[8]  = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
void C4040::initializeLinks(void)
{
    if (m_initialized)
        return;
    auto self = shared_from_this();

    m_counter.setLink(0, self, 10);
    m_counter.setLink(1, self, 11);

    m_initialized = true;
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4040::compute(size_t pin)
{
    if (!m_initialized)
        initializeLinks();

    switch (pin) {
        case 8:
        case 0:
            return (Tristate::Undefined);
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
        case 12:
        case 13:
        case 14:
        case 15:
            return (m_counter.compute(pin));
        case 10:
        case 11:
            return (getInputState(pin));
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4040::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    m_counter.simulate(tick);

    for (size_t pin : {9, 7, 6, 5, 3, 2, 4, 13, 12, 14, 15, 1}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components