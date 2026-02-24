#define WLR_USE_UNSTABLE

#include <unistd.h>

#include <any>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/desktop/view/Window.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/render/Renderer.hpp>

#include "dotDeco.hpp"
#include "globals.hpp"

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

void onNewWindow(void* self, std::any data) {
    // data is guaranteed
    const auto PWINDOW = std::any_cast<PHLWINDOW>(data);

    HyprlandAPI::addWindowDecoration(PHANDLE, PWINDOW, makeUnique<CHyprdot>(PWINDOW));
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH        = __hyprland_api_get_hash();
    const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

    if (HASH != CLIENT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[hypractive] Failure in initialization: Version mismatch (headers ver is not equal to running hyprland ver)",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[hypractive] Version mismatch");
    }

    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:dot_size", Hyprlang::INT{10});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:dot_offset_x", Hyprlang::INT{0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:dot_offset_y", Hyprlang::INT{0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:dot_color", Hyprlang::INT{*configStringToInt("rgba(ff0000aa)")});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:dot_rounding", Hyprlang::INT{-1});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:decorate_under", Hyprlang::INT{0});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:decorate_floating", Hyprlang::INT{1});
    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypractive:decorate_grouped", Hyprlang::INT{1});

    HyprlandAPI::reloadConfig();

    static auto POW = HyprlandAPI::registerCallbackDynamic(PHANDLE, "openWindow", [&](void* self, SCallbackInfo& info, std::any data) { onNewWindow(self, data); });

    // add deco to existing windows
    for (auto& w : g_pCompositor->m_windows) {
        if (w->isHidden() || !w->m_isMapped)
            continue;

        HyprlandAPI::addWindowDecoration(PHANDLE, w, makeUnique<CHyprdot>(w));
    }

    HyprlandAPI::addNotification(PHANDLE, "[hypractive] Initialized successfully!", CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);

    return {"hypractive", "A plugin to add a minimally intrusive active window indicator.", "sh", "1.0"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    g_pHyprRenderer->m_renderPass.removeAllOfType("CDotPassElement");
}
