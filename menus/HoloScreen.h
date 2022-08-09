///////////////////////////////////////////////////////////////////////////////
//
// Main screen shows current dome position if dome position changes.
// Push button will activate SelectScreen
// 
///////////////////////////////////////////////////////////////////////////////

static const char* sHoloMenu[] = {
    "Sequence",
    "Holo"
};

class HoloScreen : public MenuScreen
{
public:
    enum {
        kSequence,
        kHolo,
    };
    HoloScreen() :
        MenuScreen(kHoloScreen, sHoloMenu, SizeOfArray(sHoloMenu))
    {}

    virtual void buttonInReleased() override
    {
        switch (fCurrentItem)
        {
            case kSequence:
                break;
            case kHolo:
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

HoloScreen sHoloScreen;
