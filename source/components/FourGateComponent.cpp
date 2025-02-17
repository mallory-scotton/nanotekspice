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
#include "components/FourGateComponent.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
FourGateComponent::FourGateComponent(const std::string& name)
    : AComponent(name, 14)
{
    for (int i = 0; i < 4; i++)
        m_gates[i] = nullptr;
}

FourGateComponent::~FourGateComponent()
{
    for (int i = 0; i < 4; i++)
        if (m_gates[i])
            delete m_gates[i];
}

///////////////////////////////////////////////////////////////////////////////
void FourGateComponent::simulate(size_t tick)
{
    (void)tick;
}

///////////////////////////////////////////////////////////////////////////////
Tristate FourGateComponent::compute(size_t pin)
{
    (void)pin;
    size_t min = 0;
    if (pin < 1 || pin > 14)
        throw std::out_of_range("Invalid pin for x4Gate component");
    if (pin < 4)
        return m_gates[0]->compute(pin);
    if (pin < 7) {
        min = pin == 4 ? 1 : pin;
        min = pin == 6 ? 5 : pin;
        return m_gates[1]->compute(pin - min);
    }
    if (pin == 7)
        return (Tristate::Undefined);
    if (pin < 11)
        return m_gates[2]->compute(pin - 7);
    if (pin < 14) {
        min = pin == 11 ? 8 : pin;
        min = pin == 13 ? 12 : pin;
        return m_gates[3]->compute(pin - min);
    }
    return (Tristate::Undefined);
}

} // namespace nts
