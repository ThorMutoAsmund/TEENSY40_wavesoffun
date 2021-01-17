#ifndef GUI_H
#define GUI_H

#include <Arduino.h>
#include "tm1638.h"
#include "patch.h"
#include "synth.h"

#define MOD_TOP_MENUS 5
#define MOD_PRESET_MENUS 6
#define MOD_SYSTEM_MENUS 2
#define MOD_AHDSR_MENUS 8

#define PATCH_OT_START 0
#define PATCH_ADHSR_START 3

class Gui
{
public:
	Gui()
    {
	}
	void init(Synth *synth);
    void reset(void);
    void buttonClick(bool longPress);
    void blink();
    void flashLed();
private:
    bool menuClick(bool longPress, uint8_t button);
    bool selectMenu(uint8_t mode, uint8_t button);
    bool patchMenu(uint8_t mode, uint8_t button);
    bool topMenu(uint8_t mode, uint8_t button);
    bool presetMenu(uint8_t mode, uint8_t button);
    bool overtoneMenu(uint8_t mode, uint8_t button);
    bool ahdsrMenu(uint8_t mode, uint8_t button);
    bool menu4(uint8_t mode, uint8_t button);
    bool systemMenu(uint8_t mode, uint8_t button);
    
    void displayValue(const PatchValueDef *def, Patch *patch);

    void getValue(uint8_t button, const PatchValueDef *def, Patch *patch);
    void getUInt16(uint8_t button, uint16_t *current, uint16_t min, uint16_t max);
    void getFloat(uint8_t button, uint16_t *current, uint16_t min, uint16_t max);
    void getEnum(uint8_t button, uint8_t *value, uint8_t max, const char **options);

    uint8_t get12Sel(uint8_t button, uint8_t current, uint8_t numMenus, uint8_t min);
    void padSpace(char *buf, uint8_t max);

    void execPreset(uint8_t type);

    const char* otLabels[MOD_PATCH_OT] = { "BASE    ", "HARMONIC" };
    const char* otAmpLabels[MOD_PATCH_OT_AMP] = { "1 OVR T ", "1 OVR TT" };
    const char* ahdsrShapeLabels[MOD_PATCH_AHDSR_SHAPE_OPTIONS] = { "LINEAR  ", "ESI CUBE", "ESI QUAD", "ESO CUBE", "ESO QUAD" };

    const char* lev1Menus[MOD_TOP_MENUS] = { "PRESET  ", "OVERTONE", "AHDSR   ", "MENU4   ", "SYSTEM  " };
    const char* presetMenuLabels[MOD_PRESET_MENUS] = { "SINE    ", "TRI     ", "SAW     ", "SQUARE  ", "CIR     ", "STEPSAW " };
    const char* ahdsrMenuLabels[MOD_AHDSR_MENUS] = { "ATK TIME", "ATK SHAP", "HLD TIME", "DEC TIME", "DEC SHAP", "SUS LEVL", "REL TIME", "REL SHAP" };
    const char* systemMenuLabels[MOD_SYSTEM_MENUS] = { "CPU LEFT", "CLK SPED" };

    const char* pctLabels[1] = { "PCT" };
    const char* secLabels[1] = { "  S" };

    const PatchValueDef patchValueDefs[15] = {
        // ot
        { 0, NUMTYPE_ENUM, 0, MOD_PATCH_OT-1, otLabels },
        { 2, NUMTYPE_PHASE, 0, 0, NULL },
        { 4, NUMTYPE_ENUM, 0, MOD_PATCH_OT_AMP-1, otAmpLabels },
        // ahdsr
        { 6, NUMTYPE_FLOAT, 0, 3000, secLabels },
        { 8, NUMTYPE_ENUM, 0, MOD_PATCH_AHDSR_SHAPE_OPTIONS-1, ahdsrShapeLabels },
        { 10, NUMTYPE_FLOAT, 0, 6000, secLabels },
        { 12, NUMTYPE_FLOAT, 0, 6000, secLabels },
        { 14, NUMTYPE_ENUM, 0, MOD_PATCH_AHDSR_SHAPE_OPTIONS-1, ahdsrShapeLabels },
        { 16, NUMTYPE_FLOAT, 0, 1000, pctLabels },
        { 18, NUMTYPE_FLOAT, 0, 6000, secLabels },
        { 20, NUMTYPE_ENUM, 0, MOD_PATCH_AHDSR_SHAPE_OPTIONS-1, ahdsrShapeLabels }
    };
    // const PatchValueDef patchValueDefs[15] = {
    //     // ot
    //     { 0, NUMTYPE_ENUM, 0, MOD_PATCH_OT-1, otLabels },
    //     { 1, NUMTYPE_PHASE, 0, 0, NULL },
    //     { 2, NUMTYPE_ENUM, 0, MOD_PATCH_OT_AMP-1, otAmpLabels },
    //     // ahdsr
    //     { 3, NUMTYPE_FLOAT, 0, 3000, secLabels },
    //     { 5, NUMTYPE_ENUM, 0, MOD_PATCH_AHDSR_SHAPE_OPTIONS-1, ahdsrShapeLabels },
    //     { 6, NUMTYPE_FLOAT, 0, 6000, secLabels },
    //     { 8, NUMTYPE_FLOAT, 0, 6000, secLabels },
    //     { 10, NUMTYPE_ENUM, 0, MOD_PATCH_AHDSR_SHAPE_OPTIONS-1, ahdsrShapeLabels },
    //     { 11, NUMTYPE_FLOAT, 0, 1000, pctLabels },
    //     { 13, NUMTYPE_FLOAT, 0, 6000, secLabels },
    //     { 15, NUMTYPE_ENUM, 0, MOD_PATCH_AHDSR_SHAPE_OPTIONS-1, ahdsrShapeLabels }
    // };

    Synth *synth;
    Patch *patch;
    uint8_t bank;
    uint8_t pno;
    uint32_t menu;
    uint8_t nextBank;
    bool resetValue;
    bool storeUndoValue;
    uint8_t uint8UndoBuffer;
    uint16_t uint16UndoBuffer;
};

extern Gui gui;

#endif
