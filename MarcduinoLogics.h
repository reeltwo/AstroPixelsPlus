////////////////

MARCDUINO_ACTION(FLDNormalSequence, @1T1, ({
    FLD.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(FLDFlashSequence, @1T2, ({
    FLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(FLDAlarmSequence, @1T3, ({
    FLD.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(FLDFailureSequence, @1T4, ({
    FLD.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(FLDScreamLogicsSequence, @1T5, ({
    FLD.selectSequence(LogicEngineRenderer::REDALERT);
}))

////////////////

MARCDUINO_ACTION(FLDLeiaLogicsSequence, @1T6, ({
    FLD.selectSequence(LogicEngineRenderer::LEIA);
}))

////////////////

MARCDUINO_ACTION(FLDMarchSequence, @1T11, ({
    FLD.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////

MARCDUINO_ACTION(RLDNormalSequence, @2T1, ({
    RLD.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(RLDFlashSequence, @2T2, ({
    RLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(RLDAlarmSequence, @2T3, ({
    RLD.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(RLDFailureSequence, @2T4, ({
    RLD.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(RLDScreamLogicsSequence, @2T5, ({
    RLD.selectSequence(LogicEngineRenderer::REDALERT);
}))

////////////////

MARCDUINO_ACTION(RLDLeiaLogicsSequence, @2T6, ({
    RLD.selectSequence(LogicEngineRenderer::LEIA);
}))

////////////////

MARCDUINO_ACTION(RLDMarchSequence, @2T11, ({
    RLD.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////

MARCDUINO_ACTION(NormalSequence, @0T1, ({
    FLD.selectSequence(LogicEngineRenderer::NORMAL);
    RLD.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(FlashSequence, @0T2, ({
    FLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
    RLD.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(AlarmSequence, @0T3, ({
    FLD.selectSequence(LogicEngineRenderer::ALARM);
    RLD.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(FailureSequence, @0T4, ({
    FLD.selectSequence(LogicEngineRenderer::FAILURE);
    RLD.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(ScreamLogicsSequence, @0T5, ({
    FLD.selectSequence(LogicEngineRenderer::REDALERT);
    RLD.selectSequence(LogicEngineRenderer::REDALERT);
}))

////////////////

MARCDUINO_ACTION(LeiaLogicsSequence, @0T6, ({
    FLD.selectSequence(LogicEngineRenderer::LEIA);
    RLD.selectSequence(LogicEngineRenderer::LEIA);
}))

////////////////

MARCDUINO_ACTION(MarchSequence, @0T11, ({
    FLD.selectSequence(LogicEngineRenderer::MARCH);
    RLD.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////

static char sMTFLDText[32];
static char sMBFLDText[32];
static char sMFLDText[65];
static char sMRLDText[64];

MARCDUINO_ACTION(TFLDScrollTextLeft, @1M, ({
    strcpy(sMTFLDText, Marcduino::getCommand());
    strcpy(sMFLDText, sMTFLDText);
    strcat(sMFLDText, "\n");
    strcat(sMFLDText, sMBFLDText);
    FLD.selectScrollTextLeft(sMFLDText, FLD.randomColor());
}))

////////////////

MARCDUINO_ACTION(BFLDScrollTextLeft, @2M, ({
    strcpy(sMBFLDText, Marcduino::getCommand());
    strcpy(sMFLDText, sMTFLDText);
    strcat(sMFLDText, "\n");
    strcat(sMFLDText, sMBFLDText);
    FLD.selectScrollTextLeft(sMFLDText, FLD.randomColor());
}))

////////////////

MARCDUINO_ACTION(RLDScrollTextLeft, @3M, ({
    strcpy(sMRLDText, Marcduino::getCommand());
    RLD.selectScrollTextLeft(sMRLDText, RLD.randomColor());
}))

////////////////

MARCDUINO_ACTION(TFLDTextLatin, @1P60, ({
    FLD.setEffectFontNum(0);
}))

////////////////

MARCDUINO_ACTION(BFLDTextLatin, @2P60, ({
    FLD.setEffectFontNum(0);
}))

////////////////

MARCDUINO_ACTION(RLDTextLatin, @3P60, ({
    RLD.setEffectFontNum(0);
}))

////////////////

MARCDUINO_ACTION(TFLDTextAurabesh, @1P61, ({
    FLD.setEffectFontNum(1);
}))

////////////////

MARCDUINO_ACTION(BFLDTextAurabesh, @2P61, ({
    FLD.setEffectFontNum(1);
}))

////////////////

MARCDUINO_ACTION(RLDTextAurabesh, @3P61, ({
    RLD.setEffectFontNum(1);
}))

////////////////
