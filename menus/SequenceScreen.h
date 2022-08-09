///////////////////////////////////////////////////////////////////////////////
//
// Main screen shows current dome position if dome position changes.
// Push button will activate SelectScreen
// 
///////////////////////////////////////////////////////////////////////////////

static const char* sSequenceMenu[] = {
    "Normal",
    "Flash",
    "Alarm",
    "Failure",
    "Scream",
    "Leia",
    "March",
    "Fire",
    "Pulse",
    "Plasma",
    "Fractal",
    "Fade And\nScroll"
};

class SequenceScreen : public MenuScreen
{
public:
    enum {
        kNormal,
        kFlash,
        kAlarm,
        kFailure,
        kScream,
        kLeia,
        kMarch,
        kFire,
        kPulse,
        kPlasma,
        kFractal,
        kFadeAndScroll
    };
    SequenceScreen() :
        MenuScreen(kSequenceScreen, sSequenceMenu, SizeOfArray(sSequenceMenu))
    {}

    virtual void buttonInReleased() override
    {
        switch (fCurrentItem)
        {
            case kNormal:
                FLD.selectSequence(LogicEngineRenderer::NORMAL);
                RLD.selectSequence(LogicEngineRenderer::NORMAL);
                frontPSI.selectSequence(LogicEngineRenderer::PSICOLORWIPE);
                rearPSI.selectSequence(LogicEngineRenderer::PSICOLORWIPE);
                break;
            case kAlarm:
                FLD.selectSequence(LogicEngineRenderer::ALARM);
                RLD.selectSequence(LogicEngineRenderer::ALARM);
                break;
            case kLeia:
                FLD.selectSequence(LogicEngineRenderer::LEIA);
                RLD.selectSequence(LogicEngineRenderer::LEIA);
                break;
            case kMarch:
                FLD.selectSequence(LogicEngineRenderer::MARCH);
                RLD.selectSequence(LogicEngineRenderer::MARCH);
                break;
            case kFlash:
                FLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
                RLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
                break;
            case kFailure:
                FLD.selectSequence(LogicEngineRenderer::FAILURE);
                RLD.selectSequence(LogicEngineRenderer::FAILURE);
                frontPSI.selectSequence(LogicEngineRenderer::FAILURE);
                rearPSI.selectSequence(LogicEngineRenderer::FAILURE);
                break;
            case kScream:
                FLD.selectSequence(LogicEngineRenderer::REDALERT);
                RLD.selectSequence(LogicEngineRenderer::REDALERT);
                break;
            case kFire:
                FLD.selectSequence(LogicEngineRenderer::FIRE);
                RLD.selectSequence(LogicEngineRenderer::FIRE);
                frontPSI.selectSequence(LogicEngineRenderer::FIRE);
                rearPSI.selectSequence(LogicEngineRenderer::FIRE);
                break;
            case kPulse:
                FLD.selectSequence(LogicEngineRenderer::PULSE);
                RLD.selectSequence(LogicEngineRenderer::PULSE);
                frontPSI.selectSequence(LogicEngineRenderer::PULSE);
                rearPSI.selectSequence(LogicEngineRenderer::PULSE);
                break;
            case kPlasma:
                FLD.selectSequence(PLASMA);
                RLD.selectSequence(PLASMA);
                frontPSI.selectSequence(PLASMA);
                rearPSI.selectSequence(PLASMA);
                break;
            case kFractal:
                FLD.selectSequence(FRACTAL);
                RLD.selectSequence(FRACTAL);
                frontPSI.selectSequence(FRACTAL);
                rearPSI.selectSequence(FRACTAL);
                break;
            case kFadeAndScroll:
                FLD.selectSequence(FADEANDSCROLL);
                RLD.selectSequence(FADEANDSCROLL);
                frontPSI.selectSequence(FADEANDSCROLL);
                rearPSI.selectSequence(FADEANDSCROLL);
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

SequenceScreen sSequenceScreen;
