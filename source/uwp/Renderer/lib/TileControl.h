#pragma once

#include "pch.h"
#include <windows.ui.xaml.shapes.h>
//#include "AdaptiveCards.Rendering.Uwp.h"
//#include "Enums.h"

//#ifdef ADAPTIVE_CARDS_WINDOWS
// using namespace InternalNamespace;
//#endif

using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Xaml;
using namespace ABI::Windows::UI::Xaml::Shapes;
using namespace ABI::Windows::UI::Xaml::Controls;

namespace AdaptiveNamespace
{
    class TileControl
        : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::WinRtClassicComMix>,
                                              // Microsoft::WRL::FtmBase,
                                              // AdaptiveNamespace::IImageLoadTrackerListener,
                                              ABI::Windows::UI::Xaml::Controls::ContentControl,
                                              ABI::Windows::UI::Xaml::FrameworkElement,
                                              ABI::Windows::UI::Xaml::IFrameworkElementOverrides>
    {
        AdaptiveRuntimeStringClass(TileControl);

    public:
        // TileControl();

        // Uri ImageSource();

        // IFrameworkElementOverrides overrides
        IFACEMETHODIMP OnApplyTemplate();

        IFACEMETHODIMP MeasureOverride();
        IFACEMETHODIMP ArrangeOverride();

        IFACEMETHODIMP put_RenderContext(_In_ IAdaptiveRenderContext* value);
        IFACEMETHODIMP put_BackgroundImage(_In_ IAdaptiveBackgroundImage* value);
        IFACEMETHODIMP put_isRootElementSizeChanged(_In_ BOOL value);


    private:
        // Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::ISizeChangedEventHandler> RootElement_SizeChanged();
        // Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncOperation<BOOL>>
        BOOL LoadImageBrushAsync();
        // Microsoft::WRL::ComPtr<ABI::Windows::Foundation::IAsyncAction>
        void RefreshContainerTileLockedAsync();
        void RefreshContainerTile();
        BOOL RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight);
        HRESULT ExtractBackgroundImageData(ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                           ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                           ABI::AdaptiveNamespace::VerticalAlignment* vAlignment);

        // Fields
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::IFrameworkElement> m_rootElement;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Controls::ICanvas> m_containerElement;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Media::ITranslateTransform> m_containerTranslate;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Media::IImageBrush> m_brushXaml;
        Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Size> m_imageSize;

        Microsoft::WRL::ComPtr<ABI::AdaptiveNamespace::IAdaptiveBackgroundImage> m_backgroundImage;
        Microsoft::WRL::ComPtr<ABI::AdaptiveNamespace::IAdaptiveRenderContext> m_renderContext;

        ABI::Windows::Foundation::IUriRuntimeClass* m_uri;
        Microsoft::WRL::Wrappers::Semaphore m_flag;
        std::vector<Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Shapes::IRectangle>> m_xamlChildren;
        BOOL m_isImageSourceLoaded;
        BOOL m_isRootElementSizeChanged;
    };

    // ActivatableClass(TileControl);
}
