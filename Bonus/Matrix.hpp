///////////////////////////////////////////////////////////////////////////////
// Header guard
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "AComponent.hpp"
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <fstream>
#include <string>
#include <cstdint>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class Matrix : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private properties
    ///////////////////////////////////////////////////////////////////////////
    uint16_t m_size;                //<!
    std::vector<uint8_t> m_pixels;  //<!
    size_t m_selectedRow;           //<!
    size_t m_selectedColumn;        //<!
    bool m_hasChanged;              //<!
    std::string m_outputFile;       //<!
    sf::Image m_image;              //<!
    sf::Texture m_texture;          //<!
    sf::Sprite m_sprite;            //<!

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    /// \param outputFile
    ///
    ///////////////////////////////////////////////////////////////////////////
    Matrix(
        const std::string& name,
        const std::string& outputFile = "display.ppm"
    );

private:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void updateSelectedPixel(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void updatePixelValue(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief Get pixel index from row and column
    ///
    /// \param row Row index
    /// \param col Column index
    /// \return Index in the pixel array (for first component)
    ///
    ///////////////////////////////////////////////////////////////////////////
    size_t getPixelIndex(size_t row, size_t col) const;

protected:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    virtual void innerDraw(void) override;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    void simulate(size_t tick) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param pin
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    Tristate compute(size_t pin) override;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    ///////////////////////////////////////////////////////////////////////////
    void clear(void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    std::string getDisplayState(void) const;
};

} // namespace nts::Components
