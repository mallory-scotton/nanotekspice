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
#include "Circuit.hpp"
#include "ComponentFactory.hpp"
#include "components/InputComponent.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void Circuit::addComponent(const std::string& type, const std::string& name)
{
    if (m_components.find(name) != m_components.end())
        throw std::runtime_error("Component already exists: " + name);
    m_components[name] = ComponentFactory::createComponent(type, name);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::setLink(
    const std::string& comp1,
    size_t pin1,
    const std::string& comp2,
    size_t pin2
)
{
    auto& component1 = getComponent(comp1);
    auto& component2 = getComponent(comp2);
    component1.setLink(pin1, component2, pin2);
    component2.setLink(pin2, component1, pin1);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::simulate(void)
{
    m_tick++;
    for (auto& [name, component] : m_components)
        component->simulate(m_tick);
}

///////////////////////////////////////////////////////////////////////////////
IComponent& Circuit::getComponent(const std::string& name)
{
    auto it = m_components.find(name);
    if (it == m_components.end())
        throw std::runtime_error("Unknown component: " + name);
    return *it->second;
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::setValue(const std::string& name, Tristate value)
{
    (void)name;
    (void)value;
    // auto* input = dynamic_cast<InputComponent*>(getComponent(name).get());
    // if (!input)
        // throw std::runtime_error("Component is not an input: " + name);
    // input->setValue(value);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::displayInputs(void) const
{}

///////////////////////////////////////////////////////////////////////////////
void Circuit::displayOutputs(void) const
{}

///////////////////////////////////////////////////////////////////////////////
void Circuit::display(void) const
{
    std::cout << "tick: " << m_tick << std::endl;
    displayInputs();
    displayOutputs();
}

} // namespace nts
