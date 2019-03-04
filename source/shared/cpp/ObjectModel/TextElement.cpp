#include "pch.h"
#include <iomanip>
#include <regex>
#include <iostream>
#include <codecvt>
#include "ParseContext.h"
#include "TextElement.h"
#include "DateTimePreparser.h"
#include "ParseUtil.h"

using namespace AdaptiveSharedNamespace;

TextElement::TextElement() :
    m_text(), m_textSize(TextSize::Default), m_textWeight(TextWeight::Default), m_fontStyle(FontStyle::Default),
    m_textColor(ForegroundColor::Default), m_isSubtle(false), m_language()
{
}

std::string TextElement::GetText() const
{
    return m_text;
}

void TextElement::SetText(const std::string& value)
{
    m_text = value;
}

DateTimePreparser TextElement::GetTextForDateParsing() const
{
    return DateTimePreparser(m_text);
}

TextSize TextElement::GetTextSize() const
{
    return m_textSize;
}

void TextElement::SetTextSize(const TextSize value)
{
    m_textSize = value;
}

TextWeight TextElement::GetTextWeight() const
{
    return m_textWeight;
}

void TextElement::SetTextWeight(const TextWeight value)
{
    m_textWeight = value;
}

FontStyle TextElement::GetFontStyle() const
{
    return m_fontStyle;
}

void TextElement::SetFontStyle(const FontStyle value)
{
    m_fontStyle = value;
}

ForegroundColor TextElement::GetTextColor() const
{
    return m_textColor;
}

void TextElement::SetTextColor(const ForegroundColor value)
{
    m_textColor = value;
}

bool TextElement::GetIsSubtle() const
{
    return m_isSubtle;
}

void TextElement::SetIsSubtle(const bool value)
{
    m_isSubtle = value;
}

std::string TextElement::GetLanguage() const
{
    return m_language;
}

void TextElement::SetLanguage(const std::string& value)
{
    m_language = value;
}
