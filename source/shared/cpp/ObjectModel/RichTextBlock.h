#pragma once

#include "pch.h"
#include "BaseCardElement.h"
#include "ElementParserRegistration.h"
#include "DateTimePreparser.h"

namespace AdaptiveSharedNamespace
{
    class InlineElement
    {
    public:
        InlineElement(InlineElementType type);
        InlineElement() = default;
        InlineElement(const InlineElement&) = default;
        InlineElement(InlineElement&&) = default;
        InlineElement& operator=(const InlineElement&) = default;
        InlineElement& operator=(InlineElement&&) = default;
        ~InlineElement() = default;

        virtual Json::Value SerializeToJsonValue() const;

        virtual const InlineElementType GetElementType() const;

        template<typename T> static std::shared_ptr<T> Deserialize(ParseContext& context, const Json::Value& json);

		// Element type
        std::string GetElementTypeString() const;
        void SetElementTypeString(const std::string& value);

		template<typename T>
        static void ParseJsonObject(AdaptiveSharedNamespace::ParseContext& context,
                                    const Json::Value& json,
                                    std::shared_ptr<InlineElement>& element);

    protected:
        void SetTypeString(const std::string& type) { m_typeString = type; }
        std::string m_typeString;

    private:
        InlineElementType m_type;
    };

    class Paragraph
    {
    public:
        Paragraph();
        Paragraph() = default;
        Paragraph(const Paragraph&) = default;
        Paragraph(Paragraph&&) = default;
        Paragraph& operator=(const Paragraph&) = default;
        Paragraph& operator=(Paragraph&&) = default;
        ~Paragraph() = default;

        Json::Value SerializeToJsonValue() const;

        std::vector<std::shared_ptr<InlineElement>>& GetInlines();
        const std::vector<std::shared_ptr<InlineElement>>& GetInlines() const;

    private:
        std::vector<std::shared_ptr<InlineElement>> m_inlines;
    };

    class RichTextBlock : public BaseCardElement
    {
    public:
        RichTextBlock();
        RichTextBlock(const RichTextBlock&) = default;
        RichTextBlock(RichTextBlock&&) = default;
        RichTextBlock& operator=(const RichTextBlock&) = default;
        RichTextBlock& operator=(RichTextBlock&&) = default;
        ~RichTextBlock() = default;

        Json::Value SerializeToJsonValue() const override;

        bool GetWrap() const;
        void SetWrap(const bool value);

        unsigned int GetMaxLines() const;
        void SetMaxLines(const unsigned int value);

        HorizontalAlignment GetHorizontalAlignment() const;
        void SetHorizontalAlignment(const HorizontalAlignment value);

        std::string GetLanguage() const;
        void SetLanguage(const std::string& value);

        std::vector<std::shared_ptr<Paragraph>>& GetParagraphs();
        const std::vector<std::shared_ptr<Paragraph>>& GetParagraphs() const;

    private:
        bool m_wrap;
        unsigned int m_maxLines;
        HorizontalAlignment m_hAlignment;
        void PopulateKnownPropertiesSet() override;
        std::string m_language;

        std::vector<std::shared_ptr<Paragraph>> m_paragraphs;
    };

    class RichTextBlockParser : public BaseCardElementParser
    {
    public:
        RichTextBlockParser() = default;
        RichTextBlockParser(const RichTextBlockParser&) = default;
        RichTextBlockParser(RichTextBlockParser&&) = default;
        RichTextBlockParser& operator=(const RichTextBlockParser&) = default;
        RichTextBlockParser& operator=(RichTextBlockParser&&) = default;
        virtual ~RichTextBlockParser() = default;

        std::shared_ptr<BaseCardElement> Deserialize(ParseContext& context, const Json::Value& root) override;
        std::shared_ptr<BaseCardElement> DeserializeFromString(ParseContext& context, const std::string& jsonString) override;
    };
}
