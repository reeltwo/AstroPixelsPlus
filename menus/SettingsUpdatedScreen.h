///////////////////////////////////////////////////////////////////////////////
//
// Startup splash screen showing firmware compilation date
// 
///////////////////////////////////////////////////////////////////////////////

class SettingsUpdatedScreen : public CommandScreen
{
public:
    SettingsUpdatedScreen() :
        CommandScreen(sDisplay, kSettingsUpdatedScreen)
    {}

    virtual void init()
    {
        sDisplay.invertDisplay(false);
        sDisplay.clearDisplay();
        sDisplay.setTextSize(4);
        sDisplay.drawTextCentered("OK");
        sDisplay.display();
    }

    virtual void render()
    {
        if (sDisplay.elapsed() >= 500)
        {
            popScreen();
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Instantiate the screen
//
///////////////////////////////////////////////////////////////////////////////

SettingsUpdatedScreen sSettingsUpdatedScreen;
