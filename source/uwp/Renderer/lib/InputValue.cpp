// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "InputValue.h"
#include "json/json.h"
#include "XamlHelpers.h"
#include <windows.globalization.datetimeformatting.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::Globalization::DateTimeFormatting;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;
using namespace ABI::Windows::UI::Xaml::Controls::Primitives;
using namespace AdaptiveNamespace;

HRESULT TextInputBase::RuntimeClassInitialize(ABI::AdaptiveNamespace::IAdaptiveInputElement* adaptiveInput,
                                              ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                              ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                              ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    {
        m_textBoxElement = uiTextBoxElement;

        ComPtr<IUIElement> textBoxAsUIElement;
        m_textBoxElement.As(&textBoxAsUIElement);

        InputValue::RuntimeClassInitialize(adaptiveInput, textBoxAsUIElement.Get(), validationBorder, validationError);

        return S_OK;
    }
}

HRESULT TextInputBase::get_CurrentValue(HSTRING* serializedUserInput)
{
    HString text;
    THROW_IF_FAILED(m_textBoxElement->get_Text(text.GetAddressOf()));

    text.CopyTo(serializedUserInput);
    return S_OK;
}

HRESULT TextInputBase::EnableValueChangedValidation()
{
    if (!m_isTextChangedValidationEnabled)
    {
        EventRegistrationToken textChangedToken;
        m_textBoxElement->add_TextChanged(Callback<ITextChangedEventHandler>([this](IInspectable* /*sender*/, ITextChangedEventArgs *
                                                                                    /*args*/) -> HRESULT {
                                              return Validate(nullptr);
                                          }).Get(),
                                          &textChangedToken);

        m_isTextChangedValidationEnabled = true;
    }
    return S_OK;
}

HRESULT TextInputValue::RuntimeClassInitialize(ABI::AdaptiveNamespace::IAdaptiveTextInput* adaptiveTextInput,
                                               ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                               ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                               ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    {
        m_adaptiveTextInput = adaptiveTextInput;

        Microsoft::WRL::ComPtr<IAdaptiveInputElement> textInputAsAdaptiveInput;
        m_adaptiveTextInput.As(&textInputAsAdaptiveInput);
        TextInputBase::RuntimeClassInitialize(textInputAsAdaptiveInput.Get(), uiTextBoxElement, validationBorder, validationError);

        return S_OK;
    }
}

HRESULT TextInputValue::IsValueValid(_Out_ boolean* isInputValid)
{
    boolean isBaseValid;
    InputValue::IsValueValid(&isBaseValid);

    boolean isRegexValid = true; // TODO
    HString regex;
    m_adaptiveTextInput->get_Regex(regex.GetAddressOf());

    *isInputValid = isBaseValid && isRegexValid;

    return S_OK;
}

HRESULT NumberInputValue::RuntimeClassInitialize(ABI::AdaptiveNamespace::IAdaptiveNumberInput* adaptiveNumberInput,
                                                 ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                                 ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                                 ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveNumberInput = adaptiveNumberInput;

    Microsoft::WRL::ComPtr<IAdaptiveInputElement> numberInputAsAdaptiveInput;
    m_adaptiveNumberInput.As(&numberInputAsAdaptiveInput);
    TextInputBase::RuntimeClassInitialize(numberInputAsAdaptiveInput.Get(), uiTextBoxElement, validationBorder, validationError);
    return S_OK;
}

HRESULT NumberInputValue::IsValueValid(boolean* isInputValid)
{
    boolean isBaseValid;
    InputValue::IsValueValid(&isBaseValid);

    // TODO
    boolean isMaxMinValid = true;
    // int max;
    // m_adaptiveNumberInput->get_Max(&max);
    // int min;
    // m_adaptiveNumberInput->get_Max(&max);

    *isInputValid = isBaseValid && isMaxMinValid;

    return S_OK;
}

HRESULT DateInputValue::RuntimeClassInitialize(ABI::AdaptiveNamespace::IAdaptiveDateInput* adaptiveDateInput,
                                               ABI::Windows::UI::Xaml::Controls::ICalendarDatePicker* uiDatePickerElement,
                                               ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                               ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveDateInput = adaptiveDateInput;
    m_datePickerElement = uiDatePickerElement;

    Microsoft::WRL::ComPtr<IAdaptiveInputElement> dateInputAsAdaptiveInput;
    m_adaptiveDateInput.As(&dateInputAsAdaptiveInput);

    ComPtr<IUIElement> datePickerAsUIElement;
    m_datePickerElement.As(&datePickerAsUIElement);

    InputValue::RuntimeClassInitialize(dateInputAsAdaptiveInput.Get(), datePickerAsUIElement.Get(), validationBorder, validationError);
    return S_OK;
}

HRESULT DateInputValue::get_CurrentValue(HSTRING* serializedUserInput)
{
    ComPtr<IReference<DateTime>> dateRef;
    RETURN_IF_FAILED(m_datePickerElement->get_Date(&dateRef));

    HString formattedDate;
    if (dateRef != nullptr)
    {
        DateTime date;
        RETURN_IF_FAILED(dateRef->get_Value(&date));

        ComPtr<IDateTimeFormatterFactory> dateTimeFactory;
        RETURN_IF_FAILED(GetActivationFactory(
            HStringReference(RuntimeClass_Windows_Globalization_DateTimeFormatting_DateTimeFormatter).Get(), &dateTimeFactory));

        ComPtr<IDateTimeFormatter> dateTimeFormatter;
        RETURN_IF_FAILED(dateTimeFactory->CreateDateTimeFormatter(
            HStringReference(L"{year.full}-{month.integer(2)}-{day.integer(2)}").Get(), &dateTimeFormatter));

        RETURN_IF_FAILED(dateTimeFormatter->Format(date, formattedDate.GetAddressOf()));
    }

    formattedDate.CopyTo(serializedUserInput);

    return S_OK;
}

HRESULT DateInputValue::EnableValueChangedValidation()
{
    if (!m_isDateChangedValidationEnabled)
    {
        EventRegistrationToken dateChangedToken;
        m_datePickerElement->add_DateChanged(Callback<ITypedEventHandler<CalendarDatePicker*, CalendarDatePickerDateChangedEventArgs*>>(
                                                 [this](IInspectable* /*sender*/, ICalendarDatePickerDateChangedEventArgs *
                                                        /*args*/) -> HRESULT { return Validate(nullptr); })
                                                 .Get(),
                                             &dateChangedToken);

        m_isDateChangedValidationEnabled = true;
    }
    return S_OK;
}

HRESULT TimeInputValue::RuntimeClassInitialize(ABI::AdaptiveNamespace::IAdaptiveTimeInput* adaptiveTimeInput,
                                               ABI::Windows::UI::Xaml::Controls::ITimePicker* uiTimePickerElement,
                                               ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                               ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveTimeInput = adaptiveTimeInput;
    m_timePickerElement = uiTimePickerElement;

    Microsoft::WRL::ComPtr<IAdaptiveInputElement> timeInputAsAdaptiveInput;
    m_adaptiveTimeInput.As(&timeInputAsAdaptiveInput);

    ComPtr<IUIElement> timePickerAsUIElement;
    m_timePickerElement.As(&timePickerAsUIElement);

    InputValue::RuntimeClassInitialize(timeInputAsAdaptiveInput.Get(), timePickerAsUIElement.Get(), validationBorder, validationError);
    return S_OK;
}

HRESULT TimeInputValue::get_CurrentValue(HSTRING* serializedUserInput)
{
    TimeSpan timeSpan;
    RETURN_IF_FAILED(m_timePickerElement->get_Time(&timeSpan));

    UINT64 totalMinutes = timeSpan.Duration / 10000000 / 60;
    UINT64 hours = totalMinutes / 60;
    UINT64 minutesPastTheHour = totalMinutes - (hours * 60);

    char buffer[6];
    sprintf_s(buffer, sizeof(buffer), "%02llu:%02llu", hours, minutesPastTheHour);

    RETURN_IF_FAILED(UTF8ToHString(std::string(buffer), serializedUserInput));

    return S_OK;
}

HRESULT TimeInputValue::IsValueValid(boolean* isInputValid)
{
    boolean isBaseValid;
    InputValue::IsValueValid(&isBaseValid);

    // TODO
    boolean isMaxMinValid = true;
    // int max;
    // m_adaptiveNumberInput->get_Max(&max);
    // int min;
    // m_adaptiveNumberInput->get_Max(&max);

    *isInputValid = isBaseValid && isMaxMinValid;

    return S_OK;
}

HRESULT TimeInputValue::EnableValueChangedValidation()
{
    if (!m_isTimeChangedValidationEnabled)
    {
        EventRegistrationToken dateChangedToken;
        m_timePickerElement->add_TimeChanged(Callback<IEventHandler<TimePickerValueChangedEventArgs*>>([this](IInspectable* /*sender*/, ITimePickerValueChangedEventArgs *
                                                                                                              /*args*/) -> HRESULT {
                                                 return Validate(nullptr);
                                             }).Get(),
                                             &dateChangedToken);

        m_isTimeChangedValidationEnabled = true;
    }
    return S_OK;
}

std::string InputValue::SerializeToggleInput() const
{
    boolean checkedValue = false;
    XamlHelpers::GetToggleValue(m_uiInputElement.Get(), &checkedValue);

    ComPtr<IAdaptiveToggleInput> toggleInput;
    THROW_IF_FAILED(m_adaptiveInputElement.As(&toggleInput));

    HString value;
    if (checkedValue)
    {
        THROW_IF_FAILED(toggleInput->get_ValueOn(value.GetAddressOf()));
    }
    else
    {
        THROW_IF_FAILED(toggleInput->get_ValueOff(value.GetAddressOf()));
    }

    std::string utf8Value;
    THROW_IF_FAILED(HStringToUTF8(value.Get(), utf8Value));

    return utf8Value;
}

std::string InputValue::GetChoiceValue(_In_ IAdaptiveChoiceSetInput* choiceInput, INT32 selectedIndex) const
{
    if (selectedIndex != -1)
    {
        ComPtr<IVector<AdaptiveChoiceInput*>> choices;
        THROW_IF_FAILED(choiceInput->get_Choices(&choices));

        ComPtr<IAdaptiveChoiceInput> choice;
        THROW_IF_FAILED(choices->GetAt(selectedIndex, &choice));

        HString value;
        THROW_IF_FAILED(choice->get_Value(value.GetAddressOf()));

        return HStringToUTF8(value.Get());
    }
    return "";
}

std::string InputValue::SerializeChoiceSetInput() const
{
    ComPtr<IAdaptiveChoiceSetInput> choiceInput;
    THROW_IF_FAILED(m_adaptiveInputElement.As(&choiceInput));

    ABI::AdaptiveNamespace::ChoiceSetStyle choiceSetStyle;
    THROW_IF_FAILED(choiceInput->get_ChoiceSetStyle(&choiceSetStyle));

    boolean isMultiSelect;
    THROW_IF_FAILED(choiceInput->get_IsMultiSelect(&isMultiSelect));

    if (choiceSetStyle == ABI::AdaptiveNamespace::ChoiceSetStyle_Compact && !isMultiSelect)
    {
        // Handle compact style
        ComPtr<ISelector> selector;
        THROW_IF_FAILED(m_uiInputElement.As(&selector));

        INT32 selectedIndex;
        THROW_IF_FAILED(selector->get_SelectedIndex(&selectedIndex));

        std::string choiceValue;
        return GetChoiceValue(choiceInput.Get(), selectedIndex);
    }
    else
    {
        // For expanded style, get the panel children
        ComPtr<IPanel> panel;
        THROW_IF_FAILED(m_uiInputElement.As(&panel));

        ComPtr<IVector<UIElement*>> panelChildren;
        THROW_IF_FAILED(panel->get_Children(panelChildren.ReleaseAndGetAddressOf()));

        UINT size;
        THROW_IF_FAILED(panelChildren->get_Size(&size));

        if (isMultiSelect)
        {
            // For multiselect, gather all the inputs in a comma delimited list
            std::string multiSelectValues;
            for (UINT i = 0; i < size; i++)
            {
                ComPtr<IUIElement> currentElement;
                THROW_IF_FAILED(panelChildren->GetAt(i, &currentElement));

                boolean checkedValue = false;
                XamlHelpers::GetToggleValue(currentElement.Get(), &checkedValue);

                if (checkedValue)
                {
                    std::string choiceValue = GetChoiceValue(choiceInput.Get(), i);
                    multiSelectValues += choiceValue + ",";
                }
            }

            if (!multiSelectValues.empty())
            {
                multiSelectValues = multiSelectValues.substr(0, (multiSelectValues.size() - 1));
            }
            return multiSelectValues;
        }
        else
        {
            // Look for the single selected choice
            INT32 selectedIndex = -1;
            for (UINT i = 0; i < size; i++)
            {
                ComPtr<IUIElement> currentElement;
                THROW_IF_FAILED(panelChildren->GetAt(i, &currentElement));

                boolean checkedValue = false;
                XamlHelpers::GetToggleValue(currentElement.Get(), &checkedValue);

                if (checkedValue)
                {
                    selectedIndex = i;
                    break;
                }
            }
            return GetChoiceValue(choiceInput.Get(), selectedIndex);
        }
    }
}

HRESULT InputValue::get_CurrentValue(_Outptr_ HSTRING* result)
{
    ComPtr<IAdaptiveCardElement> cardElement;
    RETURN_IF_FAILED(m_adaptiveInputElement.As(&cardElement));

    ABI::AdaptiveNamespace::ElementType elementType;
    RETURN_IF_FAILED(cardElement->get_ElementType(&elementType));

    std::string serializedInput;
    switch (elementType)
    {
    case ElementType_ToggleInput:
    {
        serializedInput = SerializeToggleInput();
        break;
    }
    case ElementType_ChoiceSetInput:
    {
        serializedInput = SerializeChoiceSetInput();
        break;
    }
    default:
        serializedInput = "";
        break;
    }

    RETURN_IF_FAILED(UTF8ToHString(serializedInput, result));

    return S_OK;
}

HRESULT InputValue::Validate(boolean* isInputValid)
{
    boolean isValid;
    IsValueValid(&isValid);

    SetValidation(isValid);

    if (isInputValid)
    {
        *isInputValid = isValid;
    }

    return S_OK;
}

HRESULT InputValue::IsValueValid(boolean* isInputValid)
{
    boolean isRequired;
    m_adaptiveInputElement->get_IsRequired(&isRequired);

    bool isRequiredValid = true;
    if (isRequired)
    {
        HString currentValue;
        get_CurrentValue(currentValue.GetAddressOf());

        isRequiredValid = currentValue.IsValid();
    }

    *isInputValid = isRequiredValid;
    return S_OK;
}

HRESULT AdaptiveNamespace::InputValue::SetValidation(boolean isInputValid)
{
    // Show or hide the border
    if (m_validationBorder)
    {
        isInputValid ? m_validationBorder->put_BorderThickness({0, 0, 0, 0}) :
                       m_validationBorder->put_BorderThickness({2, 2, 2, 2});
    }

    // Show or hide the error message
    if (m_validationError)
    {
        isInputValid ? m_validationError->put_Visibility(Visibility_Collapsed) :
                       m_validationError->put_Visibility(Visibility_Visible);
    }

    // Once this has been marked invalid once, we should validate on all value changess
    if (!isInputValid)
    {
        EnableValueChangedValidation();
    }

    return S_OK;
}

HRESULT InputValue::EnableFocusLostValidation()
{
    EventRegistrationToken focusLostToken;
    m_uiInputElement->add_LostFocus(Callback<IRoutedEventHandler>([this](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                         /*args*/) -> HRESULT {
                                        return Validate(nullptr);
                                    }).Get(),
                                    &focusLostToken);

    return S_OK;
}

HRESULT InputValue::EnableValueChangedValidation()
{
    return S_OK;
}

HRESULT InputValue::get_InputElement(_COM_Outptr_ IAdaptiveInputElement** inputElement)
{
    return m_adaptiveInputElement.CopyTo(inputElement);
}
