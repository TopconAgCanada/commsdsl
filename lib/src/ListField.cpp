#include "commsdsl/ListField.h"

#include <cassert>

#include "ListFieldImpl.h"

namespace commsdsl
{

namespace
{

const ListFieldImpl* cast(const FieldImpl* ptr)
{
    assert(ptr != nullptr);
    return static_cast<const ListFieldImpl*>(ptr);
}

} // namespace

ListField::ListField(const ListFieldImpl* impl)
  : Base(impl)
{
}

ListField::ListField(Field field)
  : Base(field)
{
    assert(kind() == Kind::List);
}

Field ListField::elementField() const
{
    return cast(m_pImpl)->elementField();
}

std::size_t ListField::fixedCount() const
{
    return cast(m_pImpl)->count();
}

bool ListField::hasCountPrefixField() const
{
    return cast(m_pImpl)->hasCountPrefixField();
}

Field ListField::countPrefixField() const
{
    return cast(m_pImpl)->countPrefixField();
}

const std::string& ListField::detachedCountPrefixFieldName() const
{
    return cast(m_pImpl)->detachedCountPrefixFieldName();
}

bool ListField::hasLengthPrefixField() const
{
    return cast(m_pImpl)->hasLengthPrefixField();
}

Field ListField::lengthPrefixField() const
{
    return cast(m_pImpl)->lengthPrefixField();
}

const std::string& ListField::detachedLengthPrefixFieldName() const
{
    return cast(m_pImpl)->detachedLengthPrefixFieldName();
}

bool ListField::hasElemLengthPrefixField() const
{
    return cast(m_pImpl)->hasElemLengthPrefixField();
}

Field ListField::elemLengthPrefixField() const
{
    return cast(m_pImpl)->elemLengthPrefixField();
}

const std::string& ListField::detachedElemLengthPrefixFieldName() const
{
    return cast(m_pImpl)->detachedElemLengthPrefixFieldName();
}

bool ListField::elemFixedLength() const
{
    return cast(m_pImpl)->elemFixedLength();
}

} // namespace commsdsl
