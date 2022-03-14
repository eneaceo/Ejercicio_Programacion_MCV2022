#pragma once

#include "modules/module.h"
#include "ui/ui.fwd.h"

namespace ui
{
    class CModule : public IModule
    {
    public:
        CModule(const std::string& name);

        bool start() override;
        void update(float delta) override;
        void renderUI() override;
        void renderUIDebug() override;
        void renderInMenu() override;
       
        void registerWidget(CWidget* widget);
        void registerAlias(CWidget* widget);
        void activateWidget(const std::string& name);
        void deactivateWidget(const std::string& name);
        CWidget* getWidget(const std::string& name);

        void registerFont(TFontParams& fontParams);
        const TFontParams* getFont(const std::string& name) const;

        void renderBitmap(const MAT44& world, const TImageParams& params, const VEC2& contentSize);
        void renderText(const MAT44& world, const TTextParams& params, const VEC2& contentSize);

    private:
        VEC2 _resolution;
        CCamera _camera;

        // test
        CImage* background = nullptr;

        std::map<std::string_view, CWidget*> _registeredWidgets;
        std::map<std::string_view, CWidget*> _registeredAlias;
        std::vector<CWidget*> _activeWidgets;
        std::map<std::string, TFontParams> _registeredFonts;

        const CPipelineState* _pipelineCombinative = nullptr;
        const CPipelineState* _pipelineAdditive = nullptr;
        const CPipelineState* _pipelineDebug = nullptr;
        const CMesh* _mesh = nullptr;
        bool _showDebug = false;
    };
}
