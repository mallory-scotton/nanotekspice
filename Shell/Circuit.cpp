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
#include <queue>
#include <set>
#include <map>
#include <vector>
#ifdef NTS_BONUS
#include <imgui_stdlib.h>
#endif

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
void Circuit::clear(void)
{
    m_components.clear();
    m_gotoTick = 1;
    m_tick = 0;
    m_pendingInputs.clear();
    m_initialized = false;
}

///////////////////////////////////////////////////////////////////////////////
void Circuit::initializePosition(void) {
    const float HORIZONTAL_SPACING = 250.0f;
    const float VERTICAL_SPACING = 120.0f;

    std::vector<std::shared_ptr<IComponent>> inputComponents;
    std::set<std::shared_ptr<IComponent>> processedComponents;

    for (const auto& [name, component] : m_components) {
        bool hasInputConnection = false;

        auto* aComponent = dynamic_cast<AComponent*>(component.get());
        if (aComponent) {
            auto& pins = aComponent->getPins();
            for (size_t i = 0; i < pins.size(); i++) {
                if (pins[i].getType() == nts::Pin::Type::INPUT &&
                    !pins[i].getLinks().empty()) {
                    hasInputConnection = true;
                    break;
                }
            }
        }

        if (!hasInputConnection) {
            inputComponents.push_back(component);
            processedComponents.insert(component);
        }
    }

    std::unordered_map<std::shared_ptr<IComponent>, int> componentLevels;
    std::unordered_map<int, std::vector<std::shared_ptr<IComponent>>>
        levelToComponents;

    for (auto& component : inputComponents) {
        componentLevels[component] = 0;
        levelToComponents[0].push_back(component);
    }

    std::queue<std::shared_ptr<IComponent>> queue;
    for (auto& component : inputComponents) {
        queue.push(component);
    }

    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        int currentLevel = componentLevels[current];

        auto* aComponent = dynamic_cast<AComponent*>(current.get());
        if (aComponent) {
            auto& pins = aComponent->getPins();
            for (size_t i = 0; i < pins.size(); i++) {
                if (pins[i].getType() != nts::Pin::Type::OUTPUT) continue;

                for (auto& link : pins[i].getLinks()) {
                    if (auto connectedComp = link.component.lock()) {
                        if (processedComponents.find(connectedComp) ==
                            processedComponents.end()) {
                            int nextLevel = currentLevel + 1;
                            componentLevels[connectedComp] = nextLevel;
                            levelToComponents[nextLevel].push_back(
                                connectedComp);

                            processedComponents.insert(connectedComp);
                            queue.push(connectedComp);
                        }
                    }
                }
            }
        }
    }

    int maxLevel = 0;
    for (const auto& [level, comps] : levelToComponents) {
        maxLevel = std::max(maxLevel, level);
    }

    for (const auto& [name, component] : m_components) {
        if (processedComponents.find(component) == processedComponents.end()) {
            componentLevels[component] = maxLevel + 1;
            levelToComponents[maxLevel + 1].push_back(component);
        }
    }

    for (const auto& [level, components] : levelToComponents) {
        float x = 100.0f + level * HORIZONTAL_SPACING;

        for (size_t i = 0; i < components.size(); i++) {
            float y = 100.0f + i * VERTICAL_SPACING;

            auto* aComponent = dynamic_cast<AComponent*>(components[i].get());
            if (aComponent) {
                aComponent->m_position = ImVec2(x, y);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
static const std::map<
    std::string,
    std::map<std::string, std::string>
> RegisteredComponents = {
    {
        "Gates",
        {
            {"AND", "and"},
            {"NAND", "nand"},
            {"XOR", "xor"},
            {"NOR", "nor"},
            {"OR", "or"},
            {"NOT", "not"}
        }
    },
    {
        "Specials",
        {
            {"Input", "input"},
            {"Output", "output"},
            {"Clock", "clock"},
            {"True", "true"},
            {"False", "false"}
        }
    },
    {
        "Components",
        {
            {"C4001 | Four NOR Gates", "4001"},
            {"C4011 | Four NAND Gates", "4011"},
            {"C4030 | Four XOR Gates", "4030"},
            {"C4071 | Four OR Gates", "4071"},
            {"C4081 | Four AND Gates", "4081"},
            {"C4069 | Six INVERTER Gates", "4069"},
            {"C4008 | 4 bits adder", "4008"},
            {"C4013 | Dual Flip Flop", "4013"},
            {"C4017 | 10 bits Johnson decade", "4017"},
            {"C4040 | 12 bits counter", "4040"},
            {"C4094 | 8 bits shift register", "4094"},
            {"C4513 | 8 channel data selector", "4513"},
            {"C4514 | 4 bits decoder", "4514"},
            {"C4801 | Random access memory", "4801"},
            {"C2716 | Read only memory", "2716"}
        }
    },
    {
        "Sequencials",
        {
            {"FlipFlop", "flipflop"},
            {"Counter", "counter"}
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
void Circuit::draw(void) {
    if (!m_initialized) {
        initializePosition();
        m_initialized = true;
    }

    ImGuiWindowFlags blueprint_flags =
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("Blueprint", nullptr, blueprint_flags);

    Ez::BeginCanvas();
    for (const auto& [name, component] : m_components) {
        component->draw();
    }

    {
        IComponent* inNode;
        IComponent* outNode;
        const char* inSlot;
        const char* outSlot;

        if (ImNodes::GetNewConnection(
            (void**)&inNode, &inSlot, (void**)&outNode, &outSlot)
        ) {
            size_t in = (size_t)std::atol(inSlot);
            size_t out = (size_t)std::atol(outSlot);
            setLink(inNode->getName(), in, outNode->getName(), out);
        }
    }

    bool oneSelected = false;
    for (const auto& [name, component] : m_components) {
        std::string cls = component->getCleanClassName();
        if (cls != "Input" && cls != "Clock")
            continue;
        if (component->isSelected()) {
            oneSelected = true;
            break;
        }
    }

    Circuit::ComponentMap selected;
    for (const auto& [name, component] : m_components) {
        std::string cls = component->getCleanClassName();
        if (cls != "Input" && cls != "Clock")
            continue;
        if ((oneSelected && component->isSelected()) || !oneSelected) {
            selected[name] = component;
        }
    }

    if (
        ImGui::IsMouseReleased(1) &&
        ImGui::IsWindowHovered() &&
        !ImGui::IsMouseDragging(1)
    )
    {
        ImGui::FocusWindow(ImGui::GetCurrentWindow());
        ImGui::OpenPopup("NodesContextMenu");
    }

    if (ImGui::BeginPopup("NodesContextMenu")) {
        if (oneSelected && ImGui::MenuItem("Set True")) {
            for (auto& [name, cmp] : selected) {
                auto input = std::dynamic_pointer_cast<Specials::Input>(cmp);
                auto clock = std::dynamic_pointer_cast<Specials::Clock>(cmp);

                if (input) input->setValue(Tristate::True);
                if (clock) clock->setValue(Tristate::True);
            }
        }

        if (oneSelected && ImGui::MenuItem("Set False")) {
            for (auto& [name, cmp] : selected) {
                auto input = std::dynamic_pointer_cast<Specials::Input>(cmp);
                auto clock = std::dynamic_pointer_cast<Specials::Clock>(cmp);

                if (input) input->setValue(Tristate::False);
                if (clock) clock->setValue(Tristate::False);
            }
        }

        if (oneSelected && ImGui::MenuItem("Set Undefined")) {
            for (auto& [name, cmp] : selected) {
                auto input = std::dynamic_pointer_cast<Specials::Input>(cmp);
                auto clock = std::dynamic_pointer_cast<Specials::Clock>(cmp);

                if (input) input->setValue(Tristate::Undefined);
                if (clock) clock->setValue(Tristate::Undefined);
            }
        }

        if (!oneSelected) {
            for (const auto& [menu, cmps] : RegisteredComponents) {
                if (ImGui::BeginMenu(menu.c_str())) {
                    for (const auto& [name, idx] : cmps) {
                        if (ImGui::MenuItem(name.c_str())) {
                            m_pendingComponentType = idx;
                            m_pendingComponentName = idx + "_" +
                                std::to_string(m_components.size());
                            m_showComponentNamePopup = true;
                        }
                    }
                    ImGui::EndMenu();
                }
            }
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Reset Zoom")) {
            ImNodes::GetCurrentCanvas()->Zoom = 1;
        }

        ImGui::EndPopup();
    }

    if (m_showComponentNamePopup) {
        ImGui::OpenPopup("Component Name");
        ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize / 2 - ImVec2(150, 150));
        ImGui::SetNextWindowSize(ImVec2(300, 0));
    }

    if (ImGui::BeginPopupModal("Component Name", &m_showComponentNamePopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for the new %s component:", m_pendingComponentType.c_str());

        ImGui::InputText("##ComponentName", &m_pendingComponentName);

        ImGui::Separator();

        if (ImGui::Button("Create", ImVec2(120, 0))) {
            try {
                addComponent(m_pendingComponentType, m_pendingComponentName);

                auto component = getComponent(m_pendingComponentName);
                auto* aComponent = dynamic_cast<AComponent*>(component.get());
                if (aComponent) {
                    aComponent->m_position = m_lastMousePos;
                }
                
                ImNodes::AutoPositionNode(aComponent);
                m_showComponentNamePopup = false;
                ImGui::CloseCurrentPopup();
            } catch (const ComponentException&) {}
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showComponentNamePopup = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    Ez::EndCanvas();

    ImGui::Begin("Property Editor");

    if (ImGui::CollapsingHeader("Simulation")) {
        ImGui::Text("Components: %lu", m_components.size());

        ImGui::Separator();

        ImGui::Text("Current Tick: %lu", m_tick);

        if (ImGui::InputInt("Tick Destination", &m_gotoTick)) {
            if (m_gotoTick < (int)m_tick) {
                m_gotoTick = (int)m_tick + 1;
            }
        }

        if (ImGui::Button("Simulate")) {
            simulate();
            m_gotoTick = (int)m_tick + 1;
        }

        ImGui::SameLine();

        if (ImGui::Button("Goto Simulate")) {
            while (m_tick <= (size_t)m_gotoTick) {
                simulate();
            }
        }
    }

    ImGui::End();

    ImGui::End();
}

#endif

} // namespace nts
