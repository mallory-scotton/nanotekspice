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
#include "Tristate.hpp"

///////////////////////////////////////////////////////////////////////////////
// Namespace nts
///////////////////////////////////////////////////////////////////////////////
namespace nts
{

///////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& os, const Tristate& state)
{
    switch (state) {
        case Tristate::Undefined:
            os << 'U';
            break;
        case Tristate::True:
            os << '1';
            break;
        case Tristate::False:
            os << '0';
            break;
    }
    return (os);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate operator~(const Tristate& state)
{
    if (state == Tristate::Undefined)
        return (Tristate::Undefined);
    if (state == Tristate::True)
        return (Tristate::False);
    return (Tristate::True);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate operator&(const Tristate& lhs, const Tristate& rhs)
{
    if (lhs == Tristate::Undefined || rhs == Tristate::Undefined)
        return (Tristate::Undefined);
    if (lhs == Tristate::True && rhs == Tristate::True)
        return (Tristate::True);
    return (Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate operator|(const Tristate& lhs, const Tristate& rhs)
{
    if (lhs == Tristate::Undefined || rhs == Tristate::Undefined)
        return (Tristate::Undefined);
    if (lhs == Tristate::True || rhs == Tristate::True)
        return (Tristate::True);
    return (Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate operator^(const Tristate& lhs, const Tristate& rhs)
{
    if (lhs == Tristate::Undefined || rhs == Tristate::Undefined)
        return (Tristate::Undefined);
    if (lhs == rhs)
        return (Tristate::False);
    return (Tristate::True);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate& operator&=(Tristate& lhs, const Tristate& rhs)
{
    lhs = lhs & rhs;
    return (lhs);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate& operator|=(Tristate& lhs, const Tristate& rhs)
{
    lhs = lhs | rhs;
    return (lhs);
}

///////////////////////////////////////////////////////////////////////////////
inline Tristate& operator^=(Tristate& lhs, const Tristate& rhs)
{
    lhs = lhs ^ rhs;
    return (lhs);
}

} // namespace nts
