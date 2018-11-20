#include "pch.h"

//#include "enums.h"
#include <math.h>
#include "AdaptiveBackgroundImage.h"
#include "TileControl.h"
#include "XamlHelpers.h"
#include "XamlBuilder.h"
#include <windows.foundation.collections.h>
#include <windows.storage.h>
#include <windows.system.threading.h>
#include <windows.ui.xaml.h>
#include <windows.ui.xaml.shapes.h>
#include <windows.ui.xaml.hosting.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation::Numerics;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::UI::Composition;
using namespace ABI::Windows::UI::Xaml::Hosting;

// XAML STUFF
using namespace ABI::Windows::Storage;
using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::UI::Xaml::Shapes;
using namespace ABI::Windows::UI::Xaml::Controls;
using namespace ABI::Windows::UI::Xaml::Media;
using namespace ABI::Windows::UI::Xaml::Media::Imaging;

namespace AdaptiveNamespace
{
    // HRESULT TileControl::XamlTileBackground(IAdaptiveRenderContext* renderContext,
    //                                        ABI::AdaptiveNamespace::IAdaptiveBackgroundImage* backgroundImage,
    //                                        ABI::Windows::UI::Xaml::IFrameworkElement** _rootElement)
    //{
    //    ComPtr<IFrameworkElement> rootElement;
    //    rootElement = _rootElement;

    //    // Create containerElement
    //    ComPtr<ICanvas> containerElement;
    //    rootElement.As(&containerElement);

    //    // Create containerTranslate
    //    ComPtr<ITranslateTransform> containerTranslate = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ITranslateTransform>(
    //        HStringReference(RuntimeClass_Windows_UI_Xaml_Media_TranslateTransform));

    //    // Set containerTranslate to containerElement
    //    ComPtr<IUIElement> containerElementAsUIElement;
    //    containerElement.As(&containerElementAsUIElement);
    //    ComPtr<ITransform> containerTranslateAsTransform;
    //    containerTranslate.As(&containerTranslateAsTransform);
    //    containerElementAsUIElement->put_RenderTransform(containerTranslateAsTransform.Get());

    //    // Convert ImageBrush to Brush
    //    ComPtr<IBrush> brushXamlAsBrush;
    //    brushXaml.As(&brushXamlAsBrush);

    //    // Update positions/brush for all elements
    //    for (int y = 0; y < numberImagePerRow; y++)
    //    {
    //        for (int x = 0; x < numberImagePerColumn; x++)
    //        {
    //            int index = (y * numberImagePerColumn) + x;

    //            // Get Rectangle
    //            ComPtr<IRectangle> rectangle;
    //            xamlChildren->GetAt(index, rectangle.Get());

    //            // Set rectangle.Fill
    //            ComPtr<IShape> rectangleAsShape;
    //            rectangle.As(&rectangleAsShape);
    //            rectangleAsShape->put_Fill(brushXamlAsBrush.Get());

    //            // Convert rectangle to UIElement
    //            ComPtr<IUIElement> rectangleAsUIElement;
    //            rectangleAsShape.As(&rectangleAsUIElement);

    //            // Set Left and Top for rectangle
    //            ComPtr<ICanvasStatics> canvas;
    //            canvas->SetLeft(rectangleAsUIElement.Get(), (x * imageWidth) + offsetVerticalAlignment);
    //            canvas->SetTop(rectangleAsUIElement.Get(), (y * imageHeight) + offsetHorizontalAlignment;

    //            // Set Width and Height for Rectangle
    //            rectangle->put_RadiusX(imageWidth);
    //            rectangle->put_RadiusY(imageHeight);
    //        }
    //    }
    //}

    // HRESULT TileControl::LoadImageBrush(Uri** uri)
    //{
    //    if (uri == NULL)
    //    {
    //        return false;
    //    }

    //    // await _flag.WaitAsync();

    //    if (m_isImageSourceLoaded == true)
    //    {
    //        /*for (int i = 0; i < m_xamlChildren.count; i++)
    //        {
    //            ComPtr<ABI::Windows::UI::Xaml::Shapes::IRectangle> rectangle;
    //            m_xamlChildren->GetAt(i, rectangle);

    //            ComPtr<IShape> rectangleAsShape;
    //            rectangle.As(&rectangleAsShape);

    //            rectangleAsShape->put_Fill(NULL);
    //        }*/
    //    }

    //    m_isImageSourceLoaded = false;

    //    ComPtr<IBitmapImage> image = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IBitmapImage>(
    //        HStringReference(RuntimeClass_Windows_UI_Xaml_Media_Imaging_BitmapImage));

    //    ComPtr<IStorageFile> storageFile;
    //    ComPtr<IStorageFileStatics> StorageFileStatics;
    //    //StorageFileStatics->GetFileFromApplicationUriAsync(uri, storageFile);

    //    // TRANSLATE: using(var stream = await storageFile.OpenReadAsync()) { image.SetSource(stream); }

    //    m_brushXaml = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IImageBrush>(
    //        HStringReference(RuntimeClass_Windows_UI_Xaml_Media_ImageBrush));

    //    // Save image size
    //    INT32* pixelWidth;
    //    INT32* pixelHeight;
    //    image->get_DecodePixelWidth(pixelWidth);
    //    image->get_DecodePixelHeight(pixelHeight);
    //    //m_imageSize = new Size(pixelWidth, pixelHeight);

    //    m_isImageSourceLoaded = true;

    //    RefreshContainerTile();

    //    //RefreshImageSize(_imageSize.Width, _imageSize.Height);

    //    //_flag.Release();
    //}

    // Task<bool> TileControl::LoadImageBrush(Uri uri)/*(IAdaptiveRenderContext* renderContext,
    //                                     ABI::AdaptiveNamespace::IAdaptiveBackgroundImage* backgroundImage,
    //                                     IImageBrush* brushXaml,
    //                                     INT32* imageWidth,
    //                                     INT32* imageHeight)*/
    //{
    //    // Get Host Config
    //    ComPtr<IAdaptiveImage> adaptiveImage;
    //    ComPtr<IAdaptiveHostConfig> hostConfig;
    //    renderContext->get_HostConfig(&hostConfig);

    //    // Get Image URL
    //    HSTRING url;
    //    backgroundImage->get_Url(&url);
    //    ComPtr<IUriRuntimeClass> imageUrl;
    //    GetUrlFromString(hostConfig.Get(), url, imageUrl.GetAddressOf());

    //    if (imageUrl == nullptr)
    //    {
    //        renderContext->AddWarning(ABI::AdaptiveNamespace::WarningStatusCode::AssetLoadFailed,
    //                                  HStringReference(L"Image not found").Get());
    //        return;
    //    }

    //    // Load Image Brush
    //    /*ComPtr<IBitmapImage> image = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IBitmapImage>(
    //        HStringReference(RuntimeClass_Windows_UI_Xaml_Media_Imaging_BitmapImage));
    //    image->put_UriSource();
    //    ComPtr<IBitmapImage> image;*/
    //    // image = _image;

    //    // Get Image size
    //    image->get_DecodePixelWidth(&imageWidth);
    //    image->get_DecodePixelHeight(&imageHeight);
    //}

    // TileControl::TileControl() {}

    void TileControl::OnApplyTemplate()
    {
        ComPtr<IFrameworkElement> rootElement = m_rootElement.Get();
        if (rootElement != NULL)
        {
            EventRegistrationToken eventToken;
            rootElement->remove_SizeChanged(eventToken);
        }

        // TRANSLATE: rootElement = GetTemplateChild("RootElement") as FrameworkElement;

        m_rootElement = rootElement;

        if (rootElement != NULL)
        {
            // TODO: FIX ASYNC OF THIS EVENT HANDLING
            EventRegistrationToken eventToken;
            ISizeChangedEventHandler* eventHandler;
            // rootElement->add_SizeChanged(eventHandler, &eventToken);
            rootElement->add_SizeChanged(Callback<ISizeChangedEventHandler>(
                                             [/*args*/](IInspectable* /*sender*/, ISizeChangedEventArgs *
                                                        /*args*/) -> HRESULT { return S_OK; })
                                             .Get(),
                                         &eventToken);

            // Set m_containerElement
            HSTRING name;
            UTF8ToHString("ContainerElement", &name);
            ComPtr<IInspectable> containerElementAsInspectable;
            rootElement->FindName(name, &containerElementAsInspectable);
            // TRANSLATE: _containerElement = rootElement.FindName("ContainerElement") as Canvas;

            // Set m_containerTranslate
            m_containerTranslate = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ITranslateTransform>(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Media_TranslateTransform));

            // Set m_containerElement.RenderTransform
            ComPtr<IUIElement> containerElementAsUIElement;
            m_containerElement.As(&containerElementAsUIElement);
            ComPtr<ITransform> containerTranslateAsTransform;
            m_containerTranslate.As(&containerTranslateAsTransform);
            containerElementAsUIElement->put_RenderTransform(containerTranslateAsTransform.Get());

            // TRANSLATE: await LoadImageBrush(ImageSource);
        }

        // TRANSLATE: base.OnApplyTemplate()
        // ABI::Windows::UI::Xaml::Controls::ContentControl::OnApplyTemplate();
    }

    /*ABI::Windows::Foundation::IAsyncAction TileControl::RootElement_SizeChanged()
    {
        m_isRootElementSizeChanged = true;
        await RefreshContainerTileLocked();
    }*/


    void TileControl::RefreshContainerTile()
    {
        Size emptySize;
        ComPtr<ISizeHelperStatics> sizeStatics = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ISizeHelperStatics>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_SizeHelper));
        sizeStatics->get_Empty(&emptySize);

        // SUPPOSED TO GO IN IF STATEMENT: m_imageSize.Get() == emptySize ||
        if (m_rootElement == NULL)
        {
            return;
        }
        else
        {
            DOUBLE actualWidth;
            m_rootElement->get_ActualWidth(&actualWidth);

            DOUBLE actualHeight;
            m_rootElement->get_ActualWidth(&actualHeight);

            // FIGURE OUT ACTUAL WAY TO GET BACKGROUND IMAGE
            IAdaptiveBackgroundImage* backgroundImage;

            RefreshContainerTile(actualWidth, actualHeight, m_imageSize->Width, m_imageSize->Height, &backgroundImage);
        }
    }

    BOOL TileControl::RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight, IAdaptiveBackgroundImage** backgroundImage)
    {
        ABI::AdaptiveNamespace::BackgroundImageMode mode;
        ABI::AdaptiveNamespace::HorizontalAlignment hAlignment;
        ABI::AdaptiveNamespace::VerticalAlignment vAlignment;
        ExtractBackgroundImageData(backgroundImage, &mode, &hAlignment, &vAlignment);

        if (m_isImageSourceLoaded == FALSE || m_isRootElementSizeChanged == FALSE)
        {
            return FALSE;
        }

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
        ComPtr<IVector<IUIElement>> children;
        ComPtr<IPanel> containerElementAsPanel;
        m_containerElement.As(&containerElementAsPanel);
        containerElementAsPanel->get_Children(children.GetAddressOf());

        // instanciate all elements not created yet
        for (INT x = 0; x < numberSpriteToInstanciate - count; x++)
        {
            ComPtr<IRectangle> rectangle = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IRectangle>(
                HStringReference(RuntimeClass_Windows_UI_Xaml_Shapes_Rectangle));

            ComPtr<IUIElement> rectangleAsUIElement;
            rectangle.As(&rectangleAsUIElement);
            children->Append(rectangleAsUIElement);

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
                ComPtr<ICanvasStatics> canvas;
                canvas->SetLeft(rectangleAsUIElement.Get(), (x * imageWidth) + offsetVerticalAlignment);
                canvas->SetTop(rectangleAsUIElement.Get(), (y * imageHeight) + offsetHorizontalAlignment);

                // Set Width and Height for Rectangle
                rectangle->put_RadiusX(imageWidth);
                rectangle->put_RadiusY(imageHeight);
            }
        }
        return TRUE;
    }

    HRESULT TileControl::ExtractBackgroundImageData(ABI::AdaptiveNamespace::IAdaptiveBackgroundImage** backgroundImage,
                                                    ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                                    ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                                    ABI::AdaptiveNamespace::VerticalAlignment* vAlignment)
    {
        auto adaptiveElement = *backgroundImage;

        adaptiveElement->get_Mode(mode);
        adaptiveElement->get_HorizontalAlignment(hAlignment);
        adaptiveElement->get_VerticalAlignment(vAlignment);

        return S_OK;
    }
}
