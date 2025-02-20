///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Parser.hpp"
#include "Errors/ParsingException.hpp"
#include <string>
#include <sstream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void Parser::parseChipset(Circuit& circuit, const std::string& line)
{
    std::istringstream iss(line);
    std::string type, name;

    if (!(iss >> type >> name))
        throw ParsingException("Invalid chipset definition: " + line);
    circuit.addComponent(type, name);
}

///////////////////////////////////////////////////////////////////////////////
void Parser::parseLink(Circuit& circuit, const std::string& line)
{
    std::istringstream iss(line);
    std::string link1, link2;

    if (!(iss >> link1 >> link2))
        throw ParsingException("Invalid link definition: " + line);

    size_t colon1 = link1.find(':');
    if (colon1 == std::string::npos)
        throw ParsingException("Invalid component link format: " + link1);

    std::string component1 = link1.substr(0, colon1);
    size_t pin1;
    try {
        pin1 = std::stoul(link1.substr(colon1 + 1));
    } catch (const std::exception&) {
        throw ParsingException("Invalid pin number for component: " + link1);
    }

    size_t colon2 = link2.find(':');
    if (colon2 == std::string::npos)
        throw ParsingException("Invalid component link format: " + link2);

    std::string component2 = link2.substr(0, colon2);
    size_t pin2;
    try {
        pin2 = std::stoul(link2.substr(colon2 + 1));
    } catch (const std::exception&) {
        throw ParsingException("Invalid pin number for component: " + link2);
    }

    circuit.setLink(component1, pin1 - 1, component2, pin2 - 1);
}

///////////////////////////////////////////////////////////////////////////////
void Parser::parseCircuit(Circuit& circuit, const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw ParsingException("Cannot open circuit file: " + filename);

    std::string line;

    bool inChipsets = false;
    bool inLinks = false;

    while (std::getline(file, line)) {
        size_t comment = line.find('#');

        if (comment != std::string::npos)
            line = line.substr(0, comment);

        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty())
            continue;

        if (line == ".chipsets:") {
            inChipsets = true;
            inLinks = false;
            continue;
        } else if (line == ".links:") {
            inChipsets = false;
            inLinks = true;
            continue;
        }

        if (inChipsets)   parseChipset(circuit, line);
        else if (inLinks) parseLink(circuit, line);
    }

    if (circuit.getComponents().empty())
        throw ParsingException("No chipsets found in the circuit");
}

} // namespace nts
