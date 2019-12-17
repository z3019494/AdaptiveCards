// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#pragma once

#include "AdaptiveCards.Rendering.Uwp.h"

namespace AdaptiveNamespace
{
    class DECLSPEC_UUID("BB1D1269-2243-4F34-B4EC-5216296EBBA0") InputValue
        : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>, ABI::AdaptiveNamespace::IAdaptiveInputValue>
    {
    public:
        HRESULT RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveInputElement* adaptiveInputElement,
                                       _In_ ABI::Windows::UI::Xaml::IUIElement* uiInputElement)
        {
            m_adaptiveInputElement = adaptiveInputElement;
            m_uiInputElement = uiInputElement;
            return S_OK;
        }

        IFACEMETHODIMP get_InputElement(_COM_Outptr_ ABI::AdaptiveNamespace::IAdaptiveInputElement** inputElement);
        IFACEMETHODIMP get_CurrentValue(_Outptr_ HSTRING* serializedUserInput);

        IFACEMETHODIMP Validate(_Out_ boolean* isInputValid);

    private:
        std::string SerializeChoiceSetInput() const;
        std::string SerializeDateInput() const;
        std::string SerializeTextInput() const;
        std::string SerializeTimeInput() const;
        std::string SerializeToggleInput() const;

        std::string GetChoiceValue(_In_ ABI::AdaptiveNamespace::IAdaptiveChoiceSetInput* choiceInput, INT32 selectedIndex) const;

        Microsoft::WRL::ComPtr<ABI::AdaptiveNamespace::IAdaptiveInputElement> m_adaptiveInputElement;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::IUIElement> m_uiInputElement;
    };

    class TextInputValue : public InputValue
    {
    public:

        TextInputValue() : m_isTextChangedValidationEnabled(false){}

        HRESULT RuntimeClassInitialize(_In_ ABI::AdaptiveNamespace::IAdaptiveTextInput* adaptiveTextInput,
                                       _In_ ABI::Windows::UI::Xaml::Controls::ITextBox* uiTextBoxElement,
                                       _In_ ABI::Windows::UI::Xaml::Controls::IBorder* validationBorder)
        {
            m_adaptiveTextInput = adaptiveTextInput;
            m_textBoxElement = uiTextBoxElement;
            m_validationBorder = validationBorder;

            //RETURN_IF_FAILED(EnableFocusLostValidation());

            return S_OK;
        }

        IFACEMETHODIMP get_InputElement(_COM_Outptr_ ABI::AdaptiveNamespace::IAdaptiveInputElement** inputElement) override;
        IFACEMETHODIMP get_CurrentValue(_Outptr_ HSTRING* serializedUserInput) override;
        IFACEMETHODIMP Validate(_Out_ boolean* isInputValid) override;

    private:
        HRESULT EnableFocusLostValidation();
        HRESULT EnableTextChangedValidation();

        Microsoft::WRL::ComPtr<ABI::AdaptiveNamespace::IAdaptiveTextInput> m_adaptiveTextInput;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Controls::ITextBox> m_textBoxElement;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Controls::IBorder> m_validationBorder;
        bool m_isTextChangedValidationEnabled;
    };

}
