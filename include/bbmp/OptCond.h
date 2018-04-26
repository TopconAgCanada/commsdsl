#pragma once

#include <string>
#include <vector>

#include "BbmpApi.h"

namespace bbmp
{

class OptCondImpl;
class BBMP_API OptCond
{
public:

    enum class Kind
    {
        Expr,
        List,
        NumOfValues
    };

    explicit OptCond(const OptCondImpl* impl);
    OptCond(const OptCond& other);
    ~OptCond();

    bool valid() const;

    Kind kind() const;

protected:
    const OptCondImpl* m_pImpl;
};

class OptCondExprImpl;
class OptCondExpr : public OptCond
{
    using Base = OptCond;
public:
    OptCondExpr(const OptCondExprImpl* impl);
    OptCondExpr(OptCond cond);

    const std::string& left() const;
    const std::string& op() const;
    const std::string& right() const;
};

class OptCondListImpl;
class OptCondList : public OptCond
{
    using Base = OptCond;
public:
    enum class Type
    {
        And,
        Or,
        NumOfValues
    };

    using CondList = std::vector<OptCond>;

    OptCondList(const OptCondListImpl* impl);
    OptCondList(OptCond cond);


    Type type() const;
    CondList conditions() const;
};

} // namespace bbmp
