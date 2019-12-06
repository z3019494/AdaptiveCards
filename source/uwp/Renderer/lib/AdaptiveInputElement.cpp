// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"

#include "AdaptiveInputElement.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;

namespace AdaptiveNamespace
{
    HRESULT AdaptiveInputElementBase::InitializeBaseElement(const std::shared_ptr<AdaptiveSharedNamespace::BaseInputElement>& sharedModel)
    {
        AdaptiveCardElementBase::InitializeBaseElement(std::static_pointer_cast<AdaptiveSharedNamespace::BaseCardElement>(sharedModel));
        m_isRequired = sharedModel->GetIsRequired();
        RETURN_IF_FAILED(UTF8ToHString(sharedModel->GetErrorMessage(), m_errorMessage.GetAddressOf()));

		RETURN_IF_FAILED(GenerateElementProjection(sharedModel->GetLabel(), &m_label));

        return S_OK;
    }

    HRESULT AdaptiveInputElementBase::get_IsRequired(_Out_ boolean* isRequired)
    {
        *isRequired = m_isRequired;
        return S_OK;
    }

    HRESULT AdaptiveInputElementBase::put_IsRequired(boolean isRequired)
    {
        m_isRequired = isRequired;
        return S_OK;
    }

    HRESULT AdaptiveInputElementBase::get_ErrorMessage(HSTRING* title) { return m_errorMessage.CopyTo(title); }

    HRESULT AdaptiveInputElementBase::put_ErrorMessage(HSTRING title) { return m_errorMessage.Set(title); }

    HRESULT AdaptiveInputElementBase::get_Label(_COM_Outptr_ IAdaptiveCardElement** label)
    {
        return m_label.CopyTo(label);
    }

    HRESULT AdaptiveInputElementBase::put_Label(_In_ IAdaptiveCardElement* label)
    {
        m_label = label;
        return S_OK;
    }

    HRESULT AdaptiveInputElementBase::SetSharedElementProperties(std::shared_ptr<AdaptiveSharedNamespace::BaseInputElement> sharedCardElement)
    {
        AdaptiveCardElementBase::SetSharedElementProperties(sharedCardElement);
        sharedCardElement->SetIsRequired(m_isRequired);
        sharedCardElement->SetErrorMessage(HStringToUTF8(m_errorMessage.Get()));

        if (m_label != nullptr)
        {
            std::shared_ptr<BaseCardElement> sharedElement;
            RETURN_IF_FAILED(GenerateSharedElement(m_label.Get(), sharedElement));
            sharedCardElement->SetLabel(sharedElement);
        }
        return S_OK;
    }
}
