///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "logger.hpp"
#include "Errors/OutOfRangePinException.hpp"
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts::Components
///////////////////////////////////////////////////////////////////////////////
namespace nts::Components
{

///////////////////////////////////////////////////////////////////////////////
logger::logger(const std::string& name)
    : AComponent(name, 16)
{
    m_pins[0] = Pin(Pin::Type::INPUT, "B1");    // in1
    m_pins[1] = Pin(Pin::Type::INPUT, "B2");    // in2
    m_pins[2] = Pin(Pin::Type::INPUT, "B4");    // in4
    m_pins[3] = Pin(Pin::Type::INPUT, "B8");    // in8
    m_pins[4] = Pin(Pin::Type::INPUT, "B16");   // in16
    m_pins[5] = Pin(Pin::Type::INPUT, "B32");   // in32
    m_pins[6] = Pin(Pin::Type::INPUT, "B64");   // in64
    m_pins[7] = Pin(Pin::Type::INPUT, "B128");  // in128
    m_pins[8] = Pin(Pin::Type::INPUT, "CLK");   // clock
    m_pins[9] = Pin(Pin::Type::INPUT, "INH");   // inhibit
}

///////////////////////////////////////////////////////////////////////////////
Tristate logger::compute(size_t pin)
{
    if (pin >= 10) {
        throw OutOfRangePinException();
    }
    return getInputState(pin);
}

///////////////////////////////////////////////////////////////////////////////
void logger::simulate(size_t tick)
{
    if (m_tick == tick)
        return;
    AComponent::simulate(tick);
    Tristate inhibit = getInputState(9);
    Tristate clock = getInputState(8);
    char byte = 0;
    for (size_t i = 0; i < 8; i++) {
        Tristate input = getInputState(i);
        if (input == Tristate::Undefined) {
            m_lastClock = clock;
            return;
        }
        if (input == Tristate::True) {
            byte |= (1 << i);
        }
    }
    if (inhibit == False && m_lastClock != True && clock == True) {
        std::ofstream logFile("./log.bin", std::ios::app | std::ios::binary);
        if (logFile.is_open()) {
            logFile.write(&byte, 1);
            logFile.flush();
            logFile.close();
        }
    }
    m_lastClock = clock;
}

} // namespace nts::Components
