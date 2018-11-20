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
                                              ABI::Windows::UI::Xaml::Controls::IContentControl,
                                              ABI::Windows::UI::Xaml::IFrameworkElement>
    {
        AdaptiveRuntimeStringClass(TileControl);

    public:
        // public:
        // TileControl();

        // Uri ImageSource();

        /*HRESULT XamlTileBackground(IAdaptiveRenderContext* renderContext,
                                   ABI::AdaptiveNamespace::IAdaptiveBackgroundImage* backgroundImage,
                                   ABI::Windows::UI::Xaml::IFrameworkElement** backgroundAsFrameworkElement);

        HRESULT BuildImageBrush(IAdaptiveRenderContext* renderContext,
                                ABI::AdaptiveNamespace::IAdaptiveBackgroundImage* backgroundImage,
                                IImageBrush* brushXaml,
                                INT32* imageWidth,
                                INT32* imageHeight);*/
    protected:
        // override async
        void OnApplyTemplate() const;

    private:
        // async
        // static void OnAlignmentChange(DependencyObject d, DependencyPropertyChangedEventArgs e);
        // static void OnImageSourceChanged(DependencyObject d, DependencyPropertyChangedEventArgs e);
        // Task<bool> LoadImageBrush(Uri uri);
        // HRESULT LoadImageBrush(Uri** uri);
        // void RootElement_SizeChanged(object sender, SizeChangedEventArgs e);
        // IAsyncAction RefreshContainerTileLockedAsync();
        //ABI::Windows::Foundation::IAsyncAction RootElement_SizeChanged();
        void RefreshContainerTile();
        BOOL RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight, IAdaptiveBackgroundImage** backgroundImage);
        HRESULT ExtractBackgroundImageData(ABI::AdaptiveNamespace::IAdaptiveBackgroundImage** backgroundImage,
                                           ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                           ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                           ABI::AdaptiveNamespace::VerticalAlignment* vAlignment);

        // void RefreshImageSize(double width, double height);

        // Fields
        Microsoft::WRL::ComPtr<IFrameworkElement> m_rootElement;
        Microsoft::WRL::ComPtr<ICanvas> m_containerElement;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Media::ITranslateTransform> m_containerTranslate;
        Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Media::IImageBrush> m_brushXaml;
        Microsoft::WRL::ComPtr<ABI::Windows::Foundation::Size> m_imageSize;

        std::vector<Microsoft::WRL::ComPtr<ABI::Windows::UI::Xaml::Shapes::IRectangle>> m_xamlChildren;
        BOOL m_isImageSourceLoaded;
        BOOL m_isRootElementSizeChanged;
        INT32 count;
    };

    // ActivatableClass(TileControl);
}
