#include "mcv_platform.h"
#include "ui/ui_widget.h"
#include "ui/ui_effect.h"
#include "render/draw_primitives.h"

namespace ui
{
    void CWidget::updateRecursive(float elapsed)
    {
        if(!_visible)
            return;

        update(elapsed);

        for (auto effect : _effects)
        {
            effect->update(this, elapsed);
        }

        for (auto child : _children)
        {
            child->updateRecursive(elapsed);
        }
    }

    void CWidget::renderRecursive()
    {
        if (!_visible)
            return;

        render();

        for (auto& child : _children)
        {
            child->renderRecursive();
        }
    }

    void CWidget::renderDebugRecursive()
    {
        if (!_visible)
            return;

        renderDebug();

        for (auto& child : _children)
        {
            child->renderDebugRecursive();
        }
    }

    void CWidget::setPivot(VEC2 pivot)
    {
        _pivot = pivot;
        updateLocalTransform();
    }

    void CWidget::setPosition(VEC2 position)
    {
        _position = position;
        updateLocalTransform();
    }

    void CWidget::setScale(VEC2 scale)
    {
        _scale = scale;
        updateLocalTransform();
    }

    void CWidget::setAngle(float angle)
    {
        _angle = angle;
        updateLocalTransform();
    }

    void CWidget::setSize(ESizeType type, const VEC2& size)
    {
        _sizeType = type;
        _localSize = size;

        updateSize();
        updateChildren();
    }

    void CWidget::updateLocalTransform()
    {
        MAT44 pv = MAT44::CreateTranslation(-_pivot.x, -_pivot.y, 0.f);
        MAT44 sc = MAT44::CreateScale(_scale.x, _scale.y, 1.f);
        MAT44 rt = MAT44::CreateRotationZ(_angle);
        MAT44 tr = MAT44::CreateTranslation(_position.x, _position.y, 0.f);
        
        _localTransform = pv * sc * rt * tr;

        updateWorldTransform();
        updateChildren();
    }

    void CWidget::updateWorldTransform()
    {
        if (_parent)
        {
            _worldTransform = _localTransform * _parent->_worldTransform;
        }
        else
        {
            _worldTransform = _localTransform;
        }
    }

    void CWidget::updateSize()
    {
        switch (_sizeType)
        {
            case ESizeType::Percent:
            {
                const VEC2 parentSize = _parent ? _parent->getSize() : VEC2::Zero;
                _worldSize = parentSize * _localSize;
                break;
            }
            case ESizeType::Fixed:
            default:
            {
                _worldSize = _localSize;
            }
        }
    }

    void CWidget::updateChildren()
    {
        for (auto child : _children)
        {
            child->updateFromParent();
        }
    }

    void CWidget::updateFromParent()
    {
        updateSize();
        updateWorldTransform();
        updateChildren();
    }

    void CWidget::addChild(CWidget* widget)
    {
        if (widget->_parent)
        {
            widget->_parent->removeChild(widget);
        }

        _children.push_back(widget);
        widget->_parent = this;
    }

    void CWidget::removeChild(CWidget* widget)
    {
        if (widget->_parent != this)
        {
            assert(!"I'm not the father!");
            return;
        }

        auto it = std::remove(_children.begin(), _children.end(), widget);
        _children.erase(it);
        
        widget->_parent = nullptr;
    }

    void CWidget::addEffect(CEffect* effect)
    {
        _effects.push_back(effect);
    }

    void CWidget::renderInMenuRecursive()
    {
        renderInMenu();

        if (!_effects.empty())
        {
            if (ImGui::TreeNode("Effects"))
            {
                for (auto& effect : _effects)
                {
                    if (ImGui::TreeNode(effect, effect->getType().data()))
                    {
                        effect->renderInMenu();
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }

        if (!_children.empty())
        {
            if (ImGui::TreeNode("Children"))
            {
                for (auto child : _children)
                {
                    const std::string name = std::string(child->getType()) + " - " + child->getName();

                    if (ImGui::TreeNode(child, name.c_str()))
                    {
                        child->renderInMenuRecursive();
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
        }
    }

    void CWidget::renderInMenu()
    {
        bool changed = ImGui::DragFloat2("Position", &_position.x, kDebugMenuSensibility);
        changed = changed || ImGui::DragFloat("Angle", &_angle, kDebugMenuSensibility);
        changed = changed || ImGui::DragFloat2("Scale", &_scale.x, kDebugMenuSensibility);
        changed = changed || ImGui::DragFloat2("Size", &_localSize.x, kDebugMenuSensibility);
        changed = changed || ImGui::DragFloat2("Pivot", &_pivot.x, kDebugMenuSensibility);

        ImGui::Checkbox("Visible", &_visible);

        if (changed)
        {
            updateSize();
            updateLocalTransform(); // will update children
        }
    }

    void CWidget::renderDebug()
    {
        const VEC3 offset(0.01f, 0.f, 0.f);
        const VEC3 topLeft = VEC3::Transform(VEC3::Zero, _worldTransform);
        const VEC3 topRight = VEC3::Transform(VEC3(_worldSize.x, 0.f, 0.f), _worldTransform);
        const VEC3 bottomRight = VEC3::Transform(VEC3(_worldSize.x, _worldSize.y, 0.f), _worldTransform);
        const VEC3 bottomLeft = VEC3::Transform(VEC3(0.f, _worldSize.y, 0.f), _worldTransform);
        const VEC4 color = VEC4::One;

        drawLine(topLeft, topRight, color);
        drawLine(topRight, bottomRight + offset, color);
        drawLine(bottomRight, bottomLeft, color);
        drawLine(bottomLeft, topLeft + offset, color);
    }
}