///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "DigitalInput.hpp"
#include "Errors/OutOfRangePinException.hpp"
#include <limits>
#include <bitset>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
Input8::Input8(const std::string& name)
    : AComponent(name, 8)
    , m_value(0)
{
    for (size_t i = 0; i < 8; i++) {
        m_pins[i] = Pin(Pin::Type::OUTPUT);
    }
}

void Input8::innerDraw(void)
{
    unsigned int temp = static_cast<unsigned int>(m_value);

    ImGui::SetNextItemWidth(200.f * ImNodes::GetCurrentCanvas()->Zoom);
    if (ImGui::InputUint("", &temp)) {
        if (temp > UINT8_MAX)
            temp = UINT8_MAX;
        m_value = static_cast<uint8_t>(temp);
    }
    ImGui::Text("%s", std::bitset<8>(m_value).to_string().c_str());
}

///////////////////////////////////////////////////////////////////////////////
Tristate Input8::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    bool bitValue = (m_value & (1 << pin)) > 0;
    return (bitValue ? Tristate::True : Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
Input16::Input16(const std::string& name)
    : AComponent(name, 16)
    , m_value(0)
{
    for (size_t i = 0; i < 16; i++) {
        m_pins[i] = Pin(Pin::Type::OUTPUT);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Input16::innerDraw(void)
{
    unsigned int temp = static_cast<unsigned int>(m_value);

    ImGui::SetNextItemWidth(200.f * ImNodes::GetCurrentCanvas()->Zoom);
    if (ImGui::InputUint("", &temp)) {
        if (temp > UINT16_MAX)
            temp = UINT16_MAX;
        m_value = static_cast<uint16_t>(temp);
    }
    ImGui::Text("%s", std::bitset<16>(m_value).to_string().c_str());
}

///////////////////////////////////////////////////////////////////////////////
void Input16::setValue(uint16_t value)
{
    m_value = value;
}

///////////////////////////////////////////////////////////////////////////////
Tristate Input16::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    bool bitValue = (m_value & (1 << pin)) > 0;
    return (bitValue ? Tristate::True : Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
Input32::Input32(const std::string& name)
    : AComponent(name, 32)
    , m_value(0)
{
    for (size_t i = 0; i < 32; i++) {
        m_pins[i] = Pin(Pin::Type::OUTPUT);
    }
}

///////////////////////////////////////////////////////////////////////////////
void Input32::innerDraw(void)
{
    unsigned int temp = static_cast<unsigned int>(m_value);

    ImGui::SetNextItemWidth(200.f * ImNodes::GetCurrentCanvas()->Zoom);
    if (ImGui::InputUint("", &temp)) {
        if (temp > UINT32_MAX)
            temp = UINT32_MAX;
        m_value = static_cast<uint32_t>(temp);
    }
    ImGui::Text("%s", std::bitset<32>(m_value).to_string().c_str());
}

///////////////////////////////////////////////////////////////////////////////
Tristate Input32::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    bool bitValue = (m_value & (1 << pin)) > 0;
    return (bitValue ? Tristate::True : Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
InputColor::InputColor(const std::string& name)
    : AComponent(name, 32)
    , m_value(UINT32_MAX)
{
    for (size_t i = 0; i < 32; i++) {
        m_pins[i] = Pin(Pin::Type::OUTPUT);
    }
}

///////////////////////////////////////////////////////////////////////////////
void InputColor::innerDraw(void)
{
    float color[4] = {
        static_cast<float>((m_value >> 24) & 0xFF) / 255.0f, // Red
        static_cast<float>((m_value >> 16) & 0xFF) / 255.0f, // Green
        static_cast<float>((m_value >> 8) & 0xFF) / 255.0f,  // Blue
        static_cast<float>(m_value & 0xFF) / 255.0f          // Alpha
    };

    ImGui::SetNextItemWidth(200.f * ImNodes::GetCurrentCanvas()->Zoom);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    if (ImGui::ColorPicker4("Color", color)) {
        // Update m_value with the new color
        m_value = (static_cast<uint32_t>(color[0] * 255) << 24) |
                  (static_cast<uint32_t>(color[1] * 255) << 16) |
                  (static_cast<uint32_t>(color[2] * 255) << 8) |
                  (static_cast<uint32_t>(color[3] * 255));
    }
    ImGui::PopStyleVar();

    ImGui::Text("%s", std::bitset<32>(m_value).to_string().c_str());
}

///////////////////////////////////////////////////////////////////////////////
void InputColor::setValue(uint32_t value)
{
    m_value = value;
}

///////////////////////////////////////////////////////////////////////////////
Tristate InputColor::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    bool bitValue = (m_value & (1 << pin)) > 0;
    return (bitValue ? Tristate::True : Tristate::False);
}

} // namespace nts::Components
