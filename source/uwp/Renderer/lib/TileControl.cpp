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

namespace AdaptiveNamespace
{
    HRESULT TileControl::RuntimeClassInitialize() noexcept try
    {
        ComPtr<IContentControlFactory> spFactory;
        RETURN_IF_FAILED(Windows::Foundation::GetActivationFactory(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_ContentControl).Get(), &spFactory));

        // Create composable base
        ComPtr<IInspectable> spInnerInspectable;
        ComPtr<IContentControl> spInnerContentControl;
        RETURN_IF_FAILED(spFactory->CreateInstance(static_cast<ITileControl*>(this),
                                                   spInnerInspectable.GetAddressOf(),
                                                   spInnerContentControl.GetAddressOf()));
        RETURN_IF_FAILED(SetComposableBasePointers(spInnerInspectable.Get(), spFactory.Get()));

        // initialize members
        m_containerElement = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ICanvas>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Canvas));
        m_brushXaml = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IImageBrush>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Media_ImageBrush));
        // m_xamlChildren = std::vector<ComPtr<IRectangle>>(10);

        // Add m_containerElement to content of ContentControl
        ComPtr<IInspectable> content;
        m_containerElement.As(&content);
        spInnerContentControl->put_Content(content.Get());

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
            ComPtr<IImageSource> imageSource;
            THROW_IF_FAILED(image->get_Source(&imageSource));

            EventRegistrationToken eventToken;
            ComPtr<IBitmapImage> bitmapImage;
            THROW_IF_FAILED(imageSource.As(&bitmapImage));
            THROW_IF_FAILED(bitmapImage->add_ImageOpened(Callback<IRoutedEventHandler>(
                                                             [&](IInspectable* /*sender*/, IRoutedEventArgs * /*args*/) -> HRESULT {
                                                                 this->m_isImageSourceLoaded = TRUE;
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

                                                                 // Save size to member variable
                                                                 Size imageSize;
                                                                 imageSize.Height = height;
                                                                 imageSize.Width = width;
                                                                 this->put_ImageSize(imageSize);

                                                                 this->RefreshContainerTile();
                                                                 return S_OK;
                                                             })
                                                             .Get(),
                                                         &eventToken));

            THROW_IF_FAILED(image->get_Source(&imageSource));
            THROW_IF_FAILED(m_brushXaml->put_ImageSource(imageSource.Get()));
            // m_isImageSourceLoaded = TRUE;

            return S_OK;
        }
        return E_INVALIDARG;
    }

    HRESULT TileControl::OnApplyTemplate()
    {
        // Call OnApplyTemplate() for composable base
        ComPtr<IFrameworkElementOverrides> base;
        RETURN_IF_FAILED(GetComposableBase()->QueryInterface(__uuidof(IFrameworkElementOverrides),
                                                             reinterpret_cast<void**>(base.GetAddressOf())));
        HRESULT hr2 = base->OnApplyTemplate();

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

        // Define clip properties for m_containerElement
        Rect* rect = new Rect();
        rect->X = 0;
        rect->Y = 0;
        rect->Width = m_containerSize.Width;
        rect->Height = m_containerSize.Height;

        ComPtr<IRectangleGeometry> clip = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IRectangleGeometry>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Media_RectangleGeometry));
        clip->put_Rect(*rect);

        ComPtr<IUIElement> containerAsUIElement;
        m_containerElement.As(&containerAsUIElement);
        containerAsUIElement->put_Clip(clip.Get());

        RefreshContainerTile();
        return hr2;
    }

    void TileControl::RefreshContainerTile()
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
            RefreshContainerTile(m_containerSize.Width, m_containerSize.Height, m_imageSize.Width, m_imageSize.Height);
        }
    }

    void TileControl::RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight)
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

        /*if (m_isImageSourceLoaded == FALSE)
        {
            return;
        }*/

        DOUBLE numberSpriteToInstanciate = 0;
        INT numberImagePerColumn = 1;
        INT numberImagePerRow = 1;

        FLOAT offsetVerticalAlignment = 0;
        FLOAT offsetHorizontalAlignment = 0;

        switch (mode)
        {
        case ABI::AdaptiveNamespace::BackgroundImageMode::RepeatHorizontally:
            numberImagePerRow = (INT)ceil(width / imageWidth);
            numberImagePerColumn = 1;

            switch (vAlignment)
            {
            case ABI::AdaptiveNamespace::VerticalAlignment::Bottom:
                offsetVerticalAlignment = height - imageHeight;
                break;
            case ABI::AdaptiveNamespace::VerticalAlignment::Center:
                offsetVerticalAlignment = (height - imageHeight) / 2;
                break;
            case ABI::AdaptiveNamespace::VerticalAlignment::Top:
            default:
                break;
            }
            break;

        case ABI::AdaptiveNamespace::BackgroundImageMode::RepeatVertically:
            numberImagePerRow = 1;
            numberImagePerColumn = (INT)ceil(height / imageHeight);

            switch (hAlignment)
            {
            case ABI::AdaptiveNamespace::HorizontalAlignment::Right:
                offsetHorizontalAlignment = width - imageWidth;
                break;
            case ABI::AdaptiveNamespace::HorizontalAlignment::Center:
                offsetHorizontalAlignment = (width - imageWidth) / 2;
                break;
            case ABI::AdaptiveNamespace::HorizontalAlignment::Left:
            default:
                break;
            }
            break;

        case ABI::AdaptiveNamespace::BackgroundImageMode::Repeat:
            numberImagePerColumn = (INT)ceil(height / imageHeight);
            numberImagePerRow = (INT)ceil(width / imageWidth);
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
        THROW_IF_FAILED(m_brushXaml.As(&brushXamlAsBrush));

        // Change positions+brush for all actives elements
        for (INT x = 0; x < numberImagePerRow; x++)
        {
            for (INT y = 0; y < numberImagePerColumn; y++)
            {
                INT index = (y * numberImagePerRow) + x;

                // Get Rectangle
                auto rectangle = m_xamlChildren[index];

                // Set rectangle.Fill
                ComPtr<IShape> rectangleAsShape;
                THROW_IF_FAILED(rectangle.As(&rectangleAsShape));
                THROW_IF_FAILED(rectangleAsShape->put_Fill(brushXamlAsBrush.Get()));

                // Convert rectangle to UIElement
                ComPtr<IUIElement> rectangleAsUIElement;
                THROW_IF_FAILED(rectangleAsShape.As(&rectangleAsUIElement));

                // Set Left and Top for rectangle
                ComPtr<ICanvasStatics> canvasStatics;
                ABI::Windows::Foundation::GetActivationFactory(
                    HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Canvas).Get(), &canvasStatics);
                THROW_IF_FAILED(canvasStatics->SetLeft(rectangleAsUIElement.Get(), (x * imageWidth) + offsetHorizontalAlignment));
                THROW_IF_FAILED(canvasStatics->SetTop(rectangleAsUIElement.Get(), (y * imageHeight) + offsetVerticalAlignment));

                ComPtr<IFrameworkElement> rectangleAsFElement;
                rectangle.As(&rectangleAsFElement);
                // Set Width and Height for Rectangle
                THROW_IF_FAILED(rectangleAsFElement->put_Width(imageWidth));
                THROW_IF_FAILED(rectangleAsFElement->put_Height(imageHeight));
            }
        }
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
