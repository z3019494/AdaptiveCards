#pragma once

#include "pch.h"
#include "RichTextBlock.h"
#include "TextElement.h"
#include "ElementParserRegistration.h"
#include "DateTimePreparser.h"

namespace AdaptiveSharedNamespace
{
    class TextRun : public InlineElement, public TextElement
    {
    public:
        TextRun();
        TextRun(const TextRun&) = default;
        TextRun(TextRun&&) = default;
        TextRun& operator=(const TextRun&) = default;
        TextRun& operator=(TextRun&&) = default;
        ~TextRun() = default;

    private:
        void PopulateKnownPropertiesSet() override;
    };

    class TextRunParser : public BaseCardElementParser
    {
    public:
        TextRunParser() = default;
        TextRunParser(const TextRunParser&) = default;
        TextRunParser(TextRunParser&&) = default;
        TextRunParser& operator=(const TextRunParser&) = default;
        TextRunParser& operator=(TextRunParser&&) = default;
        virtual ~TextRunParser() = default;

        std::shared_ptr<BaseCardElement> Deserialize(ParseContext& context, const Json::Value& root) override;
        std::shared_ptr<BaseCardElement> DeserializeFromString(ParseContext& context, const std::string& jsonString) override;
    };
}
