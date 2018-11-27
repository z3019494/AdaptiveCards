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
#include <windows.ui.xaml.controls.h>
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

// KEEP FOR NOW, CLEAN UP LATER
using namespace ABI::Windows::Storage::Streams;
using namespace ABI::Windows::Storage::FileProperties;

namespace AdaptiveNamespace
{
    _Use_decl_annotations_ HRESULT TileControl::put_RenderContext(_In_ IAdaptiveRenderContext* value)
    {
        m_renderContext = value;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT TileControl::put_BackgroundImage(IAdaptiveBackgroundImage* value)
    {
        m_backgroundImage = value;
        return S_OK;
    }

    _Use_decl_annotations_ HRESULT TileControl::put_isRootElementSizeChanged(BOOL value)
    {
        m_isRootElementSizeChanged = value;
        return S_OK;
    }

    BOOL TileControl::LoadImageBrushAsync()
    {
        HSTRING url;
        m_backgroundImage->get_Url(&url);

        if (m_uri == nullptr)
        {
            m_renderContext->AddWarning(ABI::AdaptiveNamespace::WarningStatusCode::AssetLoadFailed,
                                      HStringReference(L"Image not found").Get());
            return FALSE;
        }

        // TODO TRANSLATE: await_flag.WaitAsync()

        if (m_isImageSourceLoaded == TRUE)
        {
            for (INT i = 0; i < m_xamlChildren.size(); i++)
            {
                // Convert xamlChild to shape to set fill to null
                ComPtr<IShape> childAsShape;
                m_xamlChildren[i].As(&childAsShape);
                childAsShape->put_Fill(NULL);
            }
        }

        m_isImageSourceLoaded = FALSE;

        // Load Image Brush
        ComPtr<IBitmapImage> image = AdaptiveNamespace::XamlHelpers::CreateXamlClass<IBitmapImage>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_Media_Imaging_BitmapImage));

        // Populate storageFile
        ComPtr<IStorageFile> storageFile;
        ComPtr<IAsyncOperation<StorageFile*>> asyncStorageFile;
        ComPtr<IStorageFileStatics> storageFileStatics;
        storageFileStatics->GetFileFromApplicationUriAsync(m_uri, asyncStorageFile.GetAddressOf());
        // TODO: IF THIS FAILS, TRY TO TRANSLATE... BlockOnCompletionAndGetResults(asyncOperation.Get(), &storageFile);
        asyncStorageFile->GetResults(&storageFile);

        // Get image source
        ComPtr<IRandomAccessStream> stream;
        ComPtr<IAsyncOperation<IRandomAccessStream*>> asyncStream;
        storageFile->OpenAsync(FileAccessMode_Read, asyncStream.GetAddressOf());
        asyncStream->GetResults(&stream);

        // Set image source
        ComPtr<IBitmapSource> imageAsBitmapSource;
        image.As(&imageAsBitmapSource);
        imageAsBitmapSource->SetSource(stream.Get());

        // Update Brush
        ComPtr<IImageSource> imageAsSource;
        image.As(&imageAsSource);
        m_brushXaml->put_ImageSource(imageAsSource.Get());

        // Update ImageSize
        INT32 imageWidth;
        INT32 imageHeight;
        image->get_DecodePixelWidth(&imageWidth);
        image->get_DecodePixelHeight(&imageHeight);
        m_imageSize->Height = imageHeight;
        m_imageSize->Width = imageWidth;

        m_isImageSourceLoaded = TRUE;

        RefreshContainerTile();

        // TODO TRANSLATE: _flag.Release()
        return TRUE;
    }

    HRESULT TileControl::OnApplyTemplate()
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
            // ISizeChangedEventHandler* eventHandler;
            // rootElement->add_SizeChanged(eventHandler, &eventToken);
            rootElement->add_SizeChanged(Callback<ISizeChangedEventHandler>([this]() {
                                             this->put_isRootElementSizeChanged(FALSE);
                                             this->RefreshContainerTileLockedAsync();
                                         })
                                             .Get(),
                                         &eventToken);

            // Set m_containerElement
            HSTRING name;
            UTF8ToHString("ContainerElement", &name);
            ComPtr<IInspectable> containerElementAsInspectable;
            rootElement->FindName(name, &containerElementAsInspectable);
            containerElementAsInspectable.As(&m_containerElement);

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
            LoadImageBrushAsync();
        }

        ComPtr<IFrameworkElementOverrides> super;
        super->OnApplyTemplate();
    }

    void TileControl::RefreshContainerTileLockedAsync()
    {
        // TODO TRANSLATE: await _flag.WaitAsync();
        RefreshContainerTile();
        // TODO TRANSLATE: _flag.Release();
    }

    void TileControl::RefreshContainerTile()
    {
        Size emptySize;
        ComPtr<ISizeHelperStatics> sizeStatics = AdaptiveNamespace::XamlHelpers::CreateXamlClass<ISizeHelperStatics>(
            HStringReference(RuntimeClass_Windows_UI_Xaml_SizeHelper));
        sizeStatics->get_Empty(&emptySize);

        // SUPPOSED TO GO IN IF STATEMENT: m_imageSize.Get() == emptySize ||
        BOOL imageSizeIsEmpty = (m_imageSize->Height == emptySize.Height && m_imageSize->Width == emptySize.Width);
        if (imageSizeIsEmpty || m_rootElement == NULL)
        {
            return;
        }
        else
        {
            DOUBLE actualWidth;
            m_rootElement->get_ActualWidth(&actualWidth);

            DOUBLE actualHeight;
            m_rootElement->get_ActualWidth(&actualHeight);

            RefreshContainerTile(actualWidth, actualHeight, m_imageSize->Width, m_imageSize->Height);
        }
    }

    BOOL TileControl::RefreshContainerTile(DOUBLE width, DOUBLE height, FLOAT imageWidth, FLOAT imageHeight)
    {
        ABI::AdaptiveNamespace::BackgroundImageMode mode;
        ABI::AdaptiveNamespace::HorizontalAlignment hAlignment;
        ABI::AdaptiveNamespace::VerticalAlignment vAlignment;
        ExtractBackgroundImageData(&mode, &hAlignment, &vAlignment);

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

    HRESULT TileControl::ExtractBackgroundImageData(ABI::AdaptiveNamespace::BackgroundImageMode* mode,
                                                    ABI::AdaptiveNamespace::HorizontalAlignment* hAlignment,
                                                    ABI::AdaptiveNamespace::VerticalAlignment* vAlignment)
    {
        m_backgroundImage->get_Mode(mode);
        m_backgroundImage->get_HorizontalAlignment(hAlignment);
        m_backgroundImage->get_VerticalAlignment(vAlignment);
        return S_OK;
    }
}
