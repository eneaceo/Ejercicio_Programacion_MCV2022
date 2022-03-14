#pragma once

#include "ui/ui.fwd.h"

namespace ui
{
    class CWidget
    {
    public:
        void updateRecursive(float dt);
        void renderRecursive();
        void renderDebugRecursive();
        void renderInMenuRecursive();

        virtual void update(float dt) {}
        virtual void render() {};
        virtual void renderInMenu();
        virtual void renderDebug();
        virtual TImageParams* getImageParams() { return nullptr; }

        virtual std::string_view getType() const { return "Widget"; }
        const std::string& getName() const { return _name; }
        const std::string& getAlias() const { return _alias; }
        bool isActive() const { return _active; }
        const VEC2& getSize() const { return _worldSize; }

        void setName(const std::string& name) { _name = name; }
        void setAlias(const std::string& alias) { _alias = alias; }
        void setPivot(VEC2 pivot);
        void setPosition(VEC2 position);
        void setScale(VEC2 scale);
        void setAngle(float angle);
        void setSize(ESizeType type, const VEC2& size);
        void setActive(bool active) { _active = active; }
        void setVisible(bool visible) { _visible = visible; }
       
        void updateLocalTransform();
        void updateWorldTransform();
        void updateSize();
        void updateChildren();
        void updateFromParent();

        void addChild(CWidget* widget);
        void removeChild(CWidget* widget);

        void addEffect(CEffect* effect);

    protected:
        std::string _name;
        std::string _alias;
        VEC2 _pivot = VEC2::Zero;
        VEC2 _position = VEC2::Zero;
        VEC2 _scale = VEC2::One;
        float _angle = 0.f;
        ESizeType _sizeType = ESizeType::Fixed;
        VEC2 _localSize = VEC2::Zero;
        VEC2 _worldSize = VEC2::Zero;
        bool _active = false;
        bool _visible = true;

        MAT44 _localTransform;
        MAT44 _worldTransform;

        CWidget* _parent = nullptr;
        std::vector<CWidget*> _children;
        std::vector<CEffect*> _effects;
    };
}
