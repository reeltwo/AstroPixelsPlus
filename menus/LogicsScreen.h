///////////////////////////////////////////////////////////////////////////////
//
// Main screen shows current dome position if dome position changes.
// Push button will activate SelectScreen
// 
///////////////////////////////////////////////////////////////////////////////

static const char* sLogicsMenu[] = {
    "Sequence",
    "Logics"
};

class LogicsScreen : public MenuScreen
{
public:
    enum {
        kSequence,
        kLogics,
    };
    LogicsScreen() :
        MenuScreen(kLogicsScreen, sLogicsMenu, SizeOfArray(sLogicsMenu))
    {}

    virtual void buttonInReleased() override
    {
        switch (fCurrentItem)
        {
            case kSequence:
                break;
            case kLogics:
                break;
        }
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

LogicsScreen sLogicsScreen;
