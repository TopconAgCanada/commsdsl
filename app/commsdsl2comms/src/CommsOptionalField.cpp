//
// Copyright 2019 - 2022 (C). Alex Robenko. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "CommsOptionalField.h"

#include "CommsGenerator.h"

#include "commsdsl/gen/comms.h"
#include "commsdsl/gen/util.h"
#include "commsdsl/gen/strings.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace util = commsdsl::gen::util;
namespace comms = commsdsl::gen::comms;
namespace strings = commsdsl::gen::strings;

namespace commsdsl2comms
{

CommsOptionalField::CommsOptionalField(
    CommsGenerator& generator, 
    commsdsl::parse::Field dslObj, 
    commsdsl::gen::Elem* parent) :
    Base(generator, dslObj, parent),
    CommsBase(static_cast<Base&>(*this))
{
}

bool CommsOptionalField::prepareImpl()
{
    bool result = Base::prepareImpl() && commsPrepare();
    if (result) {
        m_commsExternalField = dynamic_cast<CommsField*>(externalField());
        m_commsMemberField = dynamic_cast<CommsField*>(memberField());
    }

    if (m_commsExternalField != nullptr) {
        m_commsExternalField->commsSetReferenced();
    }
    return result;
}

bool CommsOptionalField::writeImpl() const
{
    return commsWrite();
}

CommsOptionalField::IncludesList CommsOptionalField::commsCommonIncludesImpl() const 
{
    IncludesList result;
    if (m_commsMemberField != nullptr) {
        result = m_commsMemberField->commsCommonIncludes();
    }

    return result;
}

std::string CommsOptionalField::commsCommonCodeBodyImpl() const
{
    return commsCommonNameFuncCode();
}

std::string CommsOptionalField::commsCommonMembersCodeImpl() const
{
    if (m_commsMemberField != nullptr) {
        return m_commsMemberField->commsCommonCode();
    }

    return strings::emptyString();
}

CommsOptionalField::IncludesList CommsOptionalField::commsDefIncludesImpl() const
{
    IncludesList result = {
        "comms/field/Optional.h"
    };

    if (m_commsExternalField != nullptr) {
        result.push_back(comms::relHeaderPathFor(m_commsExternalField->field(), generator()));
        return result;
    }

    assert(m_commsMemberField != nullptr);
    auto incList = m_commsMemberField->commsDefIncludes();
    result.reserve(result.size() + incList.size());
    std::move(incList.begin(), incList.end(), std::back_inserter(result));
    return result;
}

std::string CommsOptionalField::commsDefMembersCodeImpl() const
{
    if (m_commsMemberField != nullptr) {
        return m_commsMemberField->commsDefCode();
    }

    return strings::emptyString();
}

std::string CommsOptionalField::commsDefBaseClassImpl() const
{
    static const std::string Templ = 
        "comms::field::Optional<\n"
        "    #^#FIELD_REF#$##^#COMMA#$#\n"
        "    #^#FIELD_OPTS#$#\n"
        ">";

    util::ReplacementMap repl = {
        {"FIELD_REF", commsDefFieldRefInternal()},
        {"FIELD_OPTS", commsDefFieldOptsInternal()}
    };

    if (!repl["FIELD_OPTS"].empty()) {
        repl["COMMA"] = ",";
    }

    return util::processTemplate(Templ, repl);
}

std::string CommsOptionalField::commsDefBundledReadPrepareFuncBodyImpl(const CommsFieldsList& siblings) const
{
    static_cast<void>(siblings);
    auto c = optionalDslObj().cond();
    if (!c.valid()) {
        return strings::emptyString();
    }

    return "refresh_" + comms::accessName(dslObj().name()) + "();\n";
}

std::string CommsOptionalField::commsDefBundledRefreshFuncBodyImpl(const CommsFieldsList& siblings) const
{
    auto c = optionalDslObj().cond();
    if (!c.valid()) {
        return strings::emptyString();
    }

    static const std::string Templ =
        "auto mode = comms::field::OptionalMode::Missing;\n"
        "if (#^#COND#$#) {\n"
        "    mode = comms::field::OptionalMode::Exists;\n"
        "}\n\n"
        "if (field_#^#NAME#$#()#^#FIELD_ACC#$#.getMode() == mode) {\n"
        "    return false;\n"
        "}\n\n"
        "field_#^#NAME#$#()#^#FIELD_ACC#$#.setMode(mode);\n"
        "return true;\n";

    util::ReplacementMap repl {
        {"NAME", comms::accessName(name())},
        {"COND", commsDslCondToStringInternal(siblings, c)}
    };

    if (commsIsVersionOptional()) {
        repl["FIELD_ACC"] = ".field()";
    }

    return util::processTemplate(Templ, repl);    
}

bool CommsOptionalField::commsIsVersionDependentImpl() const
{
    return (m_commsMemberField != nullptr) && (m_commsMemberField->commsIsVersionDependent());
}

std::string CommsOptionalField::commsMembersCustomizationOptionsBodyImpl(FieldOptsFunc fieldOptsFunc) const
{
    if (m_commsMemberField == nullptr) {
        return strings::emptyString();
    }

    assert(fieldOptsFunc != nullptr);
    return (m_commsMemberField->*fieldOptsFunc)();
}

std::size_t CommsOptionalField::commsMaxLengthImpl() const
{
    if (m_commsExternalField != nullptr) {
        return m_commsExternalField->commsMaxLength();
    }

    assert(m_commsMemberField != nullptr);
    return m_commsMemberField->commsMaxLength();
}

std::string CommsOptionalField::commsDefFieldRefInternal() const
{
    if (m_commsExternalField != nullptr) {
            std::string templOpt;
        if (!comms::isInterfaceDeepMemberField(*this)) {
            templOpt = "TOpt";
        }

        return comms::scopeFor(m_commsExternalField->field(), generator()) + '<' + templOpt + '>';
    }

    assert(m_commsMemberField != nullptr);
    auto ref = "typename " + comms::className(name()) + strings::membersSuffixStr();
    if (comms::isGlobalField(*this)) {
        ref += "<TOpt>";
    }

    ref += "::" + comms::className(m_commsMemberField->field().name());    
    return ref;
}

std::string CommsOptionalField::commsDefFieldOptsInternal() const
{
    util::StringsList opts;

    commsAddFieldDefOptions(opts);
    commsAddModeOptInternal(opts);

    return util::strListToString(opts, ",\n", "");
}

void CommsOptionalField::commsAddModeOptInternal(StringsList& opts) const
{
    static const std::string Map[] = {
        strings::emptyString(),
        "comms::option::def::ExistsByDefault",
        "comms::option::def::MissingByDefault"
    };

    static const std::size_t MapSize =
            std::extent<decltype(Map)>::value;

    static_assert(MapSize == static_cast<std::size_t>(commsdsl::parse::OptionalField::Mode::NumOfValues), "Invalid map");

    auto obj = optionalDslObj();
    auto mode = obj.defaultMode();
    auto idx = static_cast<std::size_t>(mode);
    if (MapSize <= idx) {
        static constexpr bool Should_not_happen = false;
        static_cast<void>(Should_not_happen);
        assert(Should_not_happen);
        idx = 0U;
    }

    if (Map[idx].empty()) {
        return;
    }

    opts.push_back(Map[idx]);
}

std::string CommsOptionalField::commsDslCondToStringInternal(
    const CommsFieldsList& siblings, 
    const commsdsl::parse::OptCond& cond,
    bool bracketsWrap)
{
    if (cond.kind() == commsdsl::parse::OptCond::Kind::Expr) {
        auto findFieldFunc =
            [&siblings](const std::string& name) -> const CommsField*
            {
                auto iter =
                    std::find_if(
                        siblings.begin(), siblings.end(),
                        [&name](auto& f)
                        {
                            return f->field().dslObj().name() == name;
                        });

                if (iter == siblings.end()) {
                    return nullptr;
                }

                return *iter;
            };

        auto opFunc =
            [](const std::string& val) -> const std::string& {
                if (val == "=") {
                    static const std::string Str = "==";
                    return Str;
                }
                return val;
            };

        commsdsl::parse::OptCondExpr exprCond(cond);
        auto& left = exprCond.left();
        auto& op = opFunc(exprCond.op());
        auto& right = exprCond.right();

        if (!left.empty()) {
            assert(!op.empty());
            assert(!right.empty());
            assert(left[0] == '$');

            std::string leftFieldName(left, 1);
            auto* leftField = findFieldFunc(leftFieldName);
            if (leftField == nullptr) {
                static constexpr bool Should_not_happen = false;
                static_cast<void>(Should_not_happen);
                assert(Should_not_happen);
                return strings::emptyString();
            }

            if (right[0] != '$') {
                return leftField->commsCompareToValueCode(op, right);
            }

            auto* rightField = findFieldFunc(std::string(right, 1));
            if (rightField == nullptr) {
                static constexpr bool Should_not_happen = false;
                static_cast<void>(Should_not_happen);
                assert(Should_not_happen);
                return strings::emptyString();
            }

            return leftField->commsCompareToFieldCode(op, *rightField);
        }

        // Reference to bit in "set".
        if (right[0] != '$') {
            static constexpr bool Should_not_happen = false;
            static_cast<void>(Should_not_happen);
            assert(Should_not_happen);
            return strings::emptyString();
        }

        std::string fieldRef(right, 1U);
        auto dotPos = fieldRef.find(".");
        std::string fieldExternalRef(fieldRef, 0, dotPos);
        auto* rightField = findFieldFunc(fieldExternalRef);

        if (rightField == nullptr) {
            static constexpr bool Should_not_happen = false;
            static_cast<void>(Should_not_happen);
            assert(Should_not_happen);
            return strings::emptyString();
        }

        assert(
            (rightField->field().dslObj().kind() == commsdsl::parse::Field::Kind::Set) ||
            (rightField->field().dslObj().kind() == commsdsl::parse::Field::Kind::Ref));
        std::string valueStr;
        if (dotPos != std::string::npos) {
            valueStr.assign(fieldRef.begin() + dotPos + 1, fieldRef.end());
        }

        return rightField->commsCompareToValueCode(op, valueStr);
    }

    if ((cond.kind() != commsdsl::parse::OptCond::Kind::List)) {
        static constexpr bool Should_not_happen = false;
        static_cast<void>(Should_not_happen);
        assert(Should_not_happen);
        return strings::emptyString();
    }

    commsdsl::parse::OptCondList listCond(cond);
    auto type = listCond.type();

    static const std::string AndOp = " &&\n";
    static const std::string OrOp = " ||\n";

    auto* op = &AndOp;
    if (type == commsdsl::parse::OptCondList::Type::Or) {
        op = &OrOp;
    }
    else {
        assert(type == commsdsl::parse::OptCondList::Type::And);
    }

    auto conditions = listCond.conditions();
    std::string condTempl;
    util::ReplacementMap repl;
    if (bracketsWrap) {
        condTempl += '(';
    }

    for (auto count = 0U; count < conditions.size(); ++count) {
        if (0U < count) {
            condTempl += ' ';
        }

        auto condStr = "COND" + std::to_string(count);
        repl[condStr] = commsDslCondToStringInternal(siblings, conditions[count], true);
        condTempl += "(#^#";
        condTempl += condStr;
        condTempl += "#$#)";
        if (count < (conditions.size() - 1U)) {
            condTempl += *op;
        }
    }

    if (bracketsWrap) {
        condTempl += ')';
    }

    return util::processTemplate(condTempl, repl);
}

} // namespace commsdsl2comms
