///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "AComponent.hpp"
#include "Errors/OutOfRangePinException.hpp"
#include <set>
#include <queue>
#include <string>
#include <algorithm>

#ifdef NTS_BONUS
    #include <cxxabi.h>
    #include <typeinfo>
    #include <typeindex>
    #include <unordered_map>
    #include <cmath>
    #include <random>
    #include <imgui.h>
#endif

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
std::string AComponent::getCleanClassName(void) const
{
    const char* name = typeid(*this).name();
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string cleaned(demangled);
    std::free(demangled);
    size_t pos = cleaned.rfind("::");
    if (pos != std::string::npos) {
        cleaned.erase(0, pos + 2);
    }
    return (cleaned);
}

///////////////////////////////////////////////////////////////////////////////
bool AComponent::isSelected(void) const
{
    return (m_selected);
}

///////////////////////////////////////////////////////////////////////////////
#define rgb(r, g, b) ImVec4(r / 255.f, g / 255.f, b / 255.f, 1.f)

///////////////////////////////////////////////////////////////////////////////
void AComponent::applyColors(const std::string& cls) const
{
    ImVec4 color = rgb(70, 70, 70);
    ImVec4 accent = rgb(10, 10, 10);
    ImVec4 accent2 = rgb(20, 20, 20);

    if (cls == "Input" || cls == "Output" || cls == "Clock") {
        color = rgb(124, 29, 144);
    } else if (cls == "True" || cls == "False") {
        color = rgb(90, 120, 87);
    } else if (
        cls == "Or" || cls == "Xor" || cls == "And" ||
        cls == "Nand" || cls == "Nor" || cls == "Not"
    ) {
        color = rgb(74, 115, 143);
    } else if (cls[0] == 'C') {
        color = rgb(128, 21, 20);
    } else if (cls[cls.length() - 1] == '>') {
        color = rgb(133, 103, 26);
    }

    Ez::PushStyleColor(ImNodesStyleCol_NodeTitleBarBg, color);
    Ez::PushStyleColor(ImNodesStyleCol_NodeTitleBarBgHovered, color + accent);
    Ez::PushStyleColor(ImNodesStyleCol_NodeTitleBarBgActive, color + accent2);
}

///////////////////////////////////////////////////////////////////////////////
void AComponent::draw(void)
{
    if (!m_pinsInitialized) {
        m_inputs.clear();
        m_outputs.clear();
        m_pinLabels.clear();

        m_pinLabels.reserve(m_pins.size());

        for (size_t i = 0; i < m_pins.size(); i++) {
            Pin::Type type = m_pins[i].getType();

            if (type == Pin::Type::ELECTRICAL)
                continue;

            m_pinLabels.push_back(std::to_string(i));

            if (type == Pin::Type::INPUT) {
                m_inputs.push_back((Ez::SlotInfo){
                    m_pinLabels.back().c_str(), 1
                });
            } else if (type == Pin::Type::OUTPUT) {
                m_outputs.push_back((Ez::SlotInfo){
                    m_pinLabels.back().c_str(), 1
                });
            }
        }

        m_pinsInitialized = true;
    }

    std::string cls = getCleanClassName();
    std::string name = m_name + " (" + cls + ")";

    applyColors(cls);

    if (Ez::BeginNode(this, name.c_str(), &m_position, &m_selected)) {
        if (cls == "Input" || cls == "Clock") {
            int state = (int)compute(0);
            ImGui::Text("%s", state == -1 ?
                "Undefined" : state == 0 ? "False" : "True");
        }

        Ez::InputSlots(m_inputs.data(), m_inputs.size());

        if (cls == "Output") {
            int state = (int)compute(0);
            ImGui::Text("%s", state == -1 ?
                "Undefined" : state == 0 ? "False" : "True");
        }

        Ez::OutputSlots(m_outputs.data(), m_outputs.size());
    }

    for (size_t i = 0; i < m_pins.size(); i++) {
        if (m_pins[i].getType() != Pin::Type::INPUT)
            continue;
        for (auto& link : m_pins[i].getLinks()) {
            if (auto other = link.component.lock()) {
                if (!ImNodes::Connection(
                    this,
                    std::to_string(i).c_str(),
                    other.get(),
                    std::to_string(link.pin).c_str()
                )) {
                    m_pins[i].removeLink(other, link.pin);
                }
            }
        }
    }

    Ez::EndNode();

    Ez::PopStyleColor(3);
}

#endif

} // namespace nts
