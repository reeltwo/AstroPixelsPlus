///////////////////////////////////////////////////////////////////////////////
//
// Main screen shows current dome position if dome position changes.
// Push button will activate SelectScreen
// 
///////////////////////////////////////////////////////////////////////////////

static const char* sMainMenu[] = {
    "Sequence",
    "Logics",
    "Holo"
};

class MainScreen : public MenuScreen
{
public:
    MainScreen() :
        MenuScreen(kMainScreen, sMainMenu, SizeOfArray(sMainMenu))
    {}

    virtual void buttonInReleased() override
    {
        DEBUG_PRINTLN("TODO");
    }

#ifdef USE_DROID_REMOTE
    virtual void buttonLeftPressed(bool repeat) override
    {
        if (remoteEnabled)
        {
        #ifdef USE_SMQ
            if (SMQ::sendTopic("EXIT", "Remote"))
            {
                SMQ::sendString("addr", SMQ::getAddress());
                SMQ::sendEnd();
                sDisplay.setEnabled(false);
            }
        #endif
        }
    }
#endif
};

///////////////////////////////////////////////////////////////////////////////
//
// Instantiate the screen
//
///////////////////////////////////////////////////////////////////////////////

MainScreen sMainScreen;
