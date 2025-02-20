///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Shell/Shell.hpp"
#include "Errors/ComponentException.hpp"
#include <iostream>

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
    } catch (const nts::ComponentException& e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected exception: " << e.what() << std::endl;
        return (84);
    }

    return (0);
}
