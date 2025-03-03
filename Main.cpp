///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Shell/Shell.hpp"
#include "Errors/ParsingException.hpp"
#include <iostream>

#ifndef NTS_BONUS

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <circuit_file>" << std::endl;
        return (84);
    }

    try {
        nts::Shell shell;
        shell.loadCircuit(argv[1]);
        shell.run();
    } catch (const nts::ParsingException& e) {
        std::cerr << e.what() << std::endl;
        return (84);
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return (84);
    }

    return (0);
}

#else

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Bonus/Bonus.hpp"

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <circuit_file>" << std::endl;
        return (84);
    }

    try {
        nts::Bonus(argv[1]).run();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return (84);
    }
}

#endif
