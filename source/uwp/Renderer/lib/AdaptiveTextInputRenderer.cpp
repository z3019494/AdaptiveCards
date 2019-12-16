// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include "AdaptiveElementParserRegistration.h"
#include "AdaptiveTextInput.h"
#include "AdaptiveTextInputRenderer.h"
#include "ActionHelpers.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;
using namespace ABI::Windows::UI::Xaml::Input;

namespace AdaptiveNamespace
{
    HRESULT AdaptiveTextInputRenderer::RuntimeClassInitialize() noexcept
    try
    {
        return S_OK;
    }
    CATCH_RETURN;

    HRESULT HandleLabelActionAndValidation(IAdaptiveTextInput* adaptiveTextInput,
                                           ITextBox* textBox,
                                           _In_ IAdaptiveRenderContext* renderContext,
                                           _In_ IAdaptiveRenderArgs* renderArgs,
                                           IUIElement** inputLayout)
    {
        // Create a stack panel for the input and related controls
        ComPtr<IStackPanel> inputStackPanel =
            XamlHelpers::CreateXamlClass<IStackPanel>(HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_StackPanel));

        ComPtr<IPanel> stackPanelAsPanel;
        inputStackPanel.As(&stackPanelAsPanel);

        ComPtr<IAdaptiveTextInput> localTextInput(adaptiveTextInput);
        ComPtr<IAdaptiveInputElement> textInputAsAdaptiveInput;
        localTextInput.As(&textInputAsAdaptiveInput);

        // Render the label and add it to the stack panel (BECKYTODO - this should be a header, but right now i
        // can't figure out how to get a border around just the text box if i do i that way)
        ComPtr<IUIElement> label;
        XamlHelpers::RenderInputLabel(textInputAsAdaptiveInput.Get(), renderContext, renderArgs, &label);
        XamlHelpers::AppendXamlElementToPanel(label.Get(), stackPanelAsPanel.Get());

        ComPtr<ITextBox> localTextBox(textBox);
        ComPtr<IUIElement> textBoxAsUIElement;
        localTextBox.As(&textBoxAsUIElement);

        // The text box may need to go into a number of parent containers to handle validation and inline actions before
        // being added to the stack panel. textBoxParentContainer represents the current parent container.
        ComPtr<IUIElement> textBoxParentContainer = textBoxAsUIElement;

        // If there's any validation on this input, put the input inside a border
        HString regex;
        adaptiveTextInput->get_Regex(regex.GetAddressOf());
        boolean isRequired;
        textInputAsAdaptiveInput->get_IsRequired(&isRequired);

        ComPtr<IBorder> validationBorder;
        if (regex.IsValid() || isRequired)
        {
            // Create a border in the attention color. The thickness is 0 for now so it won't be visibile until validation is run
            validationBorder =
                XamlHelpers::CreateXamlClass<IBorder>(HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Border));

            ComPtr<IAdaptiveHostConfig> hostConfig;
            renderContext->get_HostConfig(&hostConfig);

            ABI::Windows::UI::Color attentionColor;
            RETURN_IF_FAILED(GetColorFromAdaptiveColor(hostConfig.Get(),
                                                       ABI::AdaptiveNamespace::ForegroundColor_Attention,
                                                       ABI::AdaptiveNamespace::ContainerStyle_Default,
                                                       false, // isSubtle
                                                       false, // highlight
                                                       &attentionColor));

            RETURN_IF_FAILED(validationBorder->put_BorderBrush(XamlHelpers::GetSolidColorBrush(attentionColor).Get()));

            RETURN_IF_FAILED(validationBorder->put_Child(textBoxAsUIElement.Get()));
            validationBorder.As(&textBoxParentContainer);
        }

        // If this input has an inline action, render it next to the input
        ComPtr<IAdaptiveActionElement> inlineAction;
        RETURN_IF_FAILED(adaptiveTextInput->get_InlineAction(&inlineAction));

        if (inlineAction != nullptr)
        {
            ComPtr<IUIElement> textBoxWithInlineAction;
            ActionHelpers::HandleInlineAction(
                renderContext, renderArgs, textBox, textBoxParentContainer.Get(), inlineAction.Get(), &textBoxWithInlineAction);
            textBoxParentContainer = textBoxWithInlineAction;
        }

        boolean isMultiline;
        adaptiveTextInput->get_IsMultiline(&isMultiline);

        if (!isMultiline)
        {
            ComPtr<IFrameworkElement> textBoxFrameworkElement;
            textBoxParentContainer.As(&textBoxFrameworkElement);
            RETURN_IF_FAILED(textBoxFrameworkElement->put_VerticalAlignment(ABI::Windows::UI::Xaml::VerticalAlignment_Top));
        }

        XamlHelpers::AppendXamlElementToPanel(textBoxParentContainer.Get(), stackPanelAsPanel.Get());

        ComPtr<TextInputValue> input;
        MakeAndInitialize<TextInputValue>(&input, adaptiveTextInput, textBox, validationBorder.Get());
        renderContext->AddInputValue(input.Get());

        stackPanelAsPanel.CopyTo(inputLayout);
        return S_OK;
    }

    HRESULT AdaptiveTextInputRenderer::Render(_In_ IAdaptiveCardElement* adaptiveCardElement,
                                              _In_ IAdaptiveRenderContext* renderContext,
                                              _In_ IAdaptiveRenderArgs* renderArgs,
                                              _COM_Outptr_ IUIElement** textInputControl) noexcept
    try
    {
        ComPtr<IAdaptiveHostConfig> hostConfig;
        RETURN_IF_FAILED(renderContext->get_HostConfig(&hostConfig));
        if (!XamlHelpers::SupportsInteractivity(hostConfig.Get()))
        {
            renderContext->AddWarning(
                ABI::AdaptiveNamespace::WarningStatusCode::InteractivityNotSupported,
                HStringReference(L"Text Input was stripped from card because interactivity is not supported").Get());
            return S_OK;
        }

        ComPtr<IAdaptiveCardElement> cardElement(adaptiveCardElement);
        ComPtr<IAdaptiveTextInput> adaptiveTextInput;
        RETURN_IF_FAILED(cardElement.As(&adaptiveTextInput));

        ComPtr<ITextBox> textBox =
            XamlHelpers::CreateXamlClass<ITextBox>(HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_TextBox));

        EventRegistrationToken textChangedToken;
        textBox->add_TextChanged(Callback<ITextChangedEventHandler>([](IInspectable* /*sender*/, ITextChangedEventArgs *
                                                                       /*args*/) -> HRESULT {
                                     return S_OK;
                                 }).Get(),
                                 &textChangedToken);

        ComPtr<IUIElement> textBoxAsUIElement;
        RETURN_IF_FAILED(textBox.As(&textBoxAsUIElement));

        EventRegistrationToken focusLostToken;
        textBoxAsUIElement->add_LostFocus(Callback<IRoutedEventHandler>([](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                           /*args*/) -> HRESULT {
                                              return S_OK;
                                          }).Get(),
                                          &focusLostToken);

        boolean isMultiLine;
        RETURN_IF_FAILED(adaptiveTextInput->get_IsMultiline(&isMultiLine));
        RETURN_IF_FAILED(textBox->put_AcceptsReturn(isMultiLine));

        HString textValue;
        RETURN_IF_FAILED(adaptiveTextInput->get_Value(textValue.GetAddressOf()));
        RETURN_IF_FAILED(textBox->put_Text(textValue.Get()));

        UINT32 maxLength;
        RETURN_IF_FAILED(adaptiveTextInput->get_MaxLength(&maxLength));
        RETURN_IF_FAILED(textBox->put_MaxLength(maxLength));

        ComPtr<ITextBox2> textBox2;
        RETURN_IF_FAILED(textBox.As(&textBox2));

        HString placeHolderText;
        RETURN_IF_FAILED(adaptiveTextInput->get_Placeholder(placeHolderText.GetAddressOf()));
        RETURN_IF_FAILED(textBox2->put_PlaceholderText(placeHolderText.Get()));

        ABI::AdaptiveNamespace::TextInputStyle textInputStyle;
        RETURN_IF_FAILED(adaptiveTextInput->get_TextInputStyle(&textInputStyle));

        ComPtr<IInputScopeName> inputScopeName =
            XamlHelpers::CreateXamlClass<IInputScopeName>(HStringReference(RuntimeClass_Windows_UI_Xaml_Input_InputScopeName));
        switch (textInputStyle)
        {
        case ABI::AdaptiveNamespace::TextInputStyle::Email:
            RETURN_IF_FAILED(inputScopeName->put_NameValue(InputScopeNameValue::InputScopeNameValue_EmailSmtpAddress));
            break;

        case ABI::AdaptiveNamespace::TextInputStyle::Tel:
            RETURN_IF_FAILED(inputScopeName->put_NameValue(InputScopeNameValue::InputScopeNameValue_TelephoneNumber));
            break;

        case ABI::AdaptiveNamespace::TextInputStyle::Url:
            RETURN_IF_FAILED(inputScopeName->put_NameValue(InputScopeNameValue::InputScopeNameValue_Url));
            break;
        }

        ComPtr<IInputScope> inputScope =
            XamlHelpers::CreateXamlClass<IInputScope>(HStringReference(RuntimeClass_Windows_UI_Xaml_Input_InputScope));
        ComPtr<IVector<InputScopeName*>> names;
        RETURN_IF_FAILED(inputScope->get_Names(names.GetAddressOf()));
        RETURN_IF_FAILED(names->Append(inputScopeName.Get()));

        RETURN_IF_FAILED(textBox->put_InputScope(inputScope.Get()));

        ComPtr<IFrameworkElement> textBoxAsFrameworkElement;
        RETURN_IF_FAILED(textBox.As(&textBoxAsFrameworkElement));
        RETURN_IF_FAILED(
            XamlHelpers::SetStyleFromResourceDictionary(renderContext, L"Adaptive.Input.Text", textBoxAsFrameworkElement.Get()));

        HandleLabelActionAndValidation(adaptiveTextInput.Get(), textBox.Get(), renderContext, renderArgs, textInputControl);

        return S_OK;
    }
    CATCH_RETURN;

    HRESULT AdaptiveTextInputRenderer::FromJson(
        _In_ ABI::Windows::Data::Json::IJsonObject* jsonObject,
        _In_ ABI::AdaptiveNamespace::IAdaptiveElementParserRegistration* elementParserRegistration,
        _In_ ABI::AdaptiveNamespace::IAdaptiveActionParserRegistration* actionParserRegistration,
        _In_ ABI::Windows::Foundation::Collections::IVector<ABI::AdaptiveNamespace::AdaptiveWarning*>* adaptiveWarnings,
        _COM_Outptr_ ABI::AdaptiveNamespace::IAdaptiveCardElement** element) noexcept
    try
    {
        return AdaptiveNamespace::FromJson<AdaptiveNamespace::AdaptiveTextInput, AdaptiveSharedNamespace::TextInput, AdaptiveSharedNamespace::TextInputParser>(
            jsonObject, elementParserRegistration, actionParserRegistration, adaptiveWarnings, element);
    }
    CATCH_RETURN;
}
