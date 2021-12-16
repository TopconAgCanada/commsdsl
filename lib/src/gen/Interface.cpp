//
// Copyright 2021 - 2022 (C). Alex Robenko. All rights reserved.
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

#include "commsdsl/gen/Interface.h"
#include "commsdsl/gen/Field.h"
#include "commsdsl/gen/Generator.h"

#include <cassert>
#include <algorithm>

namespace commsdsl
{

namespace gen
{

class InterfaceImpl
{
public:
    using FieldsList = Interface::FieldsList;

    InterfaceImpl(Generator& generator, commsdsl::parse::Interface dslObj, Elem* parent) :
        m_generator(generator),
        m_dslObj(dslObj),
        m_parent(parent)
    {
        static_cast<void>(m_parent);
    }

    bool prepare()
    {
        if (!m_dslObj.valid()) {
            return true;
        }

        auto fields = m_dslObj.fields();
        m_fields.reserve(fields.size());
        for (auto& dslObj : fields) {
            auto ptr = Field::create(m_generator, dslObj, m_parent);
            assert(ptr);
            if (!ptr->prepare()) {
                return false;
            }

            m_fields.push_back(std::move(ptr));
        }

        return true;
    }

    bool write()
    {
        bool result = 
            std::all_of(
                m_fields.begin(), m_fields.end(),
                [](auto& fieldPtr) -> bool
                {
                    return fieldPtr->write();
                });

        return result;
    }

    const FieldsList& fields() const
    {
        return m_fields;
    }

private:
    Generator& m_generator;
    commsdsl::parse::Interface m_dslObj;
    Elem* m_parent = nullptr;
    FieldsList m_fields;
}; 

Interface::Interface(Generator& generator, commsdsl::parse::Interface dslObj, Elem* parent) :
    Base(parent),
    m_impl(std::make_unique<InterfaceImpl>(generator, dslObj, this))
{
}

Interface::~Interface() = default;

bool Interface::prepare()
{
    return m_impl->prepare();
}

bool Interface::write()
{
    if (!m_impl->write()) {
        return false;
    }

    return writeImpl();
}

const Interface::FieldsList& Interface::fields() const
{
    return m_impl->fields();
}

Elem::Type Interface::elemTypeImpl() const
{
    return Type_Interface;
}

bool Interface::writeImpl()
{
    return true;
}

} // namespace gen

} // namespace commsdsl
