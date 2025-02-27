///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4013.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4013::C4013(const std::string& name)
    : AComponent(name, 14)
    , m_flipflops{
        Sequencials::FlipFlop(name + "_1"),
        Sequencials::FlipFlop(name + "_2")
    }
    , m_initialized(false)
{
    m_pins[3] = Pin(Pin::Type::INPUT);
    m_pins[5] = Pin(Pin::Type::INPUT);
    m_pins[6] = Pin(Pin::Type::INPUT);
    m_pins[4] = Pin(Pin::Type::INPUT);
    m_pins[1] = Pin(Pin::Type::OUTPUT);
    m_pins[2] = Pin(Pin::Type::OUTPUT);

    m_pins[11] = Pin(Pin::Type::INPUT);
    m_pins[9] = Pin(Pin::Type::INPUT);
    m_pins[8] = Pin(Pin::Type::INPUT);
    m_pins[10] = Pin(Pin::Type::INPUT);
    m_pins[13] = Pin(Pin::Type::OUTPUT);
    m_pins[12] = Pin(Pin::Type::OUTPUT);

    m_pins[0] = Pin(Pin::Type::ELECTRICAL);
    m_pins[7] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
void C4013::initializeLinks(void)
{
    if (m_initialized)
        return;
    m_flipflops[0].setLink(0, shared_from_this(), 3);
    m_flipflops[0].setLink(1, shared_from_this(), 5);
    m_flipflops[0].setLink(2, shared_from_this(), 6);
    m_flipflops[0].setLink(3, shared_from_this(), 4);

    m_flipflops[1].setLink(0, shared_from_this(), 11);
    m_flipflops[1].setLink(1, shared_from_this(), 9);
    m_flipflops[1].setLink(2, shared_from_this(), 8);
    m_flipflops[1].setLink(3, shared_from_this(), 10);

    m_initialized = true;
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4013::compute(size_t pin)
{
    if (!m_initialized)
        initializeLinks();

    switch (pin) {
        case 0:
        case 7:
            return (Tristate::Undefined);
        case 1:
            return (m_flipflops[0].compute(4));
        case 2:
            return (m_flipflops[0].compute(5));
        case 13:
            return (m_flipflops[1].compute(4));
        case 12:
            return (m_flipflops[1].compute(5));
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 11:
            return (getInputState(pin));
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4013::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    m_flipflops[0].simulate(tick);
    m_flipflops[1].simulate(tick);

    propagateOutput(1, compute(1));
    propagateOutput(2, compute(2));
    propagateOutput(13, compute(13));
    propagateOutput(12, compute(12));
}

} // namespace nts::Components
