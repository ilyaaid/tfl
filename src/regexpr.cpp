#include "regexpr.hpp"

Conc::Conc(const RegExprShPtr &l, const RegExprShPtr &r) : l_(l), r_(r)
{
}

Star::Star(const RegExprShPtr &opd): opd_(opd)
{
}

Or::Or(const RegExprShPtr &l, const RegExprShPtr &r) : l_(l), r_(r)
{
}

ConcLit::ConcLit(const string &concstr)
{
    concstr_ = concstr;
}
