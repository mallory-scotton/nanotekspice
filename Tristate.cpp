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
std::ostream& operator<<(std::ostream& os, const Tristate& state)
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
Tristate operator~(const Tristate& state)
{
    if (state == Tristate::Undefined)
        return (Tristate::Undefined);
    if (state == Tristate::True)
        return (Tristate::False);
    return (Tristate::True);
}

///////////////////////////////////////////////////////////////////////////////
Tristate operator&(const Tristate& lhs, const Tristate& rhs)
{
    if (lhs == Tristate::Undefined || rhs == Tristate::Undefined)
        return (Tristate::Undefined);
    if (lhs == Tristate::True && rhs == Tristate::True)
        return (Tristate::True);
    return (Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
Tristate operator|(const Tristate& lhs, const Tristate& rhs)
{
    if (lhs == Tristate::Undefined || rhs == Tristate::Undefined)
        return (Tristate::Undefined);
    if (lhs == Tristate::True || rhs == Tristate::True)
        return (Tristate::True);
    return (Tristate::False);
}

///////////////////////////////////////////////////////////////////////////////
Tristate operator^(const Tristate& lhs, const Tristate& rhs)
{
    if (lhs == Tristate::Undefined || rhs == Tristate::Undefined)
        return (Tristate::Undefined);
    if (lhs == rhs)
        return (Tristate::False);
    return (Tristate::True);
}

///////////////////////////////////////////////////////////////////////////////
Tristate& operator&=(Tristate& lhs, const Tristate& rhs)
{
    lhs = lhs & rhs;
    return (lhs);
}

///////////////////////////////////////////////////////////////////////////////
Tristate& operator|=(Tristate& lhs, const Tristate& rhs)
{
    lhs = lhs | rhs;
    return (lhs);
}

///////////////////////////////////////////////////////////////////////////////
Tristate& operator^=(Tristate& lhs, const Tristate& rhs)
{
    lhs = lhs ^ rhs;
    return (lhs);
}

} // namespace nts
