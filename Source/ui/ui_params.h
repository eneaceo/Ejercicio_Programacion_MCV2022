#pragma once

#include "ui/ui.fwd.h"

namespace ui
{
    struct TImageParams
    {
        Color color = Color::One;
        const CTexture* texture = nullptr;
        VEC2 minUV = VEC2::Zero;
        VEC2 maxUV = VEC2::One;
        bool additive = false;

        void renderInMenu();
    };

    struct TFontParams
    {
        std::string name;
        const CTexture* texture = nullptr;
        int numRows = 1;
        int numCols = 1;

        void renderInMenu();
    };

    struct TTextParams
    {
        std::string text;
        Color color = Color::One;
        const TFontParams* font = nullptr;
        int size = 16;
        EHAlign halign = EHAlign::Left;
        EVAlign valign = EVAlign::Top;

        void renderInMenu();
    };
}
