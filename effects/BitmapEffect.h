static bool LogicEffectBitmap(LogicEngineRenderer& r)
{
    return false;
    if (r.hasEffectChanged())
    {
        r.setEffectDelay(25);
        r.calculateAllColors();
    }
    r.updateDisplay();
    unsigned h = r.height();
    unsigned w = r.width();
    for (unsigned y = 0; y < h; y++)
    {
        for (unsigned x = 0; x < w; x++)
        {
            r.setPixelRGB(x, y, leds[x+y*w]);
        }
    }
    return true;
}
