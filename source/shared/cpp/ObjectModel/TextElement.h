#pragma once

#include "pch.h"
#include "DateTimePreparser.h"

namespace AdaptiveSharedNamespace
{
    class TextElement
    {
    public:
        TextElement();
        TextElement(const TextElement&) = default;
        TextElement(TextElement&&) = default;
        TextElement& operator=(const TextElement&) = default;
        TextElement& operator=(TextElement&&) = default;
        ~TextElement() = default;

        std::string GetText() const;
        void SetText(const std::string& value);
        DateTimePreparser GetTextForDateParsing() const;

        TextSize GetTextSize() const;
        void SetTextSize(const TextSize value);

        TextWeight GetTextWeight() const;
        void SetTextWeight(const TextWeight value);

        FontStyle GetFontStyle() const;
        void SetFontStyle(const FontStyle value);

        ForegroundColor GetTextColor() const;
        void SetTextColor(const ForegroundColor value);

		bool GetIsSubtle() const;
        void SetIsSubtle(const bool value);

        std::string GetLanguage() const;
        void SetLanguage(const std::string& value);

    protected:
        std::string m_text;
        TextSize m_textSize;
        TextWeight m_textWeight;
        FontStyle m_fontStyle;
        ForegroundColor m_textColor;
        bool m_isSubtle;
        std::string m_language;
    };
}
