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
#include "ComponentFactory.hpp"
#include "components/InputComponent.hpp"
#include "components/OutputComponent.hpp"
#include "components/ClockComponent.hpp"
#include "components/TrueComponent.hpp"
#include "components/FalseComponent.hpp"
#include "components/AndGateComponent.hpp"
#include "components/OrGateComponent.hpp"
#include "components/XorGateComponent.hpp"
#include "components/NotGateComponent.hpp"
#include "components/C4081Component.hpp"
#include <unordered_map>
#include <functional>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createComponent(
    const std::string& type,
    const std::string& name
)
{
    static const std::unordered_map<
        std::string,
        std::function<ComponentFactory::Component(const std::string&)>
    > creators = {
        {"input", &ComponentFactory::createInput},
        {"output", &ComponentFactory::createOutput},
        {"clock", &ComponentFactory::createClock},
        {"true", &ComponentFactory::createTrue},
        {"false", &ComponentFactory::createFalse},
        {"4081", &ComponentFactory::create4081}
        // ...
    };

    auto it = creators.find(type);
    if (it ==  creators.end())
        throw std::runtime_error("Unknown component type: " + type);
    return (it->second(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createInput(
    const std::string& name
)
{
    return (std::make_unique<InputComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createOutput(
    const std::string& name
)
{
    return (std::make_unique<OutputComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createClock(
    const std::string& name
)
{
    return (std::make_unique<ClockComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createTrue(
    const std::string& name
)
{
    return (std::make_unique<TrueComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createFalse(
    const std::string& name
)
{
    return (std::make_unique<FalseComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createAnd(
    const std::string& name
)
{
    return (std::make_unique<AndGateComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createOr(
    const std::string& name
)
{
    return (std::make_unique<OrGateComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createXor(
    const std::string& name
)
{
    return (std::make_unique<XorGateComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::createNot(
    const std::string& name
)
{
    return (std::make_unique<NotGateComponent>(name));
}

///////////////////////////////////////////////////////////////////////////////
ComponentFactory::Component ComponentFactory::create4081(
    const std::string& name
)
{
    return (std::make_unique<C4081Component>(name));
}

} // namespace nts
