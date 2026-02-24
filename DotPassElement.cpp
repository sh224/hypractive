#include "DotPassElement.hpp"
#include <hyprland/src/render/OpenGL.hpp>
#include "dotDeco.hpp"

CDotPassElement::CDotPassElement(const CDotPassElement::SDotData& data_) : data(data_) {
    ;
}

void CDotPassElement::draw(const CRegion& damage) {
    data.deco->drawPass(g_pHyprOpenGL->m_renderData.pMonitor.lock(), data.a);
}

bool CDotPassElement::needsLiveBlur() {
    return false;
}

bool CDotPassElement::needsPrecomputeBlur() {
    return false;
}
