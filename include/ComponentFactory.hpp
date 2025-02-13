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
#include "IComponent.hpp"
#include <memory>

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
/// \brief
///
///////////////////////////////////////////////////////////////////////////////
class ComponentFactory
{
public:
    ///////////////////////////////////////////////////////////////////////////
    // Type alias
    ///////////////////////////////////////////////////////////////////////////
    using Component = std::unique_ptr<IComponent>;

public:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    ///
    /// \param type
    /// \param name
    ///
    /// \return
    ///
    ///////////////////////////////////////////////////////////////////////////
    static Component createComponent(
        const std::string& type,
        const std::string& name
    );

private:
    static Component createInput(const std::string& name);
    static Component createOutput(const std::string& name);
    static Component createClock(const std::string& name);
    static Component createTrue(const std::string& name);
    static Component createFalse(const std::string& name);

private:
    static Component createAnd(const std::string& name);
    static Component createOr(const std::string& name);
    static Component createXor(const std::string& name);
    static Component createNot(const std::string& name);

private:
    static Component create4081(const std::string& name);
    static Component create4001(const std::string& name);
    static Component create4011(const std::string& name);
    static Component create4030(const std::string& name);
    static Component create4069(const std::string& name);
    static Component create4071(const std::string& name);
    static Component create4008(const std::string& name);
    static Component create4013(const std::string& name);
    static Component create4017(const std::string& name);
    static Component create4040(const std::string& name);
    static Component create4094(const std::string& name);
    static Component create4512(const std::string& name);
    static Component create4514(const std::string& name);
    static Component create4801(const std::string& name);
    static Component create2716(const std::string& name);
    // ...
};

} // namespace nts
