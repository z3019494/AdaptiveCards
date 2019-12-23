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

HRESULT TextInputBase::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                              ABI::AdaptiveNamespace::IAdaptiveInputElement* adaptiveInput,
                                              ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                              ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                              ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    {
        m_textBoxElement = uiTextBoxElement;

        ComPtr<IUIElement> textBoxAsUIElement;
        m_textBoxElement.As(&textBoxAsUIElement);

        InputValue::RuntimeClassInitialize(renderContext, adaptiveInput, textBoxAsUIElement.Get(), validationBorder, validationError);

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

HRESULT TextInputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                               ABI::AdaptiveNamespace::IAdaptiveTextInput* adaptiveTextInput,
                                               ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                               ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                               ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    {
        m_adaptiveTextInput = adaptiveTextInput;

        Microsoft::WRL::ComPtr<IAdaptiveInputElement> textInputAsAdaptiveInput;
        m_adaptiveTextInput.As(&textInputAsAdaptiveInput);
        TextInputBase::RuntimeClassInitialize(renderContext, textInputAsAdaptiveInput.Get(), uiTextBoxElement, validationBorder, validationError);

        return S_OK;
    }
}

HRESULT TextInputValue::IsValueValid(_Out_ boolean* isInputValid)
{
    // Call the base class to validate isRequired
    boolean isBaseValid;
    RETURN_IF_FAILED(InputValue::IsValueValid(&isBaseValid));

    // Validate the regex if one exists
    HString regex;
    RETURN_IF_FAILED(m_adaptiveTextInput->get_Regex(regex.GetAddressOf()));

    boolean isRegexValid = true;
    if (regex.IsValid())
    {
        std::string stringPattern = HStringToUTF8(regex.Get());
        std::regex pattern(stringPattern);

        HString currentValue;
        RETURN_IF_FAILED(get_CurrentValue(currentValue.GetAddressOf()));
        std::string currentValueStdString = HStringToUTF8(currentValue.Get());

        std::smatch matches;
        isRegexValid = std::regex_match(currentValueStdString, matches, pattern);
    }

    *isInputValid = isBaseValid && isRegexValid;

    return S_OK;
}

HRESULT NumberInputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                                 ABI::AdaptiveNamespace::IAdaptiveNumberInput* adaptiveNumberInput,
                                                 ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                                 ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                                 ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveNumberInput = adaptiveNumberInput;

    Microsoft::WRL::ComPtr<IAdaptiveInputElement> numberInputAsAdaptiveInput;
    m_adaptiveNumberInput.As(&numberInputAsAdaptiveInput);
    TextInputBase::RuntimeClassInitialize(renderContext, numberInputAsAdaptiveInput.Get(), uiTextBoxElement, validationBorder, validationError);
    return S_OK;
}

HRESULT NumberInputValue::IsValueValid(boolean* isInputValid)
{
    // Call the base class to validate isRequired
    boolean isBaseValid;
    RETURN_IF_FAILED(InputValue::IsValueValid(&isBaseValid));

    // Check that min and max are satisfied
    int max, min;
    RETURN_IF_FAILED(m_adaptiveNumberInput->get_Max(&max));
    RETURN_IF_FAILED(m_adaptiveNumberInput->get_Min(&min));

    // For now we're only validating if min or max was set. Theoretically we should probably validate that the input is
    // a number either way, but since we haven't enforced that in the past and the card author likely hasn't set an
    // error message in that case, dont't fail validation for non-numbers unless min or max is set.
    boolean minMaxValid = true;
    if ((min != -MAXINT32) && (max != MAXINT32))
    {
        HString currentValue;
        RETURN_IF_FAILED(get_CurrentValue(currentValue.GetAddressOf()));

        int currentInt;
        try
        {
            std::string currentValueStdString = HStringToUTF8(currentValue.Get());
            currentInt = std::stoi(currentValueStdString);
            minMaxValid = (currentInt < max) && (currentInt > min);
        }
        catch (...)
        {
            minMaxValid = false;
        }
    }

    *isInputValid = isBaseValid;

    return S_OK;
}

HRESULT DateInputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                               ABI::AdaptiveNamespace::IAdaptiveDateInput* adaptiveDateInput,
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

    InputValue::RuntimeClassInitialize(renderContext, dateInputAsAdaptiveInput.Get(), datePickerAsUIElement.Get(), validationBorder, validationError);
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

HRESULT TimeInputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                               ABI::AdaptiveNamespace::IAdaptiveTimeInput* adaptiveTimeInput,
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

    InputValue::RuntimeClassInitialize(renderContext, timeInputAsAdaptiveInput.Get(), timePickerAsUIElement.Get(), validationBorder, validationError);
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
    // Call the base class to validate isRequired
    boolean isBaseValid;
    InputValue::IsValueValid(&isBaseValid);

    TimeSpan currentTime;
    RETURN_IF_FAILED(m_timePickerElement->get_Time(&currentTime));

    // Validate max and min time
    boolean isMaxMinValid = true;

    HString minTimeString;
    RETURN_IF_FAILED(m_adaptiveTimeInput->get_Min(minTimeString.GetAddressOf()));
    if (minTimeString.IsValid())
    {
        std::string minTimeStdString = HStringToUTF8(minTimeString.Get());
        unsigned int minHours, minMinutes;
        if (DateTimePreparser::TryParseSimpleTime(minTimeStdString, minHours, minMinutes))
        {
            TimeSpan minTime{(INT64)(minHours * 60 + minMinutes) * 10000000 * 60};
            isMaxMinValid |= currentTime.Duration > minTime.Duration;
        }
    }

    HString maxTimeString;
    RETURN_IF_FAILED(m_adaptiveTimeInput->get_Max(maxTimeString.GetAddressOf()));
    if (maxTimeString.IsValid())
    {
        std::string maxTimeStdString = HStringToUTF8(maxTimeString.Get());
        unsigned int maxHours, maxMinutes;
        if (DateTimePreparser::TryParseSimpleTime(maxTimeStdString, maxHours, maxMinutes))
        {
            TimeSpan maxTime{(INT64)(maxHours * 60 + maxMinutes) * 10000000 * 60};
            isMaxMinValid |= currentTime.Duration < maxTime.Duration;
        }
    }

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

HRESULT ToggleInputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                                 _In_ ABI::AdaptiveNamespace::IAdaptiveToggleInput* adaptiveToggleInput,
                                                 _In_ ABI::Windows::UI::Xaml::Controls::ICheckBox* uiCheckBoxElement,
                                                 _In_ ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                                 _In_ ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveToggleInput = adaptiveToggleInput;
    m_checkBoxElement = uiCheckBoxElement;

    Microsoft::WRL::ComPtr<IAdaptiveInputElement> toggleInputAsAdaptiveInput;
    m_adaptiveToggleInput.As(&toggleInputAsAdaptiveInput);

    ComPtr<IUIElement> checkBoxAsUIElement;
    m_checkBoxElement.As(&checkBoxAsUIElement);

    InputValue::RuntimeClassInitialize(renderContext, toggleInputAsAdaptiveInput.Get(), checkBoxAsUIElement.Get(), validationBorder, validationError);
    return S_OK;
}

HRESULT ToggleInputValue::get_CurrentValue(HSTRING* serializedUserInput)
{
    boolean checkedValue = false;
    XamlHelpers::GetToggleValue(m_checkBoxElement.Get(), &checkedValue);

    HString value;
    if (checkedValue)
    {
        RETURN_IF_FAILED(m_adaptiveToggleInput->get_ValueOn(value.GetAddressOf()));
    }
    else
    {
        RETURN_IF_FAILED(m_adaptiveToggleInput->get_ValueOff(value.GetAddressOf()));
    }

    RETURN_IF_FAILED(value.CopyTo(serializedUserInput));

    return S_OK;
}

HRESULT ToggleInputValue::IsValueValid(boolean* isInputValid)
{
    // Don't use the base class IsValueValid to validate required. That method counts required as set if any value is set,
    // but for toggle, required means the check box is checked. An unchecked value will still have a value (either false, or whatever's in valueOff).
    boolean isRequired;
    RETURN_IF_FAILED(m_adaptiveInputElement->get_IsRequired(&isRequired));

    boolean meetsRequirement = true;
    if (isRequired)
    {
        boolean isToggleChecked = false;
        XamlHelpers::GetToggleValue(m_checkBoxElement.Get(), &isToggleChecked);

        meetsRequirement = isToggleChecked;
    }

    *isInputValid = meetsRequirement;

    return S_OK;
}

HRESULT ToggleInputValue::EnableValueChangedValidation()
{
    if (!m_isToggleChangedValidationEnabled)
    {
        ComPtr<IButtonBase> checkBoxAsButtonBase;
        RETURN_IF_FAILED(m_checkBoxElement.As(&checkBoxAsButtonBase));

        EventRegistrationToken toggleChangedToken;
        RETURN_IF_FAILED(checkBoxAsButtonBase->add_Click(Callback<IRoutedEventHandler>([this](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                                              /*args*/) -> HRESULT {
                                                             return Validate(nullptr);
                                                         }).Get(),
                                                         &toggleChangedToken));

        m_isToggleChangedValidationEnabled = true;
    }

    return S_OK;
}

std::string ChoiceSetInputValue::GetChoiceValue(_In_ IAdaptiveChoiceSetInput* choiceInput, INT32 selectedIndex) const
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

HRESULT ChoiceSetInputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                                    ABI::AdaptiveNamespace::IAdaptiveChoiceSetInput* adaptiveChoiceSetInput,
                                                    ABI::Windows::UI::Xaml::IUIElement* uiChoiceSetElement,
                                                    ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                                    ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveChoiceSetInput = adaptiveChoiceSetInput;

    Microsoft::WRL::ComPtr<IAdaptiveInputElement> choiceSetInputAsAdaptiveInput;
    RETURN_IF_FAILED(m_adaptiveChoiceSetInput.As(&choiceSetInputAsAdaptiveInput));

    InputValue::RuntimeClassInitialize(renderContext, choiceSetInputAsAdaptiveInput.Get(), uiChoiceSetElement, validationBorder, validationError);
    return S_OK;
}

HRESULT ChoiceSetInputValue::get_CurrentValue(HSTRING* serializedUserInput)
try
{
    ABI::AdaptiveNamespace::ChoiceSetStyle choiceSetStyle;
    RETURN_IF_FAILED(m_adaptiveChoiceSetInput->get_ChoiceSetStyle(&choiceSetStyle));

    boolean isMultiSelect;
    RETURN_IF_FAILED(m_adaptiveChoiceSetInput->get_IsMultiSelect(&isMultiSelect));

    if (choiceSetStyle == ABI::AdaptiveNamespace::ChoiceSetStyle_Compact && !isMultiSelect)
    {
        // Handle compact style
        ComPtr<ISelector> selector;
        RETURN_IF_FAILED(m_uiInputElement.As(&selector));

        INT32 selectedIndex;
        RETURN_IF_FAILED(selector->get_SelectedIndex(&selectedIndex));

        std::string choiceValue = GetChoiceValue(m_adaptiveChoiceSetInput.Get(), selectedIndex);
        RETURN_IF_FAILED(UTF8ToHString(choiceValue, serializedUserInput));
    }
    else
    {
        // For expanded style, get the panel children
        ComPtr<IPanel> panel;
        RETURN_IF_FAILED(m_uiInputElement.As(&panel));

        ComPtr<IVector<UIElement*>> panelChildren;
        RETURN_IF_FAILED(panel->get_Children(panelChildren.ReleaseAndGetAddressOf()));

        UINT size;
        RETURN_IF_FAILED(panelChildren->get_Size(&size));

        if (isMultiSelect)
        {
            // For multiselect, gather all the inputs in a comma delimited list
            std::string multiSelectValues;
            for (UINT i = 0; i < size; i++)
            {
                ComPtr<IUIElement> currentElement;
                RETURN_IF_FAILED(panelChildren->GetAt(i, &currentElement));

                boolean checkedValue = false;
                XamlHelpers::GetToggleValue(currentElement.Get(), &checkedValue);

                if (checkedValue)
                {
                    std::string choiceValue = GetChoiceValue(m_adaptiveChoiceSetInput.Get(), i);
                    multiSelectValues += choiceValue + ",";
                }
            }

            if (!multiSelectValues.empty())
            {
                multiSelectValues = multiSelectValues.substr(0, (multiSelectValues.size() - 1));
            }
            RETURN_IF_FAILED(UTF8ToHString(multiSelectValues, serializedUserInput));
        }
        else
        {
            // Look for the single selected choice
            INT32 selectedIndex = -1;
            for (UINT i = 0; i < size; i++)
            {
                ComPtr<IUIElement> currentElement;
                RETURN_IF_FAILED(panelChildren->GetAt(i, &currentElement));

                boolean checkedValue = false;
                XamlHelpers::GetToggleValue(currentElement.Get(), &checkedValue);

                if (checkedValue)
                {
                    selectedIndex = i;
                    break;
                }
            }
            std::string choiceValue = GetChoiceValue(m_adaptiveChoiceSetInput.Get(), selectedIndex);
            RETURN_IF_FAILED(UTF8ToHString(choiceValue, serializedUserInput));
        }
    }
    return S_OK;
}
CATCH_RETURN;

HRESULT ChoiceSetInputValue::EnableValueChangedValidation()
{
    if (!m_isChoiceSetChangedValidationEnabled)
    {
        ABI::AdaptiveNamespace::ChoiceSetStyle choiceSetStyle;
        RETURN_IF_FAILED(m_adaptiveChoiceSetInput->get_ChoiceSetStyle(&choiceSetStyle));

        boolean isMultiSelect;
        RETURN_IF_FAILED(m_adaptiveChoiceSetInput->get_IsMultiSelect(&isMultiSelect));

        if (choiceSetStyle == ABI::AdaptiveNamespace::ChoiceSetStyle_Compact && !isMultiSelect)
        {
            // Handle compact style
            ComPtr<ISelector> selector;
            RETURN_IF_FAILED(m_uiInputElement.As(&selector));

            EventRegistrationToken toggleChangedToken;
            RETURN_IF_FAILED(selector->add_SelectionChanged(Callback<ISelectionChangedEventHandler>([this](IInspectable* /*sender*/, ISelectionChangedEventArgs *
                                                                                                           /*args*/) -> HRESULT {
                                                                return Validate(nullptr);
                                                            }).Get(),
                                                            &toggleChangedToken));
        }
        else
        {
            // For expanded style, put click handlers to validate on all the choices
            ComPtr<IPanel> panel;
            RETURN_IF_FAILED(m_uiInputElement.As(&panel));

            ComPtr<IVector<UIElement*>> panelChildren;
            RETURN_IF_FAILED(panel->get_Children(panelChildren.ReleaseAndGetAddressOf()));

            UINT size;
            RETURN_IF_FAILED(panelChildren->get_Size(&size));

            for (UINT i = 0; i < size; i++)
            {
                ComPtr<IUIElement> currentElement;
                RETURN_IF_FAILED(panelChildren->GetAt(i, &currentElement));

                ComPtr<IButtonBase> elementAsButtonBase;
                RETURN_IF_FAILED(currentElement.As(&elementAsButtonBase));

                EventRegistrationToken elementChangedToken;
                RETURN_IF_FAILED(elementAsButtonBase->add_Click(Callback<IRoutedEventHandler>([this](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                                                     /*args*/) -> HRESULT {
                                                                    return Validate(nullptr);
                                                                }).Get(),
                                                                &elementChangedToken));
            }
        }
        m_isChoiceSetChangedValidationEnabled = true;
    }

    return S_OK;
}

HRESULT ChoiceSetInputValue::EnableFocusLostValidation()
{
    ABI::AdaptiveNamespace::ChoiceSetStyle choiceSetStyle;
    RETURN_IF_FAILED(m_adaptiveChoiceSetInput->get_ChoiceSetStyle(&choiceSetStyle));

    boolean isMultiSelect;
    RETURN_IF_FAILED(m_adaptiveChoiceSetInput->get_IsMultiSelect(&isMultiSelect));

    if (choiceSetStyle == ABI::AdaptiveNamespace::ChoiceSetStyle_Compact && !isMultiSelect)
    {
        // Compact style can use the base class implementation
        RETURN_IF_FAILED(InputValue::EnableFocusLostValidation());
    }
    else
    {
        // For expanded style, put a focus lost handler on the last choice in the choice set
        ComPtr<IPanel> panel;
        RETURN_IF_FAILED(m_uiInputElement.As(&panel));

        ComPtr<IVector<UIElement*>> panelChildren;
        RETURN_IF_FAILED(panel->get_Children(panelChildren.ReleaseAndGetAddressOf()));

        UINT size;
        RETURN_IF_FAILED(panelChildren->get_Size(&size));

        ComPtr<IUIElement> lastElement;
        RETURN_IF_FAILED(panelChildren->GetAt(size - 1, &lastElement));

        EventRegistrationToken focusLostToken;
        RETURN_IF_FAILED(lastElement->add_LostFocus(Callback<IRoutedEventHandler>([this](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                                         /*args*/) -> HRESULT {
                                                        return Validate(nullptr);
                                                    }).Get(),
                                                    &focusLostToken));
    }

    return S_OK;
}

HRESULT InputValue::RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveRenderContext* renderContext,
                                           _In_ ABI::AdaptiveNamespace::IAdaptiveInputElement* adaptiveInputElement,
                                           _In_ ABI::Windows::UI::Xaml::IUIElement* uiInputElement,
                                           _In_ ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder,
                                           _In_ ABI::Windows::UI::Xaml::IUIElement* validationError)
{
    m_adaptiveInputElement = adaptiveInputElement;
    m_uiInputElement = uiInputElement;
    m_validationError = validationError;
    m_validationBorder = validationBorder;

    ComPtr<AdaptiveRenderContext> renderContextPeek = PeekInnards<AdaptiveRenderContext>(renderContext);

    boolean inlineValidation;
    RETURN_IF_FAILED(renderContextPeek->GetInlineValidation(&inlineValidation));

    if (inlineValidation)
    {
        RETURN_IF_FAILED(EnableFocusLostValidation());
    }

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

HRESULT InputValue::SetValidation(boolean isInputValid)
{
    // Show or hide the border
    if (m_validationBorder)
    {
        isInputValid ? m_validationBorder->put_BorderThickness({0, 0, 0, 0}) :
                       m_validationBorder->put_BorderThickness({1, 1, 1, 1});
    }

    // Show or hide the error message
    if (m_validationError)
    {
        isInputValid ? m_validationError->put_Visibility(Visibility_Collapsed) :
                       m_validationError->put_Visibility(Visibility_Visible);
    }

    // Once this has been marked invalid once, we should validate on all value changess going forward
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

HRESULT InputValue::get_InputElement(_COM_Outptr_ IAdaptiveInputElement** inputElement)
{
    return m_adaptiveInputElement.CopyTo(inputElement);
}
