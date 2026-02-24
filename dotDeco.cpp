#include "dotDeco.hpp"

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/state/FocusState.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include "DotPassElement.hpp"
#include "globals.hpp"

CHyprdot::CHyprdot(PHLWINDOW pWindow) : IHyprWindowDecoration(pWindow), m_pWindow(pWindow) {
    ;
}

CHyprdot::~CHyprdot() {
    damageEntire();
}

SDecorationPositioningInfo CHyprdot::getPositioningInfo() {
    SDecorationPositioningInfo info;
    info.policy         = DECORATION_POSITION_STICKY;
    info.edges          = DECORATION_EDGE_BOTTOM | DECORATION_EDGE_LEFT | DECORATION_EDGE_RIGHT | DECORATION_EDGE_TOP;
    info.priority       = 9999;
    info.desiredExtents = {{0, 0}, {0, 0}};

    return info;
}

void CHyprdot::onPositioningReply(const SDecorationPositioningReply& reply) {
    m_bAssignedGeometry = reply.assignedGeometry;
}

uint64_t CHyprdot::getDecorationFlags() {
    return DECORATION_PART_OF_MAIN_WINDOW;
}

eDecorationLayer CHyprdot::getDecorationLayer() {
    static auto* const PUNDER = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:decorate_under")->getDataStaticPtr();
    if (**PUNDER == 1) {
        return DECORATION_LAYER_UNDER;
    }
    return DECORATION_LAYER_OVER;
}

std::string CHyprdot::getDisplayName() {
    return "Hyprdot";
}

void CHyprdot::draw(PHLMONITOR pMonitor, const float& a) {
    if (!validMapped(m_pWindow))
        return;

    const auto PWINDOW = m_pWindow.lock();

    if (!PWINDOW->m_ruleApplicator->decorate().valueOrDefault())
        return;

    static auto* const PDFLOAT = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:decorate_floating")->getDataStaticPtr();
    static auto* const PDGROUP = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:decorate_grouped")->getDataStaticPtr();
    if (**PDFLOAT == 0 && PWINDOW->m_isFloating)
        return;
    if (**PDGROUP == 0 && PWINDOW->m_groupData.pNextWindow)
        return;

    CDotPassElement::SDotData data;
    data.deco = this;

    g_pHyprRenderer->m_renderPass.add(makeUnique<CDotPassElement>(data));
}

void CHyprdot::drawPass(PHLMONITOR pMonitor, const float& a) {
    const auto PWINDOW = m_pWindow.lock();
    if (PWINDOW != Desktop::focusState()->window())
        return;

    const auto PWORKSPACE      = PWINDOW->m_workspace;
    const auto WORKSPACEOFFSET = PWORKSPACE && !PWINDOW->m_pinned ? PWORKSPACE->m_renderOffset->value() : Vector2D();

    CBox       fullBox = m_bAssignedGeometry;
    fullBox.translate(g_pDecorationPositioner->getEdgeDefinedPoint(DECORATION_EDGE_BOTTOM | DECORATION_EDGE_LEFT | DECORATION_EDGE_RIGHT | DECORATION_EDGE_TOP, m_pWindow.lock()));
    fullBox.translate(PWINDOW->m_floatingOffset - pMonitor->m_position + WORKSPACEOFFSET);

    static auto* const PSIZE = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:dot_size")->getDataStaticPtr();
    static auto* const POFFX = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:dot_offset_x")->getDataStaticPtr();
    static auto* const POFFY = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:dot_offset_y")->getDataStaticPtr();

    const double       size = (double)std::max<long>(1, **PSIZE);
    const double       x    = fullBox.x + fullBox.w - size * 0.5 + **POFFX;
    const double       y    = fullBox.y - size * 0.5 - **POFFY;
    const CBox         box  = {x, y, size, size};

    m_bLastRelativeBox = box;

    static auto* const PCOLOR = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:dot_color")->getDataStaticPtr();
    static auto* const PROUND = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypractive:dot_rounding")->getDataStaticPtr();

    int                rounding = (**PROUND < 0) ? PWINDOW->rounding() : **PROUND;

    g_pHyprOpenGL->renderRect(box, CHyprColor(**PCOLOR), {.round = rounding, .roundingPower = PWINDOW->roundingPower()});
}

eDecorationType CHyprdot::getDecorationType() {
    return DECORATION_CUSTOM;
}

void CHyprdot::updateWindow(PHLWINDOW pWindow) {
    damageEntire();
}

void CHyprdot::damageEntire() {
    CBox dm = m_bLastRelativeBox.copy().expand(2);
    g_pHyprRenderer->damageBox(dm);
}
