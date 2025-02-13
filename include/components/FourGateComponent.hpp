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
// Pragma
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
// Dependencies
///////////////////////////////////////////////////////////////////////////////
#include "AComponent.hpp"
#include "GateComponent.hpp"
#include "AndGateComponent.hpp"
#include "NandGateComponent.hpp"
#include "NorGateComponent.hpp"
#include "XorGateComponent.hpp"
#include "OrGateComponent.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
template <typename T>
class FourGateComponent : public AComponent
{
private:
    ///////////////////////////////////////////////////////////////////////////
    // Private properties
    ///////////////////////////////////////////////////////////////////////////
    std::unique_ptr<T> m_gates[4];

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param name
    ///
    ///////////////////////////////////////////////////////////////////////////
    explicit FourGateComponent(const std::string& name)
        : AComponent(name, 14)
    {}

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param tick
    ///
    ///////////////////////////////////////////////////////////////////////////
    void simulate(size_t tick) override
    {
        (void)tick;
    }

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param pin
    ///
    ///////////////////////////////////////////////////////////////////////////
    Tristate compute(size_t pin) override
    {
        (void)pin;
        return (Tristate::Undefined);
    }
};

///////////////////////////////////////////////////////////////////////////////
using C4001Component = FourGateComponent<NorGateComponent>;
using C4011Component = FourGateComponent<NandGateComponent>;
using C4030Component = FourGateComponent<XorGateComponent>;
using C4071Component = FourGateComponent<OrGateComponent>;
using C4081Component = FourGateComponent<AndGateComponent>;

} // namespace nts
