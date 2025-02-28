///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4017.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4017::C4017(const std::string& name)
    : AComponent(name, 16)
    , m_flip_flops{
        std::make_shared<nts::Sequencials::FlipFlop>(name + "_1"),
        std::make_shared<nts::Sequencials::FlipFlop>(name + "_2"),
        std::make_shared<nts::Sequencials::FlipFlop>(name + "_3"),
        std::make_shared<nts::Sequencials::FlipFlop>(name + "_4"),
        std::make_shared<nts::Sequencials::FlipFlop>(name + "_5"),
    }
    , m_initialized(false)
{
    m_pins[13] = Pin(Pin::Type::INPUT);
    m_pins[14] = Pin(Pin::Type::INPUT);
    m_pins[15] = Pin(Pin::Type::INPUT);

    m_pins[1]  = Pin(Pin::Type::OUTPUT);
    m_pins[2]  = Pin(Pin::Type::OUTPUT);
    m_pins[3]  = Pin(Pin::Type::OUTPUT);
    m_pins[4]  = Pin(Pin::Type::OUTPUT);
    m_pins[5]  = Pin(Pin::Type::OUTPUT);
    m_pins[6]  = Pin(Pin::Type::OUTPUT);
    m_pins[7]  = Pin(Pin::Type::OUTPUT);
    m_pins[9]  = Pin(Pin::Type::OUTPUT);
    m_pins[10] = Pin(Pin::Type::OUTPUT);
    m_pins[11] = Pin(Pin::Type::OUTPUT);
    m_pins[12] = Pin(Pin::Type::OUTPUT);

    m_pins[0]  = Pin(Pin::Type::ELECTRICAL);
    m_pins[8]  = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
void C4017::initializeLinks(void)
{
    if (m_initialized)
        return;
    auto self = shared_from_this();

    m_flip_flops[0]->setLink(0, self, 14);
    m_flip_flops[0]->setLink(1, m_flip_flops[4], 5);
    m_flip_flops[0]->setLink(3, self, 15);

    m_flip_flops[1]->setLink(0, self, 14);
    m_flip_flops[1]->setLink(1, m_flip_flops[0], 4);
    m_flip_flops[1]->setLink(3, self, 15);

    m_flip_flops[2]->setLink(0, self, 14);
    m_flip_flops[2]->setLink(1, m_flip_flops[1], 4);
    m_flip_flops[2]->setLink(3, self, 15);

    m_flip_flops[3]->setLink(0, self, 14);
    m_flip_flops[3]->setLink(1, m_flip_flops[2], 4);
    m_flip_flops[3]->setLink(3, self, 15);

    m_flip_flops[4]->setLink(0, self, 14);
    m_flip_flops[4]->setLink(1, m_flip_flops[3], 4);
    m_flip_flops[4]->setLink(3, self, 15);

    m_initialized = true;
}

Tristate C4017::decoder(size_t pin)
{
    Tristate bit_1 = m_flip_flops[0]->compute(5);
    Tristate bit_2 = m_flip_flops[1]->compute(5);
    Tristate bit_3 = m_flip_flops[2]->compute(5);
    Tristate bit_4 = m_flip_flops[3]->compute(5);
    Tristate bit_5 = m_flip_flops[4]->compute(5);
    if (pin == 1 && (bit_1 && bit_2 && bit_3 && bit_4 && bit_5)) // count 5
        return Tristate::True;
    if (pin == 2 && (bit_1 && !bit_2 && !bit_3 && !bit_4 && !bit_5)) // count 1
        return Tristate::True;
    if (pin == 3 && (!bit_1 && !bit_2 && !bit_3 && !bit_4 && !bit_5)) // count 0
        return Tristate::True;
    if (pin == 4 && (bit_1 && bit_2 && !bit_3 && !bit_4 && !bit_5)) // count 2
        return Tristate::True;
    if (pin == 5 && (!bit_1 && bit_2 && bit_3 && bit_4 && bit_5)) // count 6
        return Tristate::True;
    if (pin == 6 && (!bit_1 && !bit_2 && bit_3 && bit_4 && bit_5)) // count 7
        return Tristate::True;
    if (pin == 7 && (bit_1 && bit_2 && bit_3 && !bit_4 && !bit_5)) // count 3
        return Tristate::True;
    if (pin == 9 && (!bit_1 && !bit_2 && !bit_3 && bit_4 && bit_5)) // count 8
        return Tristate::True;
    if (pin == 10 && (bit_1 && bit_2 && bit_3 && bit_4 && !bit_5)) // count 4
        return Tristate::True;
    if (pin == 11 && (!bit_1 && !bit_2 && !bit_3 && !bit_4 && bit_5)) // count 9
        return Tristate::True;
    return Tristate::False;
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4017::compute(size_t pin)
{
    if (!m_initialized)
        initializeLinks();

    switch (pin) {
        case 0:
        case 8:
            return Tristate::Undefined;
        case 3:
        case 2:
        case 4:
        case 7:
        case 10:
        case 1:
        case 5:
        case 6:
        case 9:
        case 11:
        case 12:
            return decoder(pin);
        case 14:
        case 13:
        case 15:
            return getInputState(pin);
        default:
            throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4017::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    for (size_t pin : {1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components