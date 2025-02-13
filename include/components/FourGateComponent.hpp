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
    {
        m_gates[0].setlink(1, this, 1);
        m_gates[0].setlink(2, this, 2);
        m_gates[0].setlink(3, this, 3);

        m_gates[1].setlink(1, this, 6);
        m_gates[1].setlink(2, this, 5);
        m_gates[1].setlink(3, this, 4);

        m_gates[2].setlink(1, this, 8);
        m_gates[2].setlink(2, this, 9);
        m_gates[2].setlink(3, this, 10);

        m_gates[3].setlink(1, this, 13);
        m_gates[3].setlink(2, this, 12);
        m_gates[3].setlink(3, this, 11);
    }

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
        size_t min = 0;
        if (pin < 1 || pin > 14)
            throw std::out_of_range("Invalid pin for x4Gate component");
        if (pin < 4)
            return m_gates[0]->compute(pin)
        if (pin < 7) {
            min = pin == 4 ? 1 : pin;
            min = pin == 6 ? 5 : pin;
            return m_gates[1]->compute(pin - min)
        }
        if (pin == 7)
            return (Tristate::Undefinded);
        if (pin < 11)
            return m_gates[2]->compute(pin - 7)
        if (pin < 14) {
            min = pin == 11 ? 8 : pin;
            min = pin == 13 ? 12 : pin;
            return m_gates[3]->compute(pin - min)
        }
        return (Tristate::Undefined);
    }
};

///////////////////////////////////////////////////////////////////////////////
using C4001Component = FourGateComponent<NorGateComponent>;
using C4011Component = FourGateComponent<NandGateComponent>;
using C4030Component = FourGateComponent<XorGateComponent>;
using C4071Component = FourGateComponent<OrGateComponent>;
using C4081Component = FourGateComponent<AndGateComponent>;

// link pin 1 to pin 1 of mgate[0]
// link pin 2 to pin 2 of mgate[0]
// link pin 3 to pin 3 of mgate[0]

// link pin 4 to pin 3 of mgate[0]
// link pin 5 to pin 2 of mgate[0]
// link pin 6 to pin 1 of mgate[0]

// link pin 8 to pin 1 of mgate[0]
// link pin 9 to pin 2 of mgate[0]
// link pin 10 to pin 3 of mgate[0]

// link pin 11 to pin 3 of mgate[0]
// link pin 12 to pin 2 of mgate[0]
// link pin 13 to pin 1 of mgate[0]

} // namespace nts
