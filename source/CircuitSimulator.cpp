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
#include "CircuitSimulator.hpp"
#include <csignal>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
void CircuitSimulator::handleSigint(int)
{
    exit(0);
}

///////////////////////////////////////////////////////////////////////////////
CircuitSimulator::CircuitSimulator(void)
    : m_running(true)
{
    signal(SIGINT, CircuitSimulator::handleSigint);
}

///////////////////////////////////////////////////////////////////////////////
void CircuitSimulator::loadCircuit(const std::string& filename)
{
    CircuitParser::parseCircuit(m_circuit, filename);
}

///////////////////////////////////////////////////////////////////////////////
void CircuitSimulator::processCommand(const std::string& command)
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
void CircuitSimulator::runLoop(void)
{
    while (true) {
        m_circuit.simulate();
        m_circuit.display();
    }
}

///////////////////////////////////////////////////////////////////////////////
void CircuitSimulator::run(void)
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
