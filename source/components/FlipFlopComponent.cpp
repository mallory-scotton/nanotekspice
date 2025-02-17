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
#include "components/FlipFlopComponent.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
FlipFlopComponent::FlipFlopComponent(const std::string& name)
    : AComponent(name, 6)
{}

///////////////////////////////////////////////////////////////////////////////
void FlipFlopComponent::simulate(size_t tick)
{
    (void)tick;
}

Tristate FlipFlopComponent::getInputPinValue(size_t pin)
{
    if (!m_links[pin - 1].first)
        return (Tristate::Undefined);
    return (m_links[pin - 1].first->compute(m_links[pin - 1].second));
}

Tristate FlipFlopComponent::calculus(size_t pin)
{
    if (getInputPinValue(4) == Tristate::False && getInputPinValue(6) == False) {
        Q = Tristate::Undefined;
        QReversed = Tristate::Undefined;
        return (Tristate::Undefined);
    }
    if (getInputPinValue(4) == Tristate::False) {
        Q = Tristate::False;
        QReversed = Tristate::True;
        return (pin == 1 ? Q : QReversed);
    }
    if (getInputPinValue(6) == Tristate::False) {
        Q = Tristate::True;
        QReversed = Tristate::False;
        return (pin == 1 ? Q : QReversed);
    }
    if (getInputPinValue(3) == Tristate::True) {
        Q = getInputPinValue(5);
        QReversed = Q == Tristate::True ? Tristate::False: Tristate::True;
        QReversed = Q == Tristate::Undefined ? Q : QReversed; 
    }
    return (pin == 1 ? Q : QReversed);
}

///////////////////////////////////////////////////////////////////////////////
Tristate FlipFlopComponent::compute(size_t pin)
{
    if (pin < 1 || pin > 6)
        throw std::out_of_range("Invalid pin for false component");
    if (pin == 1 || pin == 2)
        return calculus(pin);
    return (getInputPinValue(pin));
}

} // namespace nts
