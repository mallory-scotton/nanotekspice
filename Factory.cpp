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

#include "Components/C4069.hpp"
#include "Components/QuadGate.hpp"

#include "Sequencials/FlipFlop.hpp"

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

    // registerComponent<IComponent>           ("2716");
    registerComponent<Components::C4001>        ("4001");
    // registerComponent<IComponent>           ("4008");
    registerComponent<Components::C4011>        ("4011");
    // registerComponent<IComponent>           ("4013");
    // registerComponent<IComponent>           ("4017");
    registerComponent<Components::C4030>        ("4030");
    // registerComponent<IComponent>           ("4040");
    registerComponent<Components::C4069>        ("4069");
    registerComponent<Components::C4071>        ("4071");
    registerComponent<Components::C4081>        ("4081");
    // registerComponent<IComponent>           ("4094");
    // registerComponent<IComponent>           ("4512");
    // registerComponent<IComponent>           ("4514");
    // registerComponent<IComponent>           ("4801");
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
