///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Shell.hpp"
#include "Parser.hpp"
#include "Pin.hpp"
#include <signal.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void Shell::handleSigint(int)
{
    exit(0);
}

///////////////////////////////////////////////////////////////////////////////
Shell::Shell(void)
    : m_running(true)
{
    signal(SIGINT, Shell::handleSigint);
}

///////////////////////////////////////////////////////////////////////////////
void Shell::loadCircuit(const std::string& filename)
{
    Parser::parseCircuit(m_circuit, filename);
}

///////////////////////////////////////////////////////////////////////////////
void Shell::displayDebugInfo(void)
{
    std::cout << "===== Circuit Debug Information =====" << std::endl;
    std::cout << "Components: "
              << m_circuit.getComponents().size() << std::endl;

    for (const auto& [name, component] : m_circuit.getComponents()) {
        std::cout << "\n[Component] " << name << std::endl;

        if (!component) {
            std::cout << "  WARNING: Null component" << std::endl;
            continue;
        }

        std::vector<Pin>& pins = component->getPins();
        std::cout << "  Pin count: " << pins.size() << std::endl;

        for (size_t i = 0; i < pins.size(); i++) {
            Pin& pin = pins[i];
            std::string typeStr;

            switch (pin.getType()) {
                case Pin::Type::INPUT:     typeStr = "INPUT"; break;
                case Pin::Type::OUTPUT:    typeStr = "OUTPUT"; break;
                case Pin::Type::ELECTRICAL: typeStr = "ELECTRICAL"; break;
                default:                   typeStr = "UNKNOWN"; break;
            }

            std::string stateStr;
            switch (pin.getState()) {
                case Tristate::True:       stateStr = "TRUE (1)"; break;
                case Tristate::False:      stateStr = "FALSE (0)"; break;
                case Tristate::Undefined:  stateStr = "UNDEFINED (U)"; break;
                default:                   stateStr = "INVALID"; break;
            }

            std::cout << "  Pin " << i
                      << " [" << typeStr << "] State: "
                      << stateStr << std::endl;

            const auto& links = pin.getLinks();
            if (links.empty()) {
                std::cout << "    No links" << std::endl;
            } else {
                std::cout << "    Links (" << links.size()
                          << "):" << std::endl;
                for (const auto& link : links) {
                    if (auto linkedComponent = link.component.lock()) {
                        std::cout << "      -> " << linkedComponent->getName()
                                  << " (pin " << link.pin << ")" << std::endl;
                    } else {
                        std::cout << "      -> EXPIRED LINK" << std::endl;
                    }
                }
            }
        }
    }

    std::cout << "\n===== End Debug Information =====" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
void Shell::processCommand(const std::string& command)
{
    if (command.empty()) return;

    if (command == "exit")      { m_running = false; return; }
    if (command == "display")   { m_circuit.display(); return; }
    if (command == "simulate")  { m_circuit.simulate(); return; }
    if (command == "loop")      { runLoop(); return; }
    if (command == "debug")     { displayDebugInfo(); return; }

    size_t equalPos = command.find('=');
    if (equalPos != std::string::npos) {
        std::string inputName = command.substr(0, equalPos);
        std::string valueStr = command.substr(equalPos + 1);

        Tristate value;
        if (valueStr == "0")
            value = Tristate::False;
        else if (valueStr == "1")
            value = Tristate::True;
        else if (valueStr == "U")
            value = Tristate::Undefined;
        else
            throw std::runtime_error("Invalid input value. Use 0, 1, or U.");

        m_circuit.setValue(inputName, value);
        return;
    }

    throw std::runtime_error("Unknown command: " + command);
}

///////////////////////////////////////////////////////////////////////////////
void Shell::runLoop(void)
{
    while (true) {
        m_circuit.simulate();
        m_circuit.display();
    }
}

///////////////////////////////////////////////////////////////////////////////
void Shell::run(void)
{
    std::string command;

    std::cout << "> ";
    while (m_running && std::getline(std::cin, command)) {
        try {
            processCommand(command);
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }

        if (m_running)
            std::cout << "> ";
    }
}

} // namespace nts
