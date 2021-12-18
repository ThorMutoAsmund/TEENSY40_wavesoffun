#include "gui.h"
#include "globals.h"

#define BTNMODE_DISPLAY 0
#define BTNMODE_CLICK 1
#define BTNMODE_LONG_PRESS 2

#define BTN_OK 12
#define BTN_PREV 13
#define BTN_NEXT 14
#define BTN_BACK 15


#define MENU_MASK 0x000f
#define SUBMENU_MASK 0x00f0
#define VALUEMENU_MASK 0x0F00
#define MENU_WELCOME 0
#define MENU_PATCH 1
#define MENU_FIRST 2
#define MENU_PRESET 2
#define MENU_OVERTONE 3
#define MENU_AHDSR 4
#define MENU_SYSTEM 5

#define SUBMENU_CPULOAD 0x10
#define SUBMENU_CLKSPEED 0x20

// Menus
// 0x0000 Welcome
// 0x0001 Patch
// 0x0002 = Edit, select "PRESET" when ok clicked
// 0x0003 = Edit, select "OVERTONE" when ok clicked
// 0x0004 = Edit, select "AHDSR" when ok clicked
// 0x0005 = Edit, select "SYSTEM"  when ok clicked
// 0x0012 Preset/Sine
// 0x0022 Preset/Tri
// 0x0032 Preset/Saw
// 0x0042 Preset/Square
// Etc

Gui gui;
bool ledIsOn = false;

void buttonClickHandler(bool longPress)
{
    gui.buttonClick(longPress);
}

void blinkHandler()
{
    cpuLoadReset = true;
    gui.blink();
}

void fastBlinkHandler()
{
    if (ledIsOn)
    {
        digitalWrite(LED_BUILTIN, LOW);
        ledIsOn = false;
    }
}

void Gui::init(Synth *_synth)
{
    this->synth = _synth;
    this->patch = &(synth->patch);
    tm1638.setup();
    tm1638.clearDisplay();
    tm1638.setBrightness(0x02);
    tm1638.onButtonClick(buttonClickHandler);
    tm1638.onBlink(blinkHandler);
    tm1638.onFastBlink(fastBlinkHandler);    
}

void Gui::reset()
{
    bank = 0;
    pno = 0;
    menu = MENU_WELCOME;
    tm1638.writeln("WELCOME ", 0);
    delay(1000);
    menu = MENU_PATCH;
    selectMenu(BTNMODE_DISPLAY, 0);
}

void Gui::flashLed()
{
    if (!ledIsOn)
    {
        digitalWrite(LED_BUILTIN, HIGH);
        ledIsOn = true;
    }
}

void Gui::blink()
{    
   if (menu == MENU_PATCH && nextBank)
    {
        tm1638.toggle(4);
    }
 }

void Gui::buttonClick(bool longPress)
{
    uint8_t button = tm1638.pressedButton();

    if ((button != 0xff || longPress) && selectMenu(longPress ? BTNMODE_LONG_PRESS : BTNMODE_CLICK, button))
    {
        selectMenu(BTNMODE_DISPLAY, 0);
    }
}

bool Gui::selectMenu(uint8_t mode, uint8_t button)
{
    switch (menu & 0x000f)
    {
        case MENU_WELCOME:
            return false;
        case MENU_PATCH: 
            return patchMenu(mode, button);
        default:
            if (menu & SUBMENU_MASK)
            {
                switch (menu & MENU_MASK)
                {
                    case MENU_PRESET: 
                        return presetMenu(mode, button);
                    case MENU_OVERTONE: 
                        return overtoneMenu(mode, button);
                    case MENU_AHDSR: 
                        return ahdsrMenu(mode, button);
                    case MENU_SYSTEM: 
                        return systemMenu(mode, button);
                    default:
                        return false;
                }
            }
            else
            {
                return editMenu(mode, button); 
            }
    }
}

bool Gui::patchMenu(uint8_t mode, uint8_t button)
{
    if (mode == BTNMODE_LONG_PRESS)
    {
        if (button < 8)
        {
            return false;
        }
        nextBank = 0;
        tm1638.clearToggle(4);
        menu = (button - 8 < MOD_LEV1_MENUS ? button - 8 : 0) + MENU_FIRST;
        return true;
    }
    else if (mode == BTNMODE_CLICK)
    {
        uint8_t button = tm1638.pressedButton();
        if (button < 8)
        {
            pno = button;
        }
        else if (button < 16)
        {
            nextBank = button;
        }

        if (nextBank != 0 && button < 8)
        {
            bank = nextBank & 0x07;
            nextBank = 0;
            tm1638.clearToggle(4);
        }
    }

    // Display
    tm1638.write("  -", 0);
    tm1638.write((char)(nextBank ? nextBank & 0x07 : bank)+'A', 3);
    tm1638.write((char)pno+'1', 4);
    tm1638.writeln("-  ", 5);

    return false;
}

bool Gui::editMenu(uint8_t mode, uint8_t button)
{
    if (mode == BTNMODE_LONG_PRESS)
    {
        return false;
    }
    else if (mode == BTNMODE_CLICK)
    {
        if (button == BTN_BACK)
        {
            menu = MENU_PATCH;
            return true;
        }
        else if (button == BTN_OK)
        {
            menu |= 0x10;
            return true;
        }
        menu = get12Sel(button, menu, MOD_LEV1_MENUS, MENU_FIRST);
    }

    // Display
    tm1638.writeln(lev1Menus[(menu & 0x0F) - 2], 0);
    
    return false;
}

bool Gui::presetMenu(uint8_t mode, uint8_t button)
{
    if (mode == BTNMODE_LONG_PRESS)
    {
        return false;
    }
    else if (mode == BTNMODE_CLICK)
    {
        if (button == BTN_OK)
        {
            this->synth->execPreset(((menu & SUBMENU_MASK) >> 4) - 1);
            button = BTN_BACK;
        }
        if (button == BTN_BACK)
        {
            menu &= MENU_MASK;
            return true;
        }
        menu = (menu & MENU_MASK) | (get12Sel(button, (menu & SUBMENU_MASK) >> 4 , MOD_PRESET_MENUS, 1) << 4);
    }

    // Display
    tm1638.writeln(presetMenuLabels[((menu & SUBMENU_MASK) >> 4) - 1], 0);
    
    return false;
}

bool Gui::overtoneMenu(uint8_t mode, uint8_t button)
{
    if (mode == BTNMODE_LONG_PRESS)
    {
        return false;
    }
    else if (mode == BTNMODE_CLICK)
    {
        if (button == BTN_BACK)
        {
            menu &= MENU_MASK;
            return true;
        }
    }
    
    // Display
    tm1638.writeln(naLabel, 0);

    return false;
}

bool Gui::ahdsrMenu(uint8_t mode, uint8_t button)
{
    const PatchValueDef *def = &(patchValueDefs[PATCH_ADHSR_START + ((menu & SUBMENU_MASK)>>4) - 1]);

    if (mode == BTNMODE_LONG_PRESS)
    {
        return false;
    }
    else if (mode == BTNMODE_CLICK)
    {
        if (menu & VALUEMENU_MASK)
        {
            if (button == BTN_OK)
            {
                this->synth->patchChanged();

                // If button == BTN_OK, return to previous menu and indicate a menu repaint
                menu &= 0x00FF;
                return true;
            }
            if (button == BTN_BACK)
            {
                // If button == BTN_BACK, reset the menu and continue. getValue called with BTN_BACK will retreive the undo value, then continue to show the correct menu
                menu &= 0x00FF;
            }
            getValue(button, def, patch);
        }
        else
        {
            if (button == BTN_OK)
            {
                menu = (menu & 0x00FF) | 0x0100;
                resetValue = true;
                storeUndoValue = true;
                return true;
            }
            if (button == BTN_BACK)
            {
                menu &= MENU_MASK;
                return true;
            }
            menu = (menu & ~SUBMENU_MASK) | (get12Sel(button, (menu & SUBMENU_MASK) >> 4 , MOD_AHDSR_MENUS, 1) << 4);
        }
    }

    // Display
    if (menu & VALUEMENU_MASK)
    {
        displayValue(def, patch);
    }
    else
    {
        tm1638.writeln(ahdsrMenuLabels[((menu & SUBMENU_MASK) >> 4) - 1], 0);
    }    
    
    return false;
}

bool Gui::systemMenu(uint8_t mode, uint8_t button)
{
    char buf[9] = "        ";

    if (mode == BTNMODE_LONG_PRESS)
    {
        return false;
    }
    else if (mode == BTNMODE_CLICK)
    {
        if (button == BTN_OK)
        {
            switch (menu & SUBMENU_MASK)
            {
                case SUBMENU_CPULOAD:
                    sprintf(buf, "%" PRIu32, lastCPULoadCount);
                    padSpace(buf, 8);
                    tm1638.writeln(buf, 0);
                    return false;
                case SUBMENU_CLKSPEED:
                    sprintf(buf, "%" PRIu32, F_CPU/1000000L);
                    padSpace(buf, 8);
                    buf[5] = 'M';
                    buf[6] = 'H';
                    buf[7] = 'Z';
                    tm1638.writeln(buf, 0);
                    return false;
            }
        }
        if (button == BTN_BACK)
        {
            menu &= MENU_MASK;
            return true;
        }
        menu = (menu & ~SUBMENU_MASK) | (get12Sel(button, (menu & SUBMENU_MASK) >> 4 , MOD_SYSTEM_MENUS, 1) << 4);
    }

    // Display
    tm1638.writeln(systemMenuLabels[((menu & SUBMENU_MASK) >> 4) - 1], 0);
    
    return false;
}

void Gui::displayValue(const PatchValueDef *def, Patch *patch)
{
    void* valueStart = ((byte*)patch) + def->offset;
    char buf[9] = "        ";

    switch (def->numType)
    {
        case NUMTYPE_ENUM:
            tm1638.writeln(def->options[*((uint8_t*)valueStart)], 0);
            return;
        case NUMTYPE_UINT16:
            sprintf(buf, "%" PRIu16, *((uint16_t*)valueStart));
            padSpace(buf, 8);
            break;
        case NUMTYPE_FLOAT:
            sprintf(buf, "%04d", *((uint16_t*)valueStart));
            padSpace(buf, 8);
            sprintf(buf+5, "%s", def->options[0]);
            buf[0] = buf[0] | 0x80;
            break;
        case NUMTYPE_PHASE:
            break;
    }
    tm1638.writeln(buf, 0);
}

void Gui::displayUINT16(uint16_t value)
{
    char buf[9] = "        ";

    sprintf(buf, "%" PRIu16, value);
    padSpace(buf, 8);
    tm1638.writeln(buf, 0);
}

void Gui::getValue(uint8_t button, const PatchValueDef *def, Patch *patch)
{
    void* valueStart = ((byte*)patch) + def->offset;
    switch (def->numType)
    {
        case NUMTYPE_ENUM:
            // If button == BTN_BACK it means "retrieve undo value"
            if (button == BTN_BACK)
            {
                if (!storeUndoValue)
                {
                    *((uint8_t*)valueStart) = uint8UndoBuffer;
                }
                return;
            }
            // If asked to store an undo value, store it here
            if (storeUndoValue)
            {
                uint8UndoBuffer = *((uint8_t*)valueStart);
                storeUndoValue = false;
            }
            // Get new value
            getEnum(button, (uint8_t*)valueStart, def->max, def->options);
            break;
        case NUMTYPE_UINT16:
            // If button == BTN_BACK it means "retrieve undo value"
            if (button == BTN_BACK)
            {
                if (!storeUndoValue)
                {
                    *((uint16_t*)valueStart) = uint16UndoBuffer;
                }
                return;
            }
            // If asked to store an undo value, store it here
            if (storeUndoValue)
            {
                uint16UndoBuffer = *((uint16_t*)valueStart);
                storeUndoValue = false;
            }
            // Get new value
            getUInt16(button, (uint16_t*)valueStart, def->min, def->max);
            break;
        case NUMTYPE_FLOAT:
            // If button == BTN_BACK it means "retrieve undo value"
            if (button == BTN_BACK)
            {
                if (!storeUndoValue)
                {
                    *((uint16_t*)valueStart) = uint16UndoBuffer;
                }
                return;
            }
            // If asked to store an undo value, store it here
            if (storeUndoValue)
            {
                uint16UndoBuffer = *((uint16_t*)valueStart);
                storeUndoValue = false;
            }
            // Get new value
            getFloat(button, (uint16_t*)valueStart, def->min, def->max);
            break;
        case NUMTYPE_PHASE:
            // getPhase();
            break;
    }
}

void Gui::getUInt16(uint8_t button, uint16_t *value, uint16_t min, uint16_t max)
{
    if (button < 10)
    {
        if (resetValue)
        {
            (*value) = 0;
            resetValue = false;
        }
        if (*value < 6553 || (*value == 6553 && button < 5))
        {
            (*value) = (*value) * 10 + ((button+1)%10);
        }
        
        if (max != 0xffff && *value > max)
        {
            *value = max;
        }
    }
    else if (button == BTN_PREV)
    {
        (*value) = (*value) == min ? max : (*value) - 1;
        resetValue = true;
    }
    else if (button == BTN_NEXT)
    {
        (*value) = (*value) == max ? min : (*value) + 1;
        resetValue = true;
    }
}

void Gui::getFloat(uint8_t button, uint16_t *value, uint16_t min, uint16_t max)
{
    if (button < 10)
    {
        button = ((button+1)%10);
        if (resetValue)
        {
            (*value) = 0;
            resetValue = false;
        }
        uint16_t newValue = (*value) * 10 + button;
        *value = newValue <= max ? newValue : max;
    }
    else if (button == BTN_PREV)
    {
        (*value) = (*value) == min ? max : (*value) - 1;
        resetValue = true;
    }
    else if (button == BTN_NEXT)
    {
        (*value) = (*value) == max ? min : (*value) + 1;
        resetValue = true;
    }
}

void Gui::getEnum(uint8_t button, uint8_t *value, uint8_t max, const char **options)
{
    if (button < 10 && button < max)
    {
        *value = button;
    }
    else if (button == BTN_PREV)
    {
        (*value) = (*value) == 0 ? max : (*value) - 1;
    }
    else if (button == BTN_NEXT)
    {
        (*value) = (*value) == max ? 0 : (*value) + 1;
    }
}

uint8_t Gui::get12Sel(uint8_t button, uint8_t current, uint8_t numMenus, uint8_t min)
{
    if (button < numMenus)
    {
        return button + min;
    }
    else if (button == BTN_PREV)
    {
        return ((current + numMenus - 1 - min) % numMenus) + min;
    }
    else if (button == BTN_NEXT)
    {
        return ((current + 1 - min) % numMenus) + min;
    }

    return current;
}

void Gui::padSpace(char *buf, uint8_t max)
{
    bool termFound = false;
    for (uint8_t p=1; p<max; ++p)
    {
        if (!termFound)
        {
            if (!buf[p])
            {
                buf[p] = ' ';
                termFound = true;
            }
        }
        else
        {
            buf[p] = ' ';
        }        
    }
    buf[max] = 0;
}


