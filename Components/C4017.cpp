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
    , last_ff4(Tristate::True)
{
    m_pins[12] = Pin(Pin::Type::INPUT, "CP0");
    m_pins[13] = Pin(Pin::Type::INPUT, "CP1");
    m_pins[14] = Pin(Pin::Type::INPUT, "MR");

    m_pins[2]  = Pin(Pin::Type::OUTPUT, "Q0");
    m_pins[1]  = Pin(Pin::Type::OUTPUT, "Q1");
    m_pins[3]  = Pin(Pin::Type::OUTPUT, "Q2");
    m_pins[6]  = Pin(Pin::Type::OUTPUT, "Q3");
    m_pins[9]  = Pin(Pin::Type::OUTPUT, "Q4");
    m_pins[0]  = Pin(Pin::Type::OUTPUT, "Q5");
    m_pins[4]  = Pin(Pin::Type::OUTPUT, "Q6");
    m_pins[5]  = Pin(Pin::Type::OUTPUT, "Q7");
    m_pins[8]  = Pin(Pin::Type::OUTPUT, "Q8");
    m_pins[10] = Pin(Pin::Type::OUTPUT, "Q9");
    m_pins[11] = Pin(Pin::Type::OUTPUT, "Q5-9");

    m_pins[7]  = Pin(Pin::Type::ELECTRICAL, "VSS");
    m_pins[15] = Pin(Pin::Type::ELECTRICAL, "VDD");
}

///////////////////////////////////////////////////////////////////////////////
void C4017::initializeLinks(void)
{
    if (m_initialized)
        return;
    auto self = shared_from_this();

    m_flip_flops[0]->setLink(0, self, 13);
    m_flip_flops[0]->setLink(1, m_flip_flops[4], 4);
    m_flip_flops[0]->setLink(2, self, 14);

    m_flip_flops[1]->setLink(0, self, 13);
    m_flip_flops[1]->setLink(1, m_flip_flops[0], 5);
    m_flip_flops[1]->setLink(3, self, 14);

    m_flip_flops[2]->setLink(0, self, 13);
    m_flip_flops[2]->setLink(1, m_flip_flops[1], 5);
    m_flip_flops[2]->setLink(3, self, 14);

    m_flip_flops[3]->setLink(0, self, 13);
    m_flip_flops[3]->setLink(1, m_flip_flops[2], 5);
    m_flip_flops[3]->setLink(3, self, 14);

    m_flip_flops[4]->setLink(0, self, 13);
    m_flip_flops[4]->setLink(1, m_flip_flops[3], 5);
    m_flip_flops[4]->setLink(3, self, 14);

    m_initialized = true;
}

Tristate C4017::decoder(size_t pin)
{
    bool bit_1 = (m_flip_flops[0]->compute(4) == Tristate::True);
    bool bit_2 = (m_flip_flops[1]->compute(4) == Tristate::True);
    bool bit_3 = (m_flip_flops[2]->compute(4) == Tristate::True);
    bool bit_4 = (m_flip_flops[3]->compute(4) == Tristate::True);
    bool bit_5 = (m_flip_flops[4]->compute(4) == Tristate::True);

    if ((pin == 0 || pin == 11) && (bit_1 && bit_2 && bit_3 && bit_4 && bit_5)) // count 5
        return (Tristate::True);
    if ((pin == 1 || pin == 11) && (bit_1 && !bit_2 && !bit_3 && !bit_4 && !bit_5)) // count 1
        return (Tristate::True);
    if ((pin == 2 || pin == 11) && (!bit_1 && !bit_2 && !bit_3 && !bit_4 && !bit_5)) // count 0
        return (Tristate::True);
    if (pin == 3 && (bit_1 && bit_2 && !bit_3 && !bit_4 && !bit_5)) // count 2
        return (Tristate::True);
    if (pin == 4 && (!bit_1 && bit_2 && bit_3 && bit_4 && bit_5)) // count 6
        return (Tristate::True);
    if (pin == 5 && (!bit_1 && !bit_2 && bit_3 && bit_4 && bit_5)) // count 7
        return (Tristate::True);
    if ((pin == 6 || pin == 11) && (bit_1 && bit_2 && bit_3 && !bit_4 && !bit_5)) // count 3
        return (Tristate::True);
    if (pin == 8 && (!bit_1 && !bit_2 && !bit_3 && bit_4 && bit_5)) // count 8
        return (Tristate::True);
    if ((pin == 9 || pin == 11) && (bit_1 && bit_2 && bit_3 && bit_4 && !bit_5)) // count 4
        return (Tristate::True);
    if (pin == 10 && (!bit_1 && !bit_2 && !bit_3 && !bit_4 && bit_5)) // count 9
        return (Tristate::True);
    return Tristate::False;
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4017::compute(size_t pin)
{
    if (!m_initialized)
        initializeLinks();

    switch (pin) {
        case 7:
        case 15:
            return Tristate::Undefined;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 11:
            return (decoder(pin));
        case 12:
        case 13:
        case 14:
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

    m_flip_flops[4]->simulate(tick);
    m_flip_flops[3]->simulate(tick);
    m_flip_flops[2]->simulate(tick);
    m_flip_flops[1]->simulate(tick);
    m_flip_flops[0]->simulate(tick);

    for (size_t pin : {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11}) {
        propagateOutput(pin, compute(pin));
    }
}

} // namespace nts::Components