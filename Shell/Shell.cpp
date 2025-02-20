///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Shell.hpp"
#include "Parser.hpp"
#include <signal.h>

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
void Shell::processCommand(const std::string& command)
{
    if (command.empty()) return;

    if (command == "exit")      { m_running = false; return; }
    if (command == "display")   { m_circuit.display(); return; }
    if (command == "simulate")  { m_circuit.simulate(); return; }
    if (command == "loop")      { runLoop(); return; }

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
