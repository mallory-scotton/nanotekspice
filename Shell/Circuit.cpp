///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Circuit.hpp"
#include "Errors/ComponentException.hpp"
#include "Specials/Clock.hpp"
#include "Specials/Input.hpp"
#include "Specials/Output.hpp"
#include <algorithm>
#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void Circuit::addComponent(const std::string& type, const std::string& name)
{
    if (m_components.find(name) != m_components.end())
        throw ComponentException("Component already exists: " + name);
    m_components[name] = m_factory.createComponent(type, name);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::setLink(
    const std::string& component1,
    size_t pin1,
    const std::string& component2,
    size_t pin2
)
{
    Component c1 = getComponent(component1);
    Component c2 = getComponent(component2);
    c1->setLink(pin1, c2, pin2);
    c2->setLink(pin2, c1, pin1);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::simulate(void)
{
    for (const auto& [name, value] : m_pendingInputs) {
        Component component = getComponent(name);

        auto input = std::dynamic_pointer_cast<Specials::Input>(component);
        auto clock = std::dynamic_pointer_cast<Specials::Clock>(component);

        if (input)      input->setValue(value);
        else if (clock) clock->setValue(value);
    }
    m_pendingInputs.clear();

    m_tick++;

    for (auto& [name, component] : m_components)
        component->simulate(m_tick);
}

///////////////////////////////////////////////////////////////////////////////
Circuit::Component Circuit::getComponent(const std::string& name)
{
    auto it = m_components.find(name);
    if (it == m_components.end())
        throw ComponentException("Unknown component: " + name);
    return (it->second);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::setValue(const std::string& name, Tristate value)
{
    Component component = getComponent(name);

    auto input = std::dynamic_pointer_cast<Specials::Input>(component);
    auto clock = std::dynamic_pointer_cast<Specials::Clock>(component);

    if (input || clock) {
        m_pendingInputs[name] = value;
        return;
    }

    throw ComponentException("Component is not an input or clock: " + name);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::displayInputs(void) const
{
    std::vector<std::string> inputs;
    for (const auto& [name, component] : m_components) {
        if (
            std::dynamic_pointer_cast<Specials::Input>(component) ||
            std::dynamic_pointer_cast<Specials::Clock>(component)
        ) inputs.push_back(name);
    }
    std::sort(inputs.begin(), inputs.end());

    std::cout << "input(s):" << std::endl;
    for (const auto& name : inputs)
        std::cout << "  " << name << ": "
                  << m_components.at(name)->compute(0) << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::displayOutputs(void) const
{
    std::vector<std::string> outputs;
    for (const auto& [name, component] : m_components) {
        if (std::dynamic_pointer_cast<Specials::Output>(component))
            outputs.push_back(name);
    }
    std::sort(outputs.begin(), outputs.end());

    std::cout << "output(s):" << std::endl;
    for (const auto& name : outputs)
        std::cout << "  " << name << ": "
                  << m_components.at(name)->compute(0) << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::display(void) const
{
    std::cout << "tick: " << m_tick << std::endl;
    displayInputs();
    displayOutputs();
}

///////////////////////////////////////////////////////////////////////////////
const Circuit::ComponentMap& Circuit::getComponents(void) const
{
    return (m_components);
}

} // namespace nts
