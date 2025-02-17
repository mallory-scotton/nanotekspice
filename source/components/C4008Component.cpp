///////////////////////////////////////////////////////////////////////////////
///
/// MIT License
///
/// Copyright(c) 2025 TekyoDrift
///
/// Permission is hereby granted, free of charge, to any person obtaining a
/// copy of this software and associated documentation files (the "Software"),
/// to deal in the Software without restriction, including without limitation
/// the rights to use, copy, modify, merge, publish, distribute, sublicense,
/// and/or sell copies of the Software, and to permit persons to whom the
/// Software is furnished to do so, subject to the following coditions:
///
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software?
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
/// DEALINGS IN THE SOFTWARE.
///
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "components/C4008Component.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
C4008Component::C4008Component(const std::string& name)
    : AComponent(name, 16)
{}

///////////////////////////////////////////////////////////////////////////////
void C4008Component::simulate(size_t tick)
{
    (void)tick;
}

Tristate C4008Component::getInputPinValue(size_t pin)
{
    if (!m_links[pin - 1].first)
        return (Tristate::Undefined);
    return (m_links[pin - 1].first->compute(m_links[pin - 1].second));
}

Tristate C4008Component::intToTristate(int n)
{
    return (n == 0 ? Tristate::False : Tristate::True);
}

Tristate C4008Component::calculus(size_t pin)
{
    if (getInputPinValue(1) == Tristate::Undefined ||
        getInputPinValue(2) == Tristate::Undefined ||
        getInputPinValue(3) == Tristate::Undefined ||
        getInputPinValue(4) == Tristate::Undefined ||
        getInputPinValue(5) == Tristate::Undefined ||
        getInputPinValue(6) == Tristate::Undefined ||
        getInputPinValue(7) == Tristate::Undefined ||
        getInputPinValue(9) == Tristate::Undefined ||
        getInputPinValue(15) == Tristate::Undefined
    )
        return (Tristate::Undefined);
    int A = getInputPinValue(7) << 0 |
        getInputPinValue(5) << 1 |
        getInputPinValue(3) << 2 |
        getInputPinValue(1) << 3;
    int B = getInputPinValue(6) << 0 |
        getInputPinValue(4) << 1 |
        getInputPinValue(5) << 2 |
        getInputPinValue(15) << 3;
    int carry_in = getInputPinValue(9);
    int sum = A + B + carry_in;

    if (pin == 10)
        return intToTristate((sum >> 0) & 1);
    if (pin == 11)
        return intToTristate((sum >> 1) & 1);
    if (pin == 12)
        return intToTristate((sum >> 2) & 1);
    if (pin == 13)
        return intToTristate((sum >> 3) & 1);
    return intToTristate((sum >> 4) & 1);
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4008Component::compute(size_t pin)
{
    if (pin < 1 || pin > 15 || pin == 8)
        throw std::out_of_range("Invalid pin for 4008 component");
    if (pin == 10 || pin == 11 || pin == 12 || pin == 13 || pin == 14)
        return calculus(pin);
    return (getInputPinValue(pin));
}

} // namespace nts