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

#include "ToolsQtGenerator.h"

#include "ToolsQtBitfieldField.h"
#include "ToolsQtBundleField.h"
#include "ToolsQtChecksumLayer.h"
#include "ToolsQtCmake.h"
#include "ToolsQtCustomLayer.h"
#include "ToolsQtDataField.h"
#include "ToolsQtEnumField.h"
#include "ToolsQtFloatField.h"
#include "ToolsQtFrame.h"
#include "ToolsQtIdLayer.h"
#include "ToolsQtInputMessages.h"
#include "ToolsQtIntField.h"
#include "ToolsQtInterface.h"
#include "ToolsQtListField.h"
#include "ToolsQtMessage.h"
#include "ToolsQtOptionalField.h"
#include "ToolsQtPayloadLayer.h"
#include "ToolsQtPlugin.h"
#include "ToolsQtRefField.h"
#include "ToolsQtSetField.h"
#include "ToolsQtSizeLayer.h"
#include "ToolsQtStringField.h"
#include "ToolsQtSyncLayer.h"
#include "ToolsQtValueLayer.h"
#include "ToolsQtVariantField.h"

#include "commsdsl/version.h"

#include <algorithm>
#include <cassert>

namespace commsdsl2tools_qt
{

const std::string& ToolsQtGenerator::fileGeneratedComment()
{
    static const std::string Str =
        "// Generated by commsdsl2tools_qt v" + std::to_string(commsdsl::versionMajor()) +
        '.' + std::to_string(commsdsl::versionMinor()) + '.' +
        std::to_string(commsdsl::versionPatch()) + '\n';
    return Str;
}

bool ToolsQtGenerator::prepareImpl() 
{
    bool result = 
        Base::prepareImpl() &&
        toolsPrepareDefaultInterfaceInternal();

    if (!result) {
        return false;
    }

    if (m_pluginInfos.empty()) {
        m_pluginInfos.resize(1U);
        auto& pInfo = m_pluginInfos.back();

        auto allInterfaces = getAllInterfaces();
        assert(!allInterfaces.empty());
        auto allFrames = getAllFrames();
        assert(!allFrames.empty());
        auto* interfacePtr = allInterfaces.front();
        assert(interfacePtr != nullptr);

        pInfo.m_frame = allFrames.front()->dslObj().externalRef();
        
        if (interfacePtr->dslObj().valid()) {
            pInfo.m_interface = interfacePtr->dslObj().externalRef();    
        }
        else {
            pInfo.m_interface = interfacePtr->name();
        }
        pInfo.m_name = schemaName();
        pInfo.m_desc = "Protocol " + schemaName();
    }

    for (auto& info : m_pluginInfos) {
        m_plugins.push_back(std::make_unique<ToolsQtPlugin>(*this, info.m_frame, info.m_interface, info.m_name, info.m_desc));
    }

    return 
        std::all_of(
            m_plugins.begin(), m_plugins.end(),
            [](auto& pPtr)
            {
                return pPtr->prepare();
            });
}

ToolsQtGenerator::InterfacePtr ToolsQtGenerator::createInterfaceImpl(commsdsl::parse::Interface dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtInterface>(*this, dslObj, parent);
}

ToolsQtGenerator::MessagePtr ToolsQtGenerator::createMessageImpl(commsdsl::parse::Message dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtMessage>(*this, dslObj, parent);
}

ToolsQtGenerator::FramePtr ToolsQtGenerator::createFrameImpl(commsdsl::parse::Frame dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtFrame>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createIntFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtIntField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createEnumFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtEnumField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createSetFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtSetField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createFloatFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtFloatField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createBitfieldFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtBitfieldField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createBundleFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtBundleField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createStringFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtStringField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createDataFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtDataField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createListFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtListField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createRefFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtRefField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createOptionalFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtOptionalField>(*this, dslObj, parent);
}

ToolsQtGenerator::FieldPtr ToolsQtGenerator::createVariantFieldImpl(commsdsl::parse::Field dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtVariantField>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createCustomLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtCustomLayer>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createSyncLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtSyncLayer>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createSizeLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtSizeLayer>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createIdLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtIdLayer>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createValueLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtValueLayer>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createPayloadLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtPayloadLayer>(*this, dslObj, parent);
}

ToolsQtGenerator::LayerPtr ToolsQtGenerator::createChecksumLayerImpl(commsdsl::parse::Layer dslObj, Elem* parent)
{
    return std::make_unique<commsdsl2tools_qt::ToolsQtChecksumLayer>(*this, dslObj, parent);
}

bool ToolsQtGenerator::writeImpl()
{
    bool result =  
        ToolsQtCmake::write(*this) &&
        ToolsQtInputMessages::write(*this);

    if (!result) {
        return false;
    }

    return 
        std::all_of(
            m_plugins.begin(), m_plugins.end(),
            [](auto& pluginPtr)
            {
                return pluginPtr->write();
            });
}

bool ToolsQtGenerator::toolsPrepareDefaultInterfaceInternal()
{
    auto allInterfaces = getAllInterfaces();
    if (!allInterfaces.empty()) {
        return true;
    }

    auto* defaultNamespace = addDefaultNamespace();
    auto* interface = defaultNamespace->addDefaultInterface();
    if (interface == nullptr) {
        logger().error("Failed to create default interface");
        return false;
    }

    return true;
}

} // namespace commsdsl2tools_qt
