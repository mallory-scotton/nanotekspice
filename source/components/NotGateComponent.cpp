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
#include "components/NotGateComponent.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
NotGateComponent::NotGateComponent(const std::string& name)
    : AComponent(name, 2)
{}

///////////////////////////////////////////////////////////////////////////////
Tristate NotGateComponent::getInputPinValue(size_t pin)
{
    if (!m_links[pin - 1].first)
        return (Tristate::Undefined);
    return (m_links[pin - 1].first->compute(m_links[pin - 1].second));
}

///////////////////////////////////////////////////////////////////////////////
void NotGateComponent::simulate(size_t tick)
{
    (void)tick;
}

///////////////////////////////////////////////////////////////////////////////
Tristate NotGateComponent::compute(size_t pin)
{
    if (pin < 1 || pin > 2)
        throw std::out_of_range("Invalid pin for NOT gate");
    if (pin == 2) {
        Tristate input = getInputPinValue(1);

        if (input == Tristate::Undefined)
            return (Tristate::Undefined);
        return (input == Tristate::True ? Tristate::False : Tristate::True);
    }
    return (getInputPinValue(pin));
}

} // namespace nts
