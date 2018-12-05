#pragma once

#include "pch.h"
#include <windows.ui.xaml.shapes.h>
#include "AdaptiveCards.Rendering.Uwp.h"
#include "Enums.h"

//#ifdef ADAPTIVE_CARDS_WINDOWS
// using namespace InternalNamespace;
//#endif

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation::Numerics;
using namespace ABI::Windows::Foundation::Collections;

// XAML STUFF
using namespace ABI::Windows::Storage;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Shapes;
using namespace ABI::Windows::UI::Xaml::Controls;
using namespace ABI::Windows::UI::Xaml::Media;
using namespace ABI::Windows::UI::Xaml::Media::Imaging;

// KEEP FOR NOW, CLEAN UP LATER
using namespace ABI::Windows::Storage::Streams;
using namespace ABI::Windows::Storage::FileProperties;

namespace AdaptiveNamespace
{
    class DECLSPEC_UUID("0F485063-EF2A-400E-A946-73E00EDFAC83") TileControl
        : public Microsoft::WRL::RuntimeClass<ABI::AdaptiveNamespace::ITileControl,
                                              ABI::Windows::UI::Xaml::IFrameworkElementOverrides,
                                              Microsoft::WRL::CloakedIid<ITypePeek>,
                                              Microsoft::WRL::ComposableBase<ABI::Windows::UI::Xaml::Controls::IContentControlFactory>>
    {
        AdaptiveRuntimeStringClass(TileControl);

    public:
        HRESULT RuntimeClassInitialize() noexcept;
        // HRESULT RuntimeClassInitialize(_In_ IAdaptiveRenderContext* context, _In_ IAdaptiveBackgroundImage* backgroundImage) noexcept;

        virtual HRESULT STDMETHODCALLTYPE put_BackgroundImage(_In_ IAdaptiveBackgroundImage* value);
        virtual HRESULT STDMETHODCALLTYPE put_RootElement(_In_ IFrameworkElement* value);
        virtual HRESULT STDMETHODCALLTYPE put_isRootElementSizeChanged(_In_ boolean value);
        virtual HRESULT STDMETHODCALLTYPE get_ResolvedImage(_In_ IUIElement** value);
        virtual HRESULT STDMETHODCALLTYPE put_ImageSize(_In_ ABI::Windows::Foundation::Size value);

        virtual HRESULT STDMETHODCALLTYPE LoadImageBrush(_In_ IUIElement* image);

        // IFrameworkElementOverrides overrides
        virtual HRESULT STDMETHODCALLTYPE OnApplyTemplate();
        virtual HRESULT STDMETHODCALLTYPE MeasureOverride(_In_ Size availableSize, _Out_ Size* pReturnValue);
        virtual HRESULT STDMETHODCALLTYPE ArrangeOverride(_In_ Size arrangeBounds, _Out_ Size* pReturnValue);

        // not implemented
        virtual HRESULT STDMETHODCALLTYPE get_BackgroundImage(_In_ IAdaptiveBackgroundImage** value) { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE get_RootElement(_In_ IFrameworkElement** value) { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE get_isRootElementSizeChanged(_In_ boolean* value) { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE put_ResolvedImage(_In_ IUIElement* value) { return E_NOTIMPL; }
        virtual HRESULT STDMETHODCALLTYPE get_ImageSize(_In_ ABI::Windows::Foundation::Size* value) { return E_NOTIMPL; }

        // ITypePeek method
        void* PeekAt(REFIID riid) override { return PeekHelper(riid, this); }

        void RefreshContainerTile();//DOUBLE actualWidth, DOUBLE actualHeight);

    private:
        // Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::ISizeChangedEventHandler> RootElement_SizeChanged();

        // Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncOperation<BOOL>>
        //BOOL LoadImageBrushAsync();

        //void RefreshImageSize();

        // Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncAction>
        //void RefreshContainerTileLockedAsync();
        BOOL RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight);
        HRESULT ExtractBackgroundImageData(ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                           ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                           ABI::AdaptiveNamespace::VerticalAlignment* vAlignment);

        // Fields
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::IFrameworkElement> m_rootElement{};
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::IUIElement> m_resolvedImage;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Controls::ICanvas> m_containerElement{};
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Media::ITranslateTransform> m_containerTranslate{};
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Media::IImageBrush> m_brushXaml{};

        Microsoft::WRL::ComPtr<ABI::AdaptiveNamespace::IAdaptiveBackgroundImage> m_adaptiveBackgroundImage{};
        ABI::Windows::Foundation::Size m_imageSize{};
        ABI::Windows::Foundation::Size m_containerSize{};

        // Microsoft::WRL::Wrappers::Semaphore m_flag;
        std::vector<Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Shapes::IRectangle>> m_xamlChildren{};
        BOOL m_isImageSourceLoaded = FALSE;
        BOOL m_isRootElementSizeChanged = FALSE;
    };
    ActivatableClass(TileControl);
}
