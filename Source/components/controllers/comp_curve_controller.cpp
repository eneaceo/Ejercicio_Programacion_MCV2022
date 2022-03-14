#include "mcv_platform.h"
#include "components/common/comp_base.h"
#include "components/common/comp_transform.h"
#include "entity/entity.h"
#include "geometry/curve.h"

struct TCompCurveController : public TCompBase
{
    DECL_SIBLING_ACCESS()

    const CCurve* curve = nullptr;
    float ratio = 0.f;
    bool active = false;
    float speed = 0.f;

    void load(const json& j, TEntityParseContext& ctx)
    {
        const std::string& filename = j["curve"];
        curve = Resources.get(filename)->as<CCurve>();
        active = j.value("active", active);
        speed = j.value("speed", speed);
    }

    void onEntityCreated() 
    {
        TCompTransform* c_target = get<TCompTransform>();
        if (!c_target) return;
        
        initialTransform = c_target->asMatrix();
    }

    void update(float dt)
    {
        TCompTransform* c_target = get<TCompTransform>();
        if (!c_target) return;

        if (active)
        {
            ratio += dt * speed;
        }

        const VEC3 newPosition = curve->evaluate(ratio, initialTransform);
        c_target->setPosition(newPosition);
    }

    void setRatio(float newRatio)
    {
        ratio = newRatio;
    }

    void debugInMenu()
    {
        if (ImGui::TreeNode("Curve"))
        {
            curve->renderInMenu();
            ImGui::TreePop();
        }
        ImGui::Checkbox("Active", &active);
        ImGui::DragFloat("Speed", &speed);
    }

    void renderDebug()
    {
        TCompTransform* c_target = get<TCompTransform>();
        if (c_target)
        {
            curve->renderDebug(initialTransform, VEC4(1.f, 1.f, 0.f, 1.f));
        }
    }

private:
    MAT44 initialTransform;
};

DECL_OBJ_MANAGER("curve_controller", TCompCurveController)