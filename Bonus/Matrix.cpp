///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Matrix.hpp"
#include "Errors/OutOfRangePinException.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <bitset>
#include <cmath>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
Matrix::Matrix(
    const std::string& name,
    const std::string& outputFile
)
    : AComponent(name, 44)
    , m_selectedRow(0)
    , m_selectedColumn(0)
    , m_hasChanged(false)
    , m_outputFile(outputFile)
    , m_image({64, 64}, sf::Color::White)
    , m_texture(m_image)
    , m_sprite(m_texture)
{
    m_pixels.resize(64 * 64 * 4, 255);

    for (size_t i = 0; i < 32; i++) {
        m_pins[i] = Pin(Pin::Type::INPUT);
    }

    for (size_t i = 32; i < 44; i++) {
        m_pins[i] = Pin(Pin::Type::INPUT);
    }
}

///////////////////////////////////////////////////////////////////////////////
size_t Matrix::getPixelIndex(size_t row, size_t col) const
{
    return ((col + row * 64) * 4);
}

///////////////////////////////////////////////////////////////////////////////
void Matrix::updateSelectedPixel(void)
{
    uint16_t address = 0;

    for (size_t i = 0; i < 12; i++) {
        if (getInputState(i + 32) == Tristate::True) {
            address |= (1 << i);
        }
    }

    m_selectedRow = address / 64;
    m_selectedColumn = address % 64;

    m_selectedRow = std::min(m_selectedRow, static_cast<size_t>(63));
    m_selectedColumn = std::min(m_selectedColumn, static_cast<size_t>(63));
}

///////////////////////////////////////////////////////////////////////////////
void Matrix::updatePixelValue(void)
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint8_t alpha = 0;

    for (size_t i = 0; i < 8; i++) {
        if (getInputState(i) == Tristate::True) {
            red |= (1 << i);
        }
    }
    for (size_t i = 8; i < 16; i++) {
        if (getInputState(i) == Tristate::True) {
            green |= (1 << (i - 8));
        }
    }
    for (size_t i = 16; i < 24; i++) {
        if (getInputState(i) == Tristate::True) {
            blue |= (1 << (i - 16));
        }
    }
    for (size_t i = 24; i < 32; i++) {
        if (getInputState(i) == Tristate::True) {
            alpha |= (1 << (i - 24));
        }
    }

    size_t pixelIdx = getPixelIndex(m_selectedRow, m_selectedColumn);

    if (pixelIdx + 3 >= m_pixels.size()) {
        return;
    }

    if (
        m_pixels[pixelIdx]     != red ||
        m_pixels[pixelIdx + 1] != green ||
        m_pixels[pixelIdx + 2] != blue ||
        m_pixels[pixelIdx + 3] != alpha
    ) {
        m_hasChanged = true;
    }

    m_pixels[pixelIdx]     = red;       // R
    m_pixels[pixelIdx + 1] = green;     // G
    m_pixels[pixelIdx + 2] = blue;      // B
    m_pixels[pixelIdx + 3] = alpha;     // A
}

///////////////////////////////////////////////////////////////////////////////
void Matrix::innerDraw(void)
{
    if (m_hasChanged) {
        for (uint32_t row = 0; row < 64; row++) {
            for (uint32_t col = 0; col < 64; col++) {
                size_t idx = getPixelIndex((size_t)row, (size_t)col);
                m_image.setPixel({row, col}, sf::Color(
                    m_pixels[idx],
                    m_pixels[idx + 1],
                    m_pixels[idx + 2],
                    m_pixels[idx + 3])
                );
            }
        }

        (void)m_texture.loadFromImage(m_image);
        m_sprite.setTexture(m_texture);
    }

    float zoom = ImNodes::GetCurrentCanvas()->Zoom;
    m_sprite.setScale({5.f * zoom, 5.f * zoom});
    ImGui::Image(m_sprite);
    ImGui::Dummy(ImVec2(20, 20));
}

///////////////////////////////////////////////////////////////////////////////
void Matrix::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);

    updateSelectedPixel();
    updatePixelValue();
}

///////////////////////////////////////////////////////////////////////////////
Tristate Matrix::compute(size_t pin)
{
    if (pin >= m_pins.size())
        throw OutOfRangePinException();
    return (Tristate::Undefined);
}

///////////////////////////////////////////////////////////////////////////////
void Matrix::clear(void)
{
    std::fill(m_pixels.begin(), m_pixels.end(), 255);
    m_hasChanged = true;
}

///////////////////////////////////////////////////////////////////////////////
std::string Matrix::getDisplayState(void) const
{
    std::stringstream ss;

    ss << "Matrix Display State:\n";
    ss << "--------------------\n";

    for (size_t row = 0; row < 64; row++) {
        for (size_t col = 0; col < 64; col++) {
            size_t idx = getPixelIndex(row, col);

            if (m_pixels[idx] < 32) {
                ss << '#';
            } else if (m_pixels[idx] < 128) {
                ss << 'o';
            } else if (m_pixels[idx] < 255) {
                ss << '.';
            } else {
                ss << ' ';
            }
        }
        ss << "\n";
    }

    return ss.str();
}

} // namespace nts::Components
