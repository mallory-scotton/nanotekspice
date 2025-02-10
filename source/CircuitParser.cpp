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
#include "CircuitParser.hpp"
#include <sstream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void CircuitParser::parseChipset(Circuit& circuit, const std::string& line)
{
    std::istringstream iss(line);
    std::string type, name;

    if (!(iss >> type >> name))
        throw std::runtime_error("Invalid chipset definition: " + line);
    circuit.addComponent(type, name);
}

///////////////////////////////////////////////////////////////////////////////
void CircuitParser::parseLink(Circuit& circuit, const std::string& line)
{
    std::istringstream iss(line);
    std::string link1, link2;

    if (!(iss >> link1 >> link2))
        throw std::runtime_error("Invalid link definition: " + line);

    size_t colon1 = link1.find(':');
    if (colon1 == std::string::npos)
        throw std::runtime_error(
            "Invalid first component link format: " + link1
        );

    std::string component1 = link1.substr(0, colon1);
    std::size_t pin1;
    try {
        pin1 = std::stoul(link1.substr(colon1 + 1));
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Invalid pin number for first component: " + link1
        );
    }

    size_t colon2 = link2.find(':');
    if (colon2 == std::string::npos)
        throw std::runtime_error(
            "Invalid second component link format: " + link2
        );

    std::string component2 = link2.substr(0, colon2);
    std::size_t pin2;
    try {
        pin2 = std::stoul(link2.substr(colon2 + 1));
    } catch (const std::exception&) {
        throw std::runtime_error(
            "Invalid pin number for second component: " + link2
        );
    }

    circuit.setLink(component1, pin1, component2, pin2);
}

///////////////////////////////////////////////////////////////////////////////
void CircuitParser::parseCircuit(Circuit& circuit, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open circuit file: " + filename);

    std::string line;
    bool inChipsets = false;
    bool inLinks = false;

    while (std::getline(file, line)) {
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);

        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty()) continue;

        if (line == ".chipsets:") {
            inChipsets = true;
            inLinks = false;
            continue;
        } else if (line == ".links:") {
            inChipsets = false;
            inLinks = true;
            continue;
        }

        if (inChipsets)     parseChipset(circuit, line);
        else if (inLinks)   parseLink(circuit, line);
    }

    if (circuit.getComponents().empty())
        throw std::runtime_error("No chipsets found in the circuit");
}

} // namespace nts
