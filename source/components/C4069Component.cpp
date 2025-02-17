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
#include "components/C4069Component.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
C4069Component::C4069Component(const std::string& name)
    : AComponent(name, 14)
{
    for (int i = 0; i < 6; i++)
        m_gates[i] = new NotGateComponent(name);
}

C4069Component::~C4069Component()
{
    for (int i = 0; i < 6; i++)
        if (m_gates[i])
            delete m_gates[i];
}

///////////////////////////////////////////////////////////////////////////////
void C4069Component::simulate(size_t tick)
{
    (void)tick;
}

///////////////////////////////////////////////////////////////////////////////
Tristate C4069Component::compute(size_t pin)
{
    if (pin < 1 || pin > 13 || pin == 7)
        throw std::out_of_range("Invalid pin for 4069 component");
    if (pin < 7) {
        size_t gateIndex = (pin - 1) / 2;
        size_t mappedPin = ((pin - 1) % 2) + 1;
        return m_gates[gateIndex]->compute(mappedPin);
    }
    size_t gateIndex = 3 + ((pin - 8) / 2);
    size_t mappedPin = ((pin - 8) % 2) + 1;
    return m_gates[gateIndex]->compute(mappedPin);
}

void C4069Component::setLink(size_t pin, IComponent& other, size_t otherPin)
{
    if (pin > m_links.size() || pin == 0)
        throw std::out_of_range("Invalid pin number");
    if (pin < 7) {
        size_t gatePin = pin - 1;
        m_gates[(pin - 1) / 2]->setLink(gatePin % 2, other, otherPin);
        return;
    }
    size_t gatePin = pin - 7;
    m_gates[(pin - 7) / 2 + 3]->setLink(1 - gatePin, other, otherPin);
}

} // namespace nts