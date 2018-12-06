#include "pch.h"

#include "AdaptiveColumnRenderer.h"
#include "enums.h"
#include "Util.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::AdaptiveNamespace;
using namespace ABI::Windows::Foundation;

namespace AdaptiveNamespace
{
    AdaptiveColumnRenderer::AdaptiveColumnRenderer() { m_xamlBuilder = std::make_shared<XamlBuilder>(); }

    AdaptiveColumnRenderer::AdaptiveColumnRenderer(std::shared_ptr<XamlBuilder> xamlBuilder) :
        m_xamlBuilder(xamlBuilder)
    {
    }

    HRESULT AdaptiveColumnRenderer::RuntimeClassInitialize() noexcept try
    {
        return S_OK;
    }
    CATCH_RETURN;

    _Use_decl_annotations_ HRESULT AdaptiveColumnRenderer::Render(IAdaptiveCardElement* cardElement,
                                                                  IAdaptiveRenderContext* renderContext,
                                                                  IAdaptiveRenderArgs* renderArgs,
                                                                  ABI::Windows::UI::Xaml::IUIElement** result)
    {
        m_xamlBuilder->BuildColumn(cardElement, renderContext, renderArgs, result);
        return S_OK;
    }

}
