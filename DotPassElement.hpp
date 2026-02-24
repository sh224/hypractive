#pragma once
#include <hyprland/src/render/pass/PassElement.hpp>

class CHyprdot;

class CDotPassElement : public IPassElement {
  public:
    struct SDotData {
        CHyprdot* deco = nullptr;
        float     a    = 1.F;
    };

    CDotPassElement(const SDotData& data_);
    virtual ~CDotPassElement() = default;

    virtual void        draw(const CRegion& damage);
    virtual bool        needsLiveBlur();
    virtual bool        needsPrecomputeBlur();

    virtual const char* passName() {
        return "CDotPassElement";
    }

  private:
    SDotData data;
};
