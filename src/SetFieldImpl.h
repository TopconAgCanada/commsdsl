#pragma once

#include <cstdint>
#include <utility>
#include <vector>

#include "bbmp/Endian.h"
#include "bbmp/SetField.h"
#include "FieldImpl.h"

namespace bbmp
{

class SetFieldImpl : public FieldImpl
{
    using Base = FieldImpl;
public:
    using Type = SetField::Type;

    using Bits = SetField::Bits;
    using RevBits = SetField::RevBits;

    SetFieldImpl(::xmlNodePtr node, ProtocolImpl& protocol);
    SetFieldImpl(const SetFieldImpl&);

    Type type() const
    {
        return m_type;
    }

    Endian endian() const
    {
        return m_endian;
    }

    std::size_t length() const
    {
        return m_length;
    }

    std::size_t bitLength() const
    {
        return m_bitLength;
    }

    std::uintmax_t defaultValue() const
    {
        return m_defaultValue;
    }

    std::uintmax_t reservedValue() const
    {
        return m_reservedValue;
    }

    std::uintmax_t reservedBits() const
    {
        return m_reservedValue | m_implicitReserved;
    }

    const Bits& bits() const
    {
        return m_bits;
    }

    const RevBits& revBits() const
    {
        return m_revBits;
    }

    bool isNonUniqueAllowed() const
    {
        return m_nonUniqueAllowed;
    }

    bool isUnique() const;

protected:
    virtual Kind kindImpl() const override;
    virtual Ptr cloneImpl() const override;
    virtual const XmlWrap::NamesList& extraPropsNamesImpl() const override;
    virtual const XmlWrap::NamesList& extraChildrenNamesImpl() const override;
    virtual bool parseImpl() override;
    virtual std::size_t lengthImpl() const override;
    virtual std::size_t bitLengthImpl() const override;

private:
    bool updateEndian();
    bool updateLength();
    bool updateNonUniqueAllowed();
    bool updateDefaultValue();
    bool updateReservedValue();
    bool updateBits();

    Type m_type = Type::NumOfValues;
    Endian m_endian = Endian_NumOfValues;
    std::size_t m_length = 0U;
    std::size_t m_bitLength = 0U;
    std::uintmax_t m_defaultValue = 0U;
    std::uintmax_t m_reservedValue = 0U;
    std::uintmax_t m_reservedBits = 0U;
    std::uintmax_t m_implicitReserved = 0U;
    Bits m_bits;
    RevBits m_revBits;
    bool m_nonUniqueAllowed = false;
    bool m_defaultBitValue = false;
    bool m_reservedBitValue = false;
};

} // namespace bbmp
