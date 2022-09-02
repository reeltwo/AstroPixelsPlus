////////////////

MARCDUINO_ACTION(FPSINormalSequence, @1P1, ({
    frontPSI.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(FPSIFlashSequence, @1P2, ({
    frontPSI.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(FPSIAlarmSequence, @1P3, ({
    frontPSI.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(FPSIFailureSequence, @1P4, ({
    frontPSI.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(FPSIScreamLogicsSequence, @1P5, ({
    frontPSI.selectSequence(LogicEngineRenderer::REDALERT);
}))

////////////////

MARCDUINO_ACTION(FPSILeiaLogicsSequence, @1P6, ({
    frontPSI.selectSequence(LogicEngineRenderer::LEIA);
}))

////////////////

MARCDUINO_ACTION(FPSIMarchSequence, @1P11, ({
    frontPSI.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////

MARCDUINO_ACTION(RPSINormalSequence, @2P1, ({
    rearPSI.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(RPSIFlashSequence, @2P2, ({
    rearPSI.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(RPSIAlarmSequence, @2P3, ({
    rearPSI.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(RPSIFailureSequence, @2P4, ({
    rearPSI.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(RPSIScreamLogicsSequence, @2P5, ({
    rearPSI.selectSequence(LogicEngineRenderer::REDALERT);
}))

////////////////

MARCDUINO_ACTION(RPSILeiaLogicsSequence, @2P6, ({
    rearPSI.selectSequence(LogicEngineRenderer::LEIA);
}))

////////////////

MARCDUINO_ACTION(RPSIMarchSequence, @2P11, ({
    rearPSI.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////

MARCDUINO_ACTION(PSINormalSequence, @0P1, ({
    frontPSI.selectSequence(LogicEngineRenderer::NORMAL);
    rearPSI.selectSequence(LogicEngineRenderer::NORMAL);
}))

////////////////

MARCDUINO_ACTION(PSIFlashSequence, @0P2, ({
    frontPSI.selectSequence(LogicEngineRenderer::FLASHCOLOR);
    rearPSI.selectSequence(LogicEngineRenderer::FLASHCOLOR);
}))

////////////////

MARCDUINO_ACTION(PSIAlarmSequence, @0P3, ({
    frontPSI.selectSequence(LogicEngineRenderer::ALARM);
    rearPSI.selectSequence(LogicEngineRenderer::ALARM);
}))

////////////////

MARCDUINO_ACTION(PSIFailureSequence, @0P4, ({
    frontPSI.selectSequence(LogicEngineRenderer::FAILURE);
    rearPSI.selectSequence(LogicEngineRenderer::FAILURE);
}))

////////////////

MARCDUINO_ACTION(PSIScreamLogicsSequence, @0P5, ({
    frontPSI.selectSequence(LogicEngineRenderer::REDALERT);
    rearPSI.selectSequence(LogicEngineRenderer::REDALERT);
}))

////////////////

MARCDUINO_ACTION(PSILeiaLogicsSequence, @0P6, ({
    frontPSI.selectSequence(LogicEngineRenderer::LEIA);
    rearPSI.selectSequence(LogicEngineRenderer::LEIA);
}))

////////////////

MARCDUINO_ACTION(PSIMarchSequence, @0P11, ({
    frontPSI.selectSequence(LogicEngineRenderer::MARCH);
    rearPSI.selectSequence(LogicEngineRenderer::MARCH);
}))

////////////////
