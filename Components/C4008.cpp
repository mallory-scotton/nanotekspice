///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "C4008.hpp"
#include "Errors/OutOfRangePinException.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
C4008::C4008(const std::string& name)
    : AComponent(name, 16)
{
    m_pins[0] = Pin(Pin::Type::INPUT);
    m_pins[1] = Pin(Pin::Type::INPUT);
    m_pins[2] = Pin(Pin::Type::INPUT);
    m_pins[3] = Pin(Pin::Type::INPUT);
    m_pins[4] = Pin(Pin::Type::INPUT);
    m_pins[5] = Pin(Pin::Type::INPUT);
    m_pins[6] = Pin(Pin::Type::INPUT);
    m_pins[8] = Pin(Pin::Type::INPUT);
    m_pins[14] = Pin(Pin::Type::INPUT);

    m_pins[9] = Pin(Pin::Type::OUTPUT);
    m_pins[10] = Pin(Pin::Type::OUTPUT);
    m_pins[11] = Pin(Pin::Type::OUTPUT);
    m_pins[12] = Pin(Pin::Type::OUTPUT);
    m_pins[13] = Pin(Pin::Type::OUTPUT);

    m_pins[7] = Pin(Pin::Type::ELECTRICAL);
    m_pins[15] = Pin(Pin::Type::ELECTRICAL);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4008::fullAdder(Tristate a, Tristate b, Tristate cin, Tristate& cout)
{
    if (
        a == Tristate::Undefined ||
        b == Tristate::Undefined ||
        cin == Tristate::Undefined
    ) {
        cout = Tristate::Undefined;
        return (Tristate::Undefined);
    }

    bool a_val = a == Tristate::True;
    bool b_val = b == Tristate::True;
    bool cin_val = cin == Tristate::True;

    bool sum_val = a_val ^ b_val ^ cin_val;

    bool cout_val = (a_val && b_val) || (cin_val && (a_val ^ b_val));

    cout = cout_val ? Tristate::True : Tristate::False;
    return (sum_val ? Tristate::True : Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4008::compute(size_t pin)
{
    if (pin >= 16) {
        throw OutOfRangePinException();
    }

    if (pin == 7 || pin == 15) {
        return (Tristate::Undefined);
    }

    if (pin == 0 || pin == 1 || pin == 2 || pin == 3 || pin == 4 ||
        pin == 5 || pin == 6 || pin == 8 || pin == 14) {
        return (getInputState(pin));
    }

    Tristate a1 = getInputState(6);
    Tristate b1 = getInputState(5);
    Tristate a2 = getInputState(4);
    Tristate b2 = getInputState(3);
    Tristate a3 = getInputState(2);
    Tristate b3 = getInputState(1);
    Tristate a4 = getInputState(0);
    Tristate b4 = getInputState(14);
    Tristate cin = getInputState(8);

    Tristate carry = cin;
    Tristate cout;

    Tristate sum0 = fullAdder(a1, b1, carry, carry);
    Tristate sum1 = fullAdder(a2, b2, carry, carry);
    Tristate sum2 = fullAdder(a3, b3, carry, carry);
    Tristate sum3 = fullAdder(a4, b4, carry, cout);

    switch (pin) {
        case 9:  return (sum0);
        case 10: return (sum1);
        case 11: return (sum2);
        case 12: return (sum3);
        case 13: return (cout);
        default: throw OutOfRangePinException();
    }
}

///////////////////////////////////////////////////////////////////////////////
void C4008::simulate(size_t tick)
{
    if (m_tick == tick)
        return;

    AComponent::simulate(tick);

    propagateOutput(9, compute(9));
    propagateOutput(10, compute(10));
    propagateOutput(11, compute(11));
    propagateOutput(12, compute(12));
    propagateOutput(13, compute(13));
}

} // namespace nts::Components
