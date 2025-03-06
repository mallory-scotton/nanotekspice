///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "Factory.hpp"
#include "Errors/UnknownComponentException.hpp"

#include "Specials/Input.hpp"
#include "Specials/Output.hpp"
#include "Specials/Clock.hpp"
#include "Specials/False.hpp"
#include "Specials/True.hpp"

#include "Gates/And.hpp"
#include "Gates/Nand.hpp"
#include "Gates/Nor.hpp"
#include "Gates/Not.hpp"
#include "Gates/Or.hpp"
#include "Gates/Xor.hpp"

#include "Components/C2716.hpp"
#include "Components/C4008.hpp"
#include "Components/C4069.hpp"
#include "Components/C4013.hpp"
#include "Components/C4017.hpp"
#include "Components/C4040.hpp"
#include "Components/C4094.hpp"
#include "Components/C4512.hpp"
#include "Components/QuadGate.hpp"

#include "Sequencials/FlipFlop.hpp"
#include "Sequencials/Counter.hpp"

#ifdef NTS_BONUS
    #include "Bonus/Matrix.hpp"
    #include "Bonus/DigitalInput.hpp"
#endif

///////////////////////////////////////////////////////////////////////////////
// Namesapce nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
Factory::Factory(void)
{
    registerComponent<Specials::Input>          ("input");
    registerComponent<Specials::Output>         ("output");
    registerComponent<Specials::Clock>          ("clock");
    registerComponent<Specials::True>           ("true");
    registerComponent<Specials::False>          ("false");

    registerComponent<Gates::And>               ("and");
    registerComponent<Gates::Nand>              ("nand");
    registerComponent<Gates::Or>                ("or");
    registerComponent<Gates::Nor>               ("nor");
    registerComponent<Gates::Xor>               ("xor");
    registerComponent<Gates::Not>               ("not");

    registerComponent<Sequencials::FlipFlop>    ("flipflop");
    registerComponent<Sequencials::Counter>     ("counter");

    registerComponent<Components::C2716>        ("2716");
    registerComponent<Components::C4001>        ("4001");
    registerComponent<Components::C4008>        ("4008");
    registerComponent<Components::C4011>        ("4011");
    registerComponent<Components::C4013>        ("4013");
    registerComponent<Components::C4017>        ("4017");
    registerComponent<Components::C4030>        ("4030");
    registerComponent<Components::C4040>        ("4040");
    registerComponent<Components::C4069>        ("4069");
    registerComponent<Components::C4071>        ("4071");
    registerComponent<Components::C4081>        ("4081");
    registerComponent<Components::C4094>                    ("4094"); //
    registerComponent<Components::C4512>                    ("4512"); //
    registerComponent<Components::C4001>                    ("4514");
    registerComponent<Components::C4001>                    ("4801");

#ifdef NTS_BONUS
    registerComponent<Components::Matrix>       ("matrix");
    registerComponent<Bonus::Input8>            ("input8");
    registerComponent<Bonus::Input16>           ("input16");
    registerComponent<Bonus::Input32>           ("input32");
    registerComponent<Bonus::InputColor>        ("color");
#endif
}

///////////////////////////////////////////////////////////////////////////////
std::shared_ptr<IComponent> Factory::createComponent(
    const std::string& type,
    const std::string& name
)
{
    auto it = m_factories.find(type);
    if (it == m_factories.end())
        throw UnknownComponentException(type);
    return (it->second(name));
}

} // namespace nts
