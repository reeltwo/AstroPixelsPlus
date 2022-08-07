///////////////////////////////////////////////////////////////////////////////
//
// Startup splash screen showing firmware compilation date
// 
///////////////////////////////////////////////////////////////////////////////

class SplashScreen : public CommandScreen
{
public:
    SplashScreen() :
        CommandScreen(sDisplay, kSplashScreen)
    {}

    virtual void init()
    {
        sDisplay.invertDisplay(false);
        sDisplay.clearDisplay();
        sDisplay.setTextSize(1);
        sDisplay.setCursor(0, 0);
        sDisplay.println(F("AstroPixels"));
        sDisplay.println(F(__DATE__));
        sDisplay.println(F("READY"));
        sDisplay.display();
    }

    virtual void render()
    {
        if (sDisplay.elapsed() >= 5000)
        {
            switchToScreen(kMainScreen);
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
//
// Instantiate the screen
//
///////////////////////////////////////////////////////////////////////////////

SplashScreen sSplashScreen;
