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
        // if (m_isImageSourceLoaded == TRUE)
        //{
        //    for (INT i = 0; i < m_xamlChildren.size(); i++)
        //    {
        //        // Convert xamlChild to shape to set fill to null
        //        ComPtr<IShape> childAsShape;
        //        m_xamlChildren[i].As(&childAsShape);
        //        childAsShape->put_Fill(NULL);
        //    }
        //}

        // m_isImageSourceLoaded = FALSE;
        // if (m_resolvedImage != nullptr)
        //{
        //    // Remove imageLoaded hook
        //}

        m_resolvedImage = uielement;

        ComPtr<IImage> image;
        m_resolvedImage.As(&image);

        if (image != nullptr)
        {
            EventRegistrationToken eventToken;
            image->add_ImageOpened(Callback<IRoutedEventHandler>([&](IInspectable* /*sender*/, IRoutedEventArgs *
                                                                     /*args*/) -> HRESULT {
                                       ComPtr<IUIElement> image;
                                       this->get_ResolvedImage(&image);

                                       Size imageSize;
                                       image->get_DesiredSize(&imageSize);
                                       this->put_ImageSize(imageSize);

                                       this->RefreshContainerTile();
                                       return S_OK;
                                   })
                                       .Get(),
                                   &eventToken);
        }

        ComPtr<IImageSource> imageSource;
        image->get_Source(&imageSource);
        m_brushXaml->put_ImageSource(imageSource.Get());

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

        // Set m_containerTranslate
        m_containerTranslate = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ITranslateTransform>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Media_TranslateTransform));

        // Set m_containerElement.RenderTransform
        ComPtr<IUIElement> containerElementAsUIElement;
        m_containerElement.As(&containerElementAsUIElement);
        ComPtr<ITransform> containerTranslateAsTransform;
        m_containerTranslate.As(&containerTranslateAsTransform);
        containerElementAsUIElement->put_RenderTransform(containerTranslateAsTransform.Get());

        // TODO TRANSLATE: await
        // LoadImageBrushAsync();
        //RefreshContainerTile();
        //}

        ComPtr<IFrameworkElementOverrides> base;
        RETURN_IF_FAILED(GetComposableBase()->QueryInterface(__uuidof(IFrameworkElementOverrides),
                                                             reinterpret_cast<void**>(base.GetAddressOf())));
        return base->OnApplyTemplate();
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

        // RefreshContainerTile(pReturnValue->Width, pReturnValue->Height, m_imageSize.Width, m_imageSize.Height);
        // RefreshContainerTile(pReturnValue->Width, pReturnValue->Height);
        m_containerSize = *pReturnValue;
        RefreshContainerTile();

        return hr2;
    }

    //    void TileControl::RefreshImageSize()
    //    {
    //        DOUBLE imageWidth;
    //        DOUBLE imageHeight;
    //        ComPtr<IFrameworkElement> frameworkElement;
    //        m_resolvedImage.As(&frameworkElement);
    //
    //        frameworkElement->get_Height(&imageHeight);
    //        m_imageSize.Height = imageHeight;
    //
    //        frameworkElement->get_Width(&imageWidth);
    //        m_imageSize.Width = imageWidth;
    //
    //        /*INT32 imageWidth, imageHeight;
    //        ComPtr<IBitmapImage> image;
    //        m_resolvedImage.As(&image);
    //
    //        image->get_DecodePixelHeight(&imageHeight);
    //        m_imageSize.Height = imageHeight;
    //
    //        image->get_DecodePixelWidth(&imageWidth);
    //        m_imageSize.Width = imageWidth;
    //*/
    //    }

    // void TileControl::RefreshContainerTileLockedAsync()
    //{
    //    // TODO TRANSLATE: await _flag.WaitAsync();
    //    RefreshContainerTile();
    //    // TODO TRANSLATE: _flag.Release();
    //}

    void TileControl::RefreshContainerTile()//DOUBLE actualWidth, DOUBLE actualHeight)
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

            //RefreshContainerTile(actualWidth, actualHeight, m_imageSize.Width, m_imageSize.Height);
            RefreshContainerTile(m_containerSize.Width, m_containerSize.Height, m_imageSize.Width, m_imageSize.Height);
        }
    }

    BOOL TileControl::RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight)
    {
        ABI::AdaptiveNamespace::BackgroundImageMode mode;
        ABI::AdaptiveNamespace::HorizontalAlignment hAlignment;
        ABI::AdaptiveNamespace::VerticalAlignment vAlignment;
        ExtractBackgroundImageData(&mode, &hAlignment, &vAlignment);

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
            numberImagePerColumn = (INT)ceil(width / imageWidth) + 1;

            switch (vAlignment)
            {
            case ABI::AdaptiveNamespace::VerticalAlignment::Bottom:
                numberImagePerRow = 1;
                offsetHorizontalAlignment = (INT)(height - imageHeight);
                break;
            case ABI::AdaptiveNamespace::VerticalAlignment::Center:
                numberImagePerRow = (INT)ceil(height / imageHeight);
                break;
            case ABI::AdaptiveNamespace::VerticalAlignment::Top:
            default:
                numberImagePerRow = 1;
                break;
            }
            break;

        case ABI::AdaptiveNamespace::BackgroundImageMode::RepeatVertically:
            numberImagePerRow = (INT)ceil(height / imageHeight) + 1;

            switch (hAlignment)
            {
            case ABI::AdaptiveNamespace::HorizontalAlignment::Right:
                numberImagePerColumn = 1;
                offsetVerticalAlignment = (INT)(width - imageWidth);
                break;
            case ABI::AdaptiveNamespace::HorizontalAlignment::Center:
                numberImagePerColumn = (INT)ceil(width / imageWidth);
                break;
            case ABI::AdaptiveNamespace::HorizontalAlignment::Left:
            default:
                numberImagePerColumn = 1;
                break;
            }
            break;

        case ABI::AdaptiveNamespace::BackgroundImageMode::Repeat:
            numberImagePerRow = (INT)ceil(height / imageHeight) + 1;
            numberImagePerColumn = (INT)ceil(width / imageWidth) + 1;
            break;
        }
        numberSpriteToInstanciate = numberImagePerColumn * numberImagePerRow;

        INT count = (INT)(m_xamlChildren.size());

        // Get containerElement.Children
        ComPtr<IVector<UIElement*>> children;
        ComPtr<IPanel> containerElementAsPanel;
        m_containerElement.As(&containerElementAsPanel);
        containerElementAsPanel->get_Children(&children);

        // instanciate all elements not created yet
        for (INT x = 0; x < numberSpriteToInstanciate - count; x++)
        {
            ComPtr<IRectangle> rectangle = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IRectangle>(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Shapes_Rectangle));

            ComPtr<IUIElement> rectangleAsUIElement;
            rectangle.As(&rectangleAsUIElement);
            children->Append(rectangleAsUIElement.Get());

            m_xamlChildren.push_back(rectangle);
        }

        // remove elements not used now
        for (INT x = 0; x < count - numberSpriteToInstanciate; x++)
        {
            children->RemoveAtEnd();
            m_xamlChildren.pop_back();
        }

        // Convert ImageBrush to Brush
        ComPtr<IBrush> brushXamlAsBrush;
        m_brushXaml.As(&brushXamlAsBrush);

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
                rectangle.As(&rectangleAsShape);
                rectangleAsShape->put_Fill(brushXamlAsBrush.Get());

                // Convert rectangle to UIElement
                ComPtr<IUIElement> rectangleAsUIElement;
                rectangleAsShape.As(&rectangleAsUIElement);

                // Set Left and Top for rectangle
                ComPtr<ICanvasStatics> canvasStatics;
                ABI::Windows::Foundation::GetActivationFactory(
                    HStringReference(RuntimeClass_Windows_UI_Xaml_Controls_Canvas).Get(), &canvasStatics);
                canvasStatics->SetLeft(rectangleAsUIElement.Get(), (x * imageWidth) + offsetVerticalAlignment);
                canvasStatics->SetTop(rectangleAsUIElement.Get(), (y * imageHeight) + offsetHorizontalAlignment);

                // Set Width and Height for Rectangle
                rectangle->put_RadiusX(imageWidth);
                rectangle->put_RadiusY(imageHeight);
            }
        }
        return TRUE;
    }

    HRESULT TileControl::ExtractBackgroundImageData(ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                                    ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                                    ABI::AdaptiveNamespace::VerticalAlignment* vAlignment)
    {
        m_adaptiveBackgroundImage->get_Mode(mode);
        m_adaptiveBackgroundImage->get_HorizontalAlignment(hAlignment);
        m_adaptiveBackgroundImage->get_VerticalAlignment(vAlignment);
        return S_OK;
    }
}
