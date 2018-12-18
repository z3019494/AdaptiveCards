#include "pch.h"

#include "enums.h"
#include <math.h>
#include "AdaptiveBackgroundImage.h"
#include "TileControl.h"
#include "XamlHelpers.h"
#include "XamlBuilder.h"
#include <windows.foundation.collections.h>
#include <windows.storage.h>
#include <windows.system.threading.h>
#include <windows.ui.xaml.h>
#include <windows.ui.xaml.controls.h>
#include <windows.ui.xaml.shapes.h>
#include <windows.ui.xaml.hosting.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Numerics;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Composition;

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
using namespace ABI::Windows::System;

//temp
using namespace ABI::Windows::UI;

namespace AdaptiveNamespace
{
    HRESULT TileControl::RuntimeClassInitialize() noexcept try
    {
        // Create composable base
        ComPtr<IContentControlFactory> spFactory;
        ComPtr<IInspectable> spInnerInspectable;
        ComPtr<IContentControl> spInnerContentControl;
        RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_ContentControl).Get(), &spFactory));
        RETURN_IF_FAILED(spFactory->CreateInstance(static_cast<ITileControl*>(this),
                                                   spInnerInspectable.GetAddressOf(),
                                                   spInnerContentControl.GetAddressOf()));
        RETURN_IF_FAILED(SetComposableBasePointers(spInnerInspectable.Get(), spFactory.Get()));

        // initialize members
        m_containerElement = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ICanvas>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Canvas));
        m_brushXaml = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IImageBrush>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Media_ImageBrush));

        // Add containerElement to content of ContentControl
        ComPtr<IInspectable> content;
        m_containerElement.As(&content);
        spInnerContentControl->put_Content(content.Get());
        //this->put_Content(content.Get());

        return S_OK;
    }
    CATCH_RETURN;

    _Use_decl_annotations_ HRESULT TileControl::put_BackgroundImage(_In_ IAdaptiveBackgroundImage* value)
    {
        m_adaptiveBackgroundImage = value;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT TileControl::put_RootElement(_In_ IFrameworkElement* value)
    {
        m_rootElement = value;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT TileControl::put_isRootElementSizeChanged(_In_ boolean value)
    {
        m_isRootElementSizeChanged = value;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT TileControl::get_ResolvedImage(_In_ IUIElement** value)
    {
        return m_resolvedImage.CopyTo(value);
    }

    _Use_decl_annotations_ HRESULT TileControl::put_ImageSize(_In_ ABI::Windows::Foundation::Size value)
    {
        m_imageSize = value;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT TileControl::LoadImageBrush(_In_ IUIElement* uielement)
    {
        m_resolvedImage = uielement;

        ComPtr<IImage> image;
        m_resolvedImage.As(&image);

        if (image != nullptr)
        {
            EventRegistrationToken eventToken;
            ComPtr<IImageSource> imageSource;
            image->get_Source(&imageSource);
            ComPtr<IBitmapImage> bitmapImage;
            imageSource.As(&bitmapImage);
            bitmapImage->add_ImageOpened(Callback<IRoutedEventHandler>([&](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                           /*args*/) -> HRESULT {
                                             ComPtr<IUIElement> uiElement;
                                             this->get_ResolvedImage(&uiElement);

                                             // Extract BitmapSource from Image
                                             ComPtr<IImage> image;
                                             uiElement.As(&image);
                                             ComPtr<IImageSource> imageSource;
                                             image->get_Source(&imageSource);
                                             ComPtr<IBitmapSource> bitmapSource;
                                             imageSource.As(&bitmapSource);

                                             // Extract Size from Image
                                             INT32 height, width;
                                             bitmapSource->get_PixelHeight(&height);
                                             bitmapSource->get_PixelWidth(&width);

                                             // Save dimensions to member
                                             Size imageSize;
                                             imageSize.Height = height;
                                             imageSize.Width = width;
                                             this->put_ImageSize(imageSize);

                                             this->RefreshContainerTile();
                                             return S_OK;
                                         })
                                             .Get(),
                                         &eventToken);
        }

        ComPtr<IImageSource> imageSource;
        THROW_IF_FAILED(image->get_Source(&imageSource));
        THROW_IF_FAILED(m_brushXaml->put_ImageSource(imageSource.Get()));

        m_isImageSourceLoaded = TRUE;

        // RefreshContainerTile();

        return S_OK;
    }

    HRESULT TileControl::OnApplyTemplate()
    {
        // ComPtr<IFrameworkElement> rootElement = m_rootElement.Get();
        /*if (rootElement != NULL)
        {
            rootElement->remove_SizeChanged(m_eventToken);
        }*/

        // TRANSLATE: rootElement = GetTemplateChild("RootElement") as FrameworkElement;

        // m_rootElement = rootElement;

        /*if (rootElement != NULL)
        {*/
        // TODO: FIX ASYNC OF THIS EVENT HANDLING
        // ISizeChangedEventHandler* eventHandler;
        // rootElement->add_SizeChanged(eventHandler, &eventToken);
        /*
        TODO: UNCOMMENT
        auto sizeChangedCallback = Callback<Implements<RuntimeClassFlags<WinRtClassicComMix>, ISizeChangedEventHandler>>(
            [&](IInspectable* pSender, ISizeChangedEventArgs* pArgs) -> HRESULT {
                this->put_isRootElementSizeChanged(FALSE);
                this->RefreshContainerTileLockedAsync();
                return S_OK;
            });
        m_rootElement->add_SizeChanged(sizeChangedCallback.Get(), &m_eventToken);*/

        // Set m_containerElement
        /*HSTRING name;
        UTF8ToHString("ContainerElement", &name);
        ComPtr<IInspectable> containerElementAsInspectable;
        m_rootElement->FindName(name, &containerElementAsInspectable);
        containerElementAsInspectable.As(&m_containerElement);*/

        ComPtr<IFrameworkElementOverrides> base;
        RETURN_IF_FAILED(GetComposableBase()->QueryInterface(__uuidof(IFrameworkElementOverrides),
                                                             reinterpret_cast<void**>(base.GetAddressOf())));
        HRESULT hr2 = base->OnApplyTemplate();

        // Set m_containerTranslate
        m_containerTranslate = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ITranslateTransform>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Media_TranslateTransform));

        // Set m_containerElement.RenderTransform
        ComPtr<IUIElement> containerElementAsUIElement;
        RETURN_IF_FAILED(m_containerElement.As(&containerElementAsUIElement));
        ComPtr<ITransform> containerTranslateAsTransform;
        RETURN_IF_FAILED(m_containerTranslate.As(&containerTranslateAsTransform));
        RETURN_IF_FAILED(containerElementAsUIElement->put_RenderTransform(containerTranslateAsTransform.Get()));

        RefreshContainerTile();

        return hr2;
    }

    HRESULT TileControl::MeasureOverride(Size availableSize, Size* pReturnValue)
    {
        ComPtr<IFrameworkElementOverrides> base;
        RETURN_IF_FAILED(GetComposableBase()->QueryInterface(__uuidof(IFrameworkElementOverrides),
                                                             reinterpret_cast<void**>(base.GetAddressOf())));

        return base->MeasureOverride(availableSize, pReturnValue);
    }

    HRESULT TileControl::ArrangeOverride(Size arrangeBounds, Size* pReturnValue)
    {
        ComPtr<IFrameworkElementOverrides> base;
        RETURN_IF_FAILED(GetComposableBase()->QueryInterface(__uuidof(IFrameworkElementOverrides),
                                                             reinterpret_cast<void**>(base.GetAddressOf())));

        HRESULT hr2 = base->ArrangeOverride(arrangeBounds, pReturnValue);
        RETURN_IF_FAILED(hr2);

        m_containerSize = *pReturnValue;
        RefreshContainerTile();
        return hr2;
    }

    // void TileControl::RefreshContainerTileLockedAsync()
    //{
    //    // TODO TRANSLATE: await _flag.WaitAsync();
    //    RefreshContainerTile();
    //    // TODO TRANSLATE: _flag.Release();
    //}

    void TileControl::RefreshContainerTile() // DOUBLE actualWidth, DOUBLE actualHeight)
    {
        Size emptySize;
        ComPtr<ISizeHelperStatics> sizeStatics;
        ABI::Windows::Foundation::GetActivationFactory(HStringReference(RuntimeClass_Windows_UI_Xaml_SizeHelper).Get(), &sizeStatics);
        sizeStatics->get_Empty(&emptySize);

        BOOL imageSizeIsEmpty = (m_imageSize.Height == emptySize.Height && m_imageSize.Width == emptySize.Width);
        if (imageSizeIsEmpty || m_rootElement == NULL)
        {
            return;
        }
        else
        {
            /*DOUBLE actualWidth;
            m_rootElement->get_ActualWidth(&actualWidth);

            DOUBLE actualHeight;
            m_rootElement->get_ActualWidth(&actualHeight);*/

            // RefreshContainerTile(actualWidth, actualHeight, m_imageSize.Width, m_imageSize.Height);
            RefreshContainerTile(m_containerSize.Width, m_containerSize.Height, m_imageSize.Width, m_imageSize.Height);
        }
    }

    BOOL TileControl::RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight)
    {
        ABI::AdaptiveNamespace::BackgroundImageMode mode;
        ABI::AdaptiveNamespace::HorizontalAlignment hAlignment;
        ABI::AdaptiveNamespace::VerticalAlignment vAlignment;
        THROW_IF_FAILED(ExtractBackgroundImageData(&mode, &hAlignment, &vAlignment));
        if (imageWidth == 0)
        {
            imageWidth = width;
        }
        if (imageHeight == 0)
        {
            imageHeight = height;
        }

        /*if (m_isImageSourceLoaded == FALSE || m_isRootElementSizeChanged == FALSE)
        {
            return FALSE;
        }*/

        DOUBLE numberSpriteToInstanciate = 0;
        INT numberImagePerColumn = 1;
        INT numberImagePerRow = 1;

        FLOAT offsetVerticalAlignment = 0;
        FLOAT offsetHorizontalAlignment = 0;

        switch (mode)
        {
        case ABI::AdaptiveNamespace::BackgroundImageMode::RepeatHorizontally:
            // og: numberImagePerColumn = (INT)ceil(width / imageWidth) + 1;
            // me...
            numberImagePerRow = (INT)ceil(width / imageWidth);
            numberImagePerColumn = 1;
            // end me

            switch (vAlignment)
            {
            case ABI::AdaptiveNamespace::VerticalAlignment::Bottom:
                // og: numberImagePerRow = 1;
                // og: offsetHorizontalAlignment = (INT)(height - imageHeight);
                offsetVerticalAlignment = height - imageHeight;
                break;
            case ABI::AdaptiveNamespace::VerticalAlignment::Center:
                // og: numberImagePerRow = (INT)ceil(height / imageHeight);
                offsetVerticalAlignment = (height - imageHeight) / 2;
                break;
            case ABI::AdaptiveNamespace::VerticalAlignment::Top:
            default:
                // numberImagePerRow = 1;
                break;
            }
            break;

        case ABI::AdaptiveNamespace::BackgroundImageMode::RepeatVertically:
            // og: numberImagePerRow = (INT)ceil(height / imageHeight) + 1;
            // me...
            numberImagePerRow = 1;
            numberImagePerColumn = (INT)ceil(height / imageHeight);
            // end me

            switch (hAlignment)
            {
            case ABI::AdaptiveNamespace::HorizontalAlignment::Right:
                // og: numberImagePerColumn = 1;
                // og: offsetVerticalAlignment = (INT)(width - imageWidth);
                offsetHorizontalAlignment = width - imageWidth;
                break;
            case ABI::AdaptiveNamespace::HorizontalAlignment::Center:
                // og: numberImagePerColumn = (INT)ceil(width / imageWidth);
                offsetHorizontalAlignment = (width - imageWidth) / 2;
                break;
            case ABI::AdaptiveNamespace::HorizontalAlignment::Left:
            default:
                // numberImagePerColumn = 1;
                break;
            }
            break;

        case ABI::AdaptiveNamespace::BackgroundImageMode::Repeat:
            // numberImagePerRow = (INT)ceil(height / imageHeight) + 1;
            // numberImagePerColumn = (INT)ceil(width / imageWidth) + 1;
            numberImagePerColumn = (INT)ceil(height / imageHeight) + 1;
            numberImagePerRow = (INT)ceil(width / imageWidth) + 1;
            break;
        }
        numberSpriteToInstanciate = numberImagePerColumn * numberImagePerRow;

        INT count = (INT)(m_xamlChildren.size());

        // Get containerElement.Children
        ComPtr<IVector<UIElement*>> children;
        ComPtr<IPanel> containerElementAsPanel;
        THROW_IF_FAILED(m_containerElement.As(&containerElementAsPanel));
        THROW_IF_FAILED(containerElementAsPanel->get_Children(&children));

        // instanciate all elements not created yet
        for (INT x = 0; x < numberSpriteToInstanciate - count; x++)
        {
            ComPtr<IRectangle> rectangle = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IRectangle>(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Shapes_Rectangle));

            ComPtr<IUIElement> rectangleAsUIElement;
            THROW_IF_FAILED(rectangle.As(&rectangleAsUIElement));
            THROW_IF_FAILED(children->Append(rectangleAsUIElement.Get()));

            m_xamlChildren.push_back(rectangle);
        }

        // remove elements not used now
        for (INT x = 0; x < count - numberSpriteToInstanciate; x++)
        {
            THROW_IF_FAILED(children->RemoveAtEnd());
            m_xamlChildren.pop_back();
        }

        // Convert ImageBrush to Brush
        ComPtr<IBrush> brushXamlAsBrush;
        // THROW_IF_FAILED(m_brushXaml.As(&brushXamlAsBrush));
        ComPtr<ISolidColorBrushFactory> factory;
        ComPtr<ISolidColorBrush> colorBrush;
        ComPtr<IColorHelperStatics> statics;

        ABI::Windows::UI::Color color = {};
        BYTE a, r, g, b;
        a = 0xff;
        r = 0x1e;
        g = 0x99;
        b = 0x11;

        ABI::Windows::Foundation::GetActivationFactory(HStringReference(L"Windows.UI.ColorHelper").Get(), &statics);
        statics->FromArgb(a,r,g,b,&color);

        ABI::Windows::Foundation::GetActivationFactory(HStringReference(L"Windows.UI.Xaml.Media.SolidColorBrush").Get(), &factory);
        factory->CreateInstanceWithColor(color, &colorBrush);
        ComPtr<IBrush> brush;
        colorBrush.As(&brush);

        // Change positions+brush for all actives elements
        for (INT y = 0; y < numberImagePerRow; y++)
        {
            for (INT x = 0; x < numberImagePerColumn; x++)
            {
                INT index = (y * numberImagePerColumn) + x;

                // Get Rectangle
                auto rectangle = m_xamlChildren[index];

                // Set rectangle.Fill
                ComPtr<IShape> rectangleAsShape;
                THROW_IF_FAILED(rectangle.As(&rectangleAsShape));
                // THROW_IF_FAILED(rectangleAsShape->put_Fill(brushXamlAsBrush.Get()));
                rectangleAsShape->put_Fill(brush.Get());

                // Convert rectangle to UIElement
                ComPtr<IUIElement> rectangleAsUIElement;
                THROW_IF_FAILED(rectangleAsShape.As(&rectangleAsUIElement));

                // Set Left and Top for rectangle
                ComPtr<ICanvasStatics> canvasStatics;
                ABI::Windows::Foundation::GetActivationFactory(
                    HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Canvas).Get(), &canvasStatics);
                THROW_IF_FAILED(canvasStatics->SetLeft(rectangleAsUIElement.Get(), (x * imageWidth) + offsetVerticalAlignment));
                THROW_IF_FAILED(canvasStatics->SetTop(rectangleAsUIElement.Get(), (y * imageHeight) + offsetHorizontalAlignment));

                // Set Width and Height for Rectangle
                THROW_IF_FAILED(rectangle->put_RadiusX(imageWidth));
                THROW_IF_FAILED(rectangle->put_RadiusY(imageHeight));
            }
        }
        return TRUE;
    }

    HRESULT TileControl::ExtractBackgroundImageData(ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                                    ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                                    ABI::AdaptiveNamespace::VerticalAlignment* vAlignment)
    {
        THROW_IF_FAILED(m_adaptiveBackgroundImage->get_Mode(mode));
        THROW_IF_FAILED(m_adaptiveBackgroundImage->get_HorizontalAlignment(hAlignment));
        THROW_IF_FAILED(m_adaptiveBackgroundImage->get_VerticalAlignment(vAlignment));
        return S_OK;
    }
}
