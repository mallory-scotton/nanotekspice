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

///////////////////////////////////////////////////////////////////////////////
std::vector<Pin>& AComponent::getPins(void)
{
    return (m_pins);
}

#ifdef NTS_BONUS

///////////////////////////////////////////////////////////////////////////////
size_t AComponent::getPinCount(void) const
{
    return (m_pins.size());
}

///////////////////////////////////////////////////////////////////////////////
const Pin& AComponent::getPin(size_t idx) const
{
    return (m_pins[idx]);
}

///////////////////////////////////////////////////////////////////////////////
void AComponent::setPosition(const sf::Vector2f& position)
{
    m_position = position;
    m_positionSet = true;

    updatePinPositions();
}

///////////////////////////////////////////////////////////////////////////////
sf::Vector2f AComponent::getPosition(void) const
{
    return (m_position);
}

///////////////////////////////////////////////////////////////////////////////
sf::Vector2f AComponent::getSize(void) const
{
    return (m_size);
}

///////////////////////////////////////////////////////////////////////////////
sf::Vector2f AComponent::getPinPosition(size_t pin) const
{
    if (pin >= m_pinPositions.size())
        return (m_position);
    return (m_pinPositions[pin]);
}

///////////////////////////////////////////////////////////////////////////////
bool AComponent::isPositionSet(void) const
{
    return (m_positionSet);
}

///////////////////////////////////////////////////////////////////////////////
void AComponent::updatePinPositions(void)
{
    size_t inputCount = 0;
    size_t outputCount = 0;

    for (const auto& pin : m_pins) {
        if (pin.getType() == Pin::Type::INPUT)
            inputCount++;
        else if (pin.getType() == Pin::Type::OUTPUT)
            outputCount++;
    }

    float height = std::max(inputCount, outputCount) * 20.f + 40.f;
    m_size = {150.f, height};

    m_pinPositions.resize(m_pins.size());

    size_t inputIdx = 0;
    size_t outputIdx = 0;
    size_t electricalIdx = 0;

    for (size_t i = 0; i < m_pins.size(); i++) {
        const Pin& pin = m_pins[i];

        if (pin.getType() == Pin::Type::INPUT) {
            float y = m_position.y + 30.f + inputIdx * 20.f;
            m_pinPositions[i] = {m_position.x, y};
            inputIdx++;
        } else if (pin.getType() == Pin::Type::OUTPUT) {
            float y = m_position.y + 30.f + outputIdx * 20.f;
            m_pinPositions[i] = {m_position.x + m_size.x, y};
            outputIdx++;
        } else {
            float x = m_position.x + 40.f + electricalIdx * 20.f;
            m_pinPositions[i] = {x, m_position.y + m_size.y};
            electricalIdx++;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
void AComponent::draw(sf::RenderTarget& target) const
{
    sf::RectangleShape body;
    body.setPosition(m_position);
    body.setSize(m_size);
    body.setFillColor(sf::Color(50, 50, 50));
    body.setOutlineThickness(2.f);
    body.setOutlineColor(sf::Color(20, 20, 20));
    target.draw(body);

    static sf::Font font("segoe-ui.ttf");
    sf::Text text(font, m_name, 14);

    text.setOrigin(text.getGlobalBounds().size / 2.f);

    text.setPosition({
        m_position.x + (m_size.x / 2.f),
        m_position.y + 5.f}
    );
    text.setFillColor(sf::Color::White);
    target.draw(text);

    for (size_t i = 0; i < m_pins.size(); i++) {
        const Pin& pin = m_pins[i];
        sf::CircleShape pinShape(5.f);
        pinShape.setPosition(m_pinPositions[i] - sf::Vector2f(5.f, 5.f));

        sf::Color pinColor;
        switch (pin.getState()) {
            case Tristate::True:
                pinColor = sf::Color::Green;
                break;
            case Tristate::False:
                pinColor = sf::Color::Red;
                break;
            case Tristate::Undefined:
                pinColor = sf::Color::Yellow;
                break;
        }

        pinShape.setFillColor(pinColor);
        pinShape.setOutlineThickness(1.f);
        pinShape.setOutlineColor(sf::Color::Black);
        target.draw(pinShape);

        sf::Text pinText(font, std::to_string(i), 10);
        pinText.setPosition(m_pinPositions[i] + sf::Vector2f(6.f, -10.f));
        pinText.setFillColor(sf::Color::White);
        target.draw(pinText);
    }
}

#endif

} // namespace nts
