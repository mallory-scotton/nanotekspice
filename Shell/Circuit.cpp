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
#include <unordered_set>

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

#ifdef NTS_BONUS

///////////////////////////////////////////////////////////////////////////////
void Circuit::draw(sf::RenderTarget& target)
{
    positionComponents();

    for (const auto& [name, component] : m_components) {
        component->draw(target);
    }

    drawLinks(target);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::drawLinks(sf::RenderTarget& target)
{
    std::unordered_set<std::string> drawnLinks;
    sf::VertexArray lines(sf::PrimitiveType::Lines);

    for (const auto& [name, component] : m_components) {
        for (size_t i = 0; i < component->getPinCount(); i++) {

            const Pin& pin = component->getPin(i);


            if (pin.getType() == Pin::Type::OUTPUT) {
                for (const auto& link : pin.getLinks()) {
                    if (auto linkedComp = link.component.lock()) {
                        std::string linkId = name + ":" + std::to_string(i) +
                                             "->" + linkedComp->getName() + ":" +
                                             std::to_string(link.pin);

                        if (drawnLinks.find(linkId) == drawnLinks.end()) {
                            sf::Vertex startVertex;
                            startVertex.position = component->getPinPosition(i);
                            startVertex.color = sf::Color::White;

                            sf::Vertex endVertex;
                            endVertex.position = linkedComp->getPinPosition(link.pin);
                            endVertex.color = sf::Color::White;

                            lines.append(startVertex);
                            lines.append(endVertex);
                            drawnLinks.insert(linkId);
                        }
                    }
                }
            }
        }
    }

    target.draw(lines);
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::positionComponents(void)
{
    const float startX = 50.f;
    const float startY = 50.f;
    const float spacingX = 200.f;
    const float spacingY = 150.f;

    size_t row = 0;
    size_t col = 0;
    size_t maxCol = 4;

    for (auto& [name, component] : m_components) {
        if (!component->isPositionSet()) {
            component->setPosition(
                {startX + col * spacingX, startY + row * spacingY}
            );

            col++;
            if (col >= maxCol) {
                col = 0;
                row++;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
std::shared_ptr<nts::IComponent> Circuit::getComponentAt(
    const sf::Vector2f& position
)
{
    for (const auto& [name, component] : m_components) {
        sf::Vector2f compPos = component->getPosition();
        sf::Vector2f compSize = component->getSize();

        sf::FloatRect bounds(compPos, compSize);

        if (bounds.contains(position)) {
            return (component);
        }
    }

    return (nullptr);
}

#endif

} // namespace nts
