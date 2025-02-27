///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "AComponent.hpp"
#include "Errors/OutOfRangePinException.hpp"
#include <set>
#include <queue>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
AComponent::AComponent(const std::string& name, size_t count)
    : m_name(name)
    , m_pins(count)
    , m_tick(0)
{}

///////////////////////////////////////////////////////////////////////////////
void AComponent::setLink(
    size_t pin,
    std::shared_ptr<IComponent> other,
    size_t otherPin
)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    m_pins[pin].addLink(other, otherPin);
}

///////////////////////////////////////////////////////////////////////////////
void AComponent::simulate(size_t tick)
{
    m_tick = tick;
}

///////////////////////////////////////////////////////////////////////////////
const std::string AComponent::getName(void) const
{
    return (m_name);
}

///////////////////////////////////////////////////////////////////////////////
void AComponent::propagateOutput(size_t pin, Tristate state)
{
    if (pin >= m_pins.size() && m_pins[pin].getType() == Pin::Type::INPUT)
        throw ComponentException("Invalid output pin");
    m_pins[pin].setState(state);

    static std::queue<std::pair<std::weak_ptr<IComponent>, size_t>> uqueue;

    for (const auto& link : m_pins[pin].getLinks()) {
        if (auto component = link.component.lock())
            uqueue.push({link.component, link.pin});
    }

    while (!uqueue.empty()) {
        auto [weakComponent, componentPin] = uqueue.front();
        uqueue.pop();
        if (auto component = weakComponent.lock()) {
            component->compute(componentPin);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
Tristate AComponent::getInputState(size_t pin)
{
    static thread_local std::set<const IComponent*> computingComponents;

    if (pin >= m_pins.size() || m_pins[pin].getType() == Pin::Type::OUTPUT)
        throw ComponentException("Invalid input pin");
    Tristate result = Tristate::Undefined;
    for (const auto& link : m_pins[pin].getLinks()) {
        if (auto component = link.component.lock()) {
            if (
                computingComponents.find(component.get()) !=
                computingComponents.end()
            ) {
                return (Tristate::Undefined);
            }

            computingComponents.insert(component.get());
            result = component->compute(link.pin);
            computingComponents.erase(component.get());

            break;
        }
    }
    return (result);
}

} // namespace nts
