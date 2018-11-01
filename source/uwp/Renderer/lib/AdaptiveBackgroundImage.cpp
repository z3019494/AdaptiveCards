#include "pch.h"
#include "AdaptiveBackgroundImage.h"

#include "Util.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Controls;

namespace AdaptiveNamespace
{
    AdaptiveBackgroundImage::AdaptiveBackgroundImage() {}

    HRESULT AdaptiveBackgroundImage::RuntimeClassInitialize() noexcept try
    {
        std::shared_ptr<AdaptiveSharedNamespace::BackgroundImage> image =
            std::make_shared<AdaptiveSharedNamespace::BackgroundImage>();
        return RuntimeClassInitialize(image);
    }
    CATCH_RETURN;

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::RuntimeClassInitialize(
        const std::shared_ptr<AdaptiveSharedNamespace::BackgroundImage>& sharedImage) try
    {
        if (sharedImage == nullptr)
        {
            return E_INVALIDARG;
        }

        RETURN_IF_FAILED(UTF8ToHString(sharedImage->GetUrl(), m_url.GetAddressOf()));

        m_mode = static_cast<ABI::AdaptiveNamespace::BackgroundImageMode>(sharedImage->GetMode());
        m_horizontalAlignment = static_cast<ABI::AdaptiveNamespace::HAlignment>(sharedImage->GetHorizontalAlignment());
        m_verticalAlignment = static_cast<ABI::AdaptiveNamespace::VAlignment>(sharedImage->GetVerticalAlignment());

        return S_OK;
    }
    CATCH_RETURN;

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::get_Url(HSTRING* url) { return m_url.CopyTo(url); }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::put_Url(HSTRING url) { return m_url.Set(url); }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::get_Mode(BackgroundImageMode* mode)
    {
        *mode = m_mode;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::put_Mode(BackgroundImageMode mode)
    {
        m_mode = mode;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::get_HorizontalAlignment(HAlignment* hAlignment)
    {
        *hAlignment = m_horizontalAlignment;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::put_HorizontalAlignment(HAlignment hAlignment)
    {
        m_horizontalAlignment = hAlignment;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::get_VerticalAlignment(VAlignment* vAlignment)
    {
        *vAlignment = m_verticalAlignment;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT AdaptiveBackgroundImage::put_VerticalAlignment(VAlignment vAlignment)
    {
        m_verticalAlignment = vAlignment;
        return S_OK;
    }

    HRESULT AdaptiveBackgroundImage::GetSharedModel(std::shared_ptr<AdaptiveSharedNamespace::BackgroundImage>& sharedImage) try
    {
        std::shared_ptr<AdaptiveSharedNamespace::BackgroundImage> image =
            std::make_shared<AdaptiveSharedNamespace::BackgroundImage>();

        image->SetUrl(HStringToUTF8(m_url.Get()));
        image->SetMode(static_cast<AdaptiveSharedNamespace::BackgroundImageMode>(m_mode));
        image->SetHorizontalAlignment(static_cast<AdaptiveSharedNamespace::HorizontalAlignment>(m_horizontalAlignment));
        image->SetVerticalAlignment(static_cast<AdaptiveSharedNamespace::VerticalAlignment>(m_verticalAlignment));

        sharedImage = image;
        return S_OK;
    }
    CATCH_RETURN;
}
