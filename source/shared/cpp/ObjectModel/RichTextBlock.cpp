#include "pch.h"
#include <iomanip>
#include <regex>
#include <iostream>
#include <codecvt>
#include "ParseContext.h"
#include "RichTextBlock.h"
#include "DateTimePreparser.h"
#include "ParseUtil.h"

using namespace AdaptiveSharedNamespace;

InlineElement::InlineElement(InlineElementType type) : m_type(type)
{
}

Json::Value InlineElement::SerializeToJsonValue() const
{
    Json::Value root = Json::Value();

	root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Type)] = GetElementTypeString();

	return root;
}

Paragraph::Paragraph() : m_inlines()
{
}

Json::Value Paragraph::SerializeToJsonValue() const
{
    Json::Value root = Json::Value();

    std::string inlinesPropertyName = AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Inlines);
    root[inlinesPropertyName] = Json::Value(Json::arrayValue);
    for (const auto& inlineElement : GetInlines())
    {
        root[inlinesPropertyName].append(inlineElement->SerializeToJsonValue());
    }

    return root;
}

std::vector<std::shared_ptr<InlineElement>>& Paragraph::GetInlines()
{
    return m_inlines;
}

const std::vector<std::shared_ptr<InlineElement>>& Paragraph::GetInlines() const
{
    return m_inlines;
}

RichTextBlock::RichTextBlock() :
    BaseCardElement(CardElementType::RichTextBlock), m_wrap(false), m_maxLines(0),
    m_hAlignment(HorizontalAlignment::Left), m_language()
{
    PopulateKnownPropertiesSet();
}

Json::Value RichTextBlock::SerializeToJsonValue() const
{
    Json::Value root = BaseCardElement::SerializeToJsonValue();

    if (m_hAlignment != HorizontalAlignment::Left)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::HorizontalAlignment)] = HorizontalAlignmentToString(m_hAlignment);
    }

    if (m_maxLines != 0)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::MaxLines)] = m_maxLines;
    }

    if (m_wrap)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Wrap)] = true;
    }

    std::string paragraphsPropertyName = AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Paragraphs);
    root[paragraphsPropertyName] = Json::Value(Json::arrayValue);
    for (const auto& paragraph : GetParagraphs())
    {
        root[paragraphsPropertyName].append(paragraph->SerializeToJsonValue());
    }

    return root;
}

bool RichTextBlock::GetWrap() const
{
    return m_wrap;
}

void RichTextBlock::SetWrap(const bool value)
{
    m_wrap = value;
}

unsigned int RichTextBlock::GetMaxLines() const
{
    return m_maxLines;
}

void RichTextBlock::SetMaxLines(const unsigned int value)
{
    m_maxLines = value;
}

HorizontalAlignment RichTextBlock::GetHorizontalAlignment() const
{
    return m_hAlignment;
}

void RichTextBlock::SetHorizontalAlignment(const HorizontalAlignment value)
{
    m_hAlignment = value;
}

std::string RichTextBlock::GetLanguage() const
{
    return m_language;
}

void RichTextBlock::SetLanguage(const std::string& value)
{
    m_language = value;
}

std::vector<std::shared_ptr<Paragraph>>& RichTextBlock::GetParagraphs()
{
    return m_paragraphs;
}

const std::vector<std::shared_ptr<Paragraph>>& RichTextBlock::GetParagraphs() const
{
    return m_paragraphs;
}

std::shared_ptr<BaseCardElement> RichTextBlockParser::Deserialize(ParseContext& context, const Json::Value& json)
{
    ParseUtil::ExpectTypeString(json, CardElementType::RichTextBlock);

    std::shared_ptr<RichTextBlock> richTB = BaseCardElement::Deserialize<RichTextBlock>(context, json);
    richTB->SetWrap(ParseUtil::GetBool(json, AdaptiveCardSchemaKey::Wrap, false));
    richTB->SetMaxLines(ParseUtil::GetUInt(json, AdaptiveCardSchemaKey::MaxLines, 0));
    richTB->SetHorizontalAlignment(ParseUtil::GetEnumValue<HorizontalAlignment>(
        json, AdaptiveCardSchemaKey::HorizontalAlignment, HorizontalAlignment::Left, HorizontalAlignmentFromString));

    return richTB;
}

std::shared_ptr<BaseCardElement> RichTextBlockParser::DeserializeFromString(ParseContext& context, const std::string& jsonString)
{
    return RichTextBlockParser::Deserialize(context, ParseUtil::GetJsonValueFromString(jsonString));
}

void RichTextBlock::PopulateKnownPropertiesSet()
{
    m_knownProperties.insert({AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Wrap),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::MaxLines),
                              AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::HorizontalAlignment)});
}
