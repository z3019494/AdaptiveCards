#pragma once

#include "pch.h"
#include "BaseCardElement.h"
#include "TextElement.h"
#include "ElementParserRegistration.h"
#include "DateTimePreparser.h"

namespace AdaptiveSharedNamespace
{
    class TextBlock : public BaseCardElement, public TextElement
    {
    public:
        TextBlock();
        TextBlock(const TextBlock&) = default;
        TextBlock(TextBlock&&) = default;
        TextBlock& operator=(const TextBlock&) = default;
        TextBlock& operator=(TextBlock&&) = default;
        ~TextBlock() = default;

        Json::Value SerializeToJsonValue() const override;

        bool GetWrap() const;
        void SetWrap(const bool value);

        unsigned int GetMaxLines() const;
        void SetMaxLines(const unsigned int value);

        HorizontalAlignment GetHorizontalAlignment() const;
        void SetHorizontalAlignment(const HorizontalAlignment value);

    private:
        bool m_wrap;
        unsigned int m_maxLines;
        HorizontalAlignment m_hAlignment;
        void PopulateKnownPropertiesSet() override;
    };

    class TextBlockParser : public BaseCardElementParser
    {
    public:
        TextBlockParser() = default;
        TextBlockParser(const TextBlockParser&) = default;
        TextBlockParser(TextBlockParser&&) = default;
        TextBlockParser& operator=(const TextBlockParser&) = default;
        TextBlockParser& operator=(TextBlockParser&&) = default;
        virtual ~TextBlockParser() = default;

        std::shared_ptr<BaseCardElement> Deserialize(ParseContext& context, const Json::Value& root) override;
        std::shared_ptr<BaseCardElement> DeserializeFromString(ParseContext& context, const std::string& jsonString) override;
    };
}
