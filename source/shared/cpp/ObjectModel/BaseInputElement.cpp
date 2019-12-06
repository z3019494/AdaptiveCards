// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "BaseInputElement.h"
#include "ParseUtil.h"

using namespace AdaptiveSharedNamespace;

BaseInputElement::BaseInputElement(CardElementType elementType) : BaseCardElement(elementType), m_isRequired(false)
{
    PopulateKnownPropertiesSet();
}

BaseInputElement::BaseInputElement(CardElementType elementType, Spacing spacing, bool separator, HeightType height) :
    BaseCardElement(elementType, spacing, separator, height), m_isRequired(false)
{
    PopulateKnownPropertiesSet();
}

std::shared_ptr<BaseCardElement> BaseInputElement::GetLabel() const
{
    return m_label;
}

void BaseInputElement::SetLabel(const std::shared_ptr<BaseCardElement> label)
{
    m_label = label;
}

bool BaseInputElement::GetIsRequired() const
{
    return m_isRequired;
}

void BaseInputElement::SetIsRequired(const bool value)
{
    m_isRequired = value;
}

std::string BaseInputElement::GetErrorMessage() const
{
    return m_errorMessage;
}

void BaseInputElement::SetErrorMessage(const std::string errorMessage)
{
    m_errorMessage = errorMessage;
}

Json::Value BaseInputElement::SerializeToJsonValue() const
{
    Json::Value root = BaseCardElement::SerializeToJsonValue();

    if (m_isRequired)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IsRequired)] = m_isRequired;
    }

    if (!m_errorMessage.empty())
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::ErrorMessage)] = m_errorMessage;
    }

    if (m_label != nullptr)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Label)] = m_label->SerializeToJsonValue();
    }

    return root;
}

std::shared_ptr<BaseCardElement> BaseInputElement::DeserializeInputLabel(ParseContext& context, const Json::Value& json)
{
    std::shared_ptr<BaseCardElement> labelElement;

    auto propertyValue = json.get(AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Label), Json::Value());
    if (!propertyValue.empty())
    {
        if (propertyValue.isString())
        {
            std::shared_ptr<TextBlock> stringAsTextBlock = std::make_shared<TextBlock>();
            stringAsTextBlock->SetText(propertyValue.asString());
            labelElement = stringAsTextBlock;
        }
        else if (propertyValue.isObject())
        {
            auto labelType =
                propertyValue.get(AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Type), Json::Value()).asString();

            if (labelType.compare("TextBlock") == 0 || labelType.compare("RichTextBlock") == 0)
            {
                labelElement = context.elementParserRegistration->GetParser(labelType)->Deserialize(context, propertyValue);
            }
        }

        if (labelElement == nullptr)
        {
            throw AdaptiveCardParseException(ErrorStatusCode::InvalidPropertyValue,
                                             "Input Labels must be a string, a TextBlock element, or a RichTextBlockElement");
        }
    }

    return labelElement;
}

void BaseInputElement::PopulateKnownPropertiesSet()
{
    m_knownProperties.insert({AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IsRequired),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::ErrorMessage),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Label)});
}
