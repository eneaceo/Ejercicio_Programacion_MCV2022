#include "mcv_platform.h"
#include "ui_params.h"
#include "render/textures/texture.h"

namespace ui
{
    void TImageParams::renderInMenu()
    {
        ImGui::Text("Texture: %s", texture ? texture->getName().c_str() : "");
        ImGui::ColorEdit4("Color", &color.x);
        ImGui::Checkbox("Additive", &additive);
    }

    void TFontParams::renderInMenu()
    {
        ImGui::Text("Name: %s", name.c_str());
        ImGui::Text("Texture: %s", texture ? texture->getName().c_str() : "");
        ImGui::DragInt("Cols", &numCols);
        ImGui::DragInt("Rows", &numRows);
    }

    void TTextParams::renderInMenu()
    {
        ImGui::Text("Font: %s", font ? font->name.c_str() : "");
        ImGui::DragInt("Font Size", &size);
        ImGui::ColorEdit4("Color", &color.x);
        ImGui::Text(text.c_str());
    }
}
