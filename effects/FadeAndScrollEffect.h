static inline int iabs(int a)
{
    return (a < 0) ? -a : a;
}

static bool LogicEffectFadeAndScroll(LogicEngineRenderer& r)
{
    enum Palette
    {
        kPaletteRGB,
        kPaletteRed,
        kPaletteGreen,
        kPaletteBlue,
        kPaletteWhite,
        kPaletteHalf,
        kPaletteLast = kPaletteHalf,
        kPaletteRandom
    };
    enum Direction
    {
        kForward,
        kBackward,
        kLastDirection = kBackward,
        kRandomDirection
    };
    enum Type
    {
        kFlat,
        kVertical,
        kHorizontal,
        kDiagonalLeft,
        kDiagonalRight,
        kHorizontalSymmetric,
        kVerticalSymmetric,
        kHyperbola,
        kDiamond,
        kCircle,
        kPlasma,
        kTypeLast = kPlasma,
        kRandomType
    };
    class FadeObject : public LogicEffectObject
    {
    public:
        int fs_speed = 1+random(10);
        int fs_zoom = 5+random(40);
        int fs_index = 0;
        Type fs_scroll_type = kRandomType;
        Direction fs_dir = kRandomDirection;
        Palette fs_palette = random(1) ? kPaletteRGB : random(1) ? kPaletteHalf : Palette(random(kPaletteLast));
        int* fs_height = NULL;
        CRGB* fs_lut = NULL;
        int fs_lut_len = 0;

        FadeObject(LogicEngineRenderer& r)
        {
            unsigned h = r.height();
            unsigned w = r.width();

            if (fs_palette == kPaletteRandom)
                fs_palette = Palette(random(int(kPaletteLast)));
            if (fs_dir == kRandomDirection)
                fs_dir = Direction(random(int(kLastDirection)));
            if (fs_scroll_type == kRandomType)
                fs_scroll_type = Type(random(int(kTypeLast)));
            switch (fs_palette)
            {
                case kPaletteRGB:
                    fs_lut = new CRGB[fs_lut_len = 1536];
                    for (int i = 0; i < 256; i++)
                    {
                        fs_lut[i].setRGB(255, i, 0);
                        fs_lut[i + 256].setRGB(255 - i, 255, 0);
                        fs_lut[i + 512].setRGB(0, 255, i);
                        fs_lut[i + 768].setRGB(0, 255 - i, 255);
                        fs_lut[i + 1024].setRGB(i, 0, 255);
                        fs_lut[i + 1280].setRGB(255, 0, 255 - i);
                    }
                    break;
                case kPaletteRed:
                    fs_lut = new CRGB[fs_lut_len = 512];
                    for (int i = 0; i < 256; i++)
                    {
                        fs_lut[i].setRGB(i, 0, 0);
                        fs_lut[i + 256].setRGB(255 - i, 0, 0);
                    }
                    break;
                case kPaletteGreen:
                    fs_lut = new CRGB[fs_lut_len = 512];
                    for (int i = 0; i < 256; i++)
                    {
                        fs_lut[i + 0].setRGB(0, i, 0);
                        fs_lut[i + 256].setRGB(0, 255 - i, 0);
                    }
                    break;
                case kPaletteBlue:
                    fs_lut = new CRGB[fs_lut_len = 512];
                    for (int i = 0; i < 256; i++)
                    {
                        fs_lut[i + 0].setRGB(0, 0, i);
                        fs_lut[i + 256].setRGB(0, 0, 255 - i);
                    }
                    break;
                case kPaletteWhite:
                    fs_lut = new CRGB[fs_lut_len = 512];
                    for (int i = 0; i < 256; i++)
                    {
                        fs_lut[i + 0].setRGB(i, i, i);
                        fs_lut[i + 256].setRGB(255 - i, 255 - i, 255 - i);
                    }
                    break;
                case kPaletteHalf:
                    fs_lut = new CRGB[fs_lut_len = 768];
                    for (int i = 0; i < 128; i++)
                    {
                        fs_lut[i + 0].setRGB(254 - 2 * i, 0, 127 - i);
                        fs_lut[i + 128].setRGB(i, 2 * i, 0);
                        fs_lut[i + 256].setRGB(127 - i, 254 - 2 * i, 0);
                        fs_lut[i + 384].setRGB(0, i, 2 * i);
                        fs_lut[i + 512].setRGB(0, 127 - i, 254 - 2 * i);
                        fs_lut[i + 640].setRGB(2 * i, 0, i);
                    }
                    break;
                case kPaletteRandom:
                    /* ignore */
                    break;
            }
            fs_height = new int[w * h];
            switch (fs_scroll_type)
            {
                case kFlat:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = 0;
                        }
                    }
                    break;
                case kVertical:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = y * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kHorizontal:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = x * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kDiagonalLeft:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = (x + y) * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kDiagonalRight:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = (x + h - y) * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kHorizontalSymmetric:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = iabs(x - w / 2) * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kVerticalSymmetric:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = iabs(y - h / 2) * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kHyperbola:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = iabs(y - h / 2) * iabs(x - w / 2) * fs_zoom / 10 % fs_lut_len;
                        }
                    }
                    break;
                case kDiamond:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = (iabs(y - h / 2) + iabs(x - w / 2)) * fs_zoom % fs_lut_len;
                        }
                    }
                    break;
                case kCircle:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = ((y - h / 2) * (y - h / 2) + (x - w / 2) * (x - w / 2)) * fs_zoom / 10 % fs_lut_len;
                        }
                    }
                    break;
                case kPlasma:
                    for (int x = 0; x < w; x++)
                    {
                        for (int y = 0; y < h; y++)
                        {
                            fs_index = y * w + x;
                            fs_height[fs_index] = (int)(((fs_lut_len / 2.0F) + fs_lut_len / 2.0 * sin((x / fs_zoom)) + (fs_lut_len / 2.0F) + fs_lut_len / 2.0 * sin((y / fs_zoom))) / 2.0);
                        }
                    }
                    break;
                case kRandomType:
                    /* ignore */
                    break;
            }
        }

        virtual ~FadeObject() override
        {
            if (fs_lut != NULL)
                delete[] fs_lut;
            if (fs_height != NULL)
                delete[] fs_height;
        }
    };

    if (r.hasEffectChanged())
    {
        r.setEffectObject(new FadeObject(r));
    }
    FadeObject* obj = (FadeObject*)r.getEffectObject();
    unsigned h = r.height();
    unsigned w = r.width();
    if (obj->fs_height == NULL || obj->fs_lut == NULL)
        return false;

    switch (obj->fs_dir)
    {
        case kForward:
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    obj->fs_index = y * w + x;
                    obj->fs_height[obj->fs_index] = obj->fs_height[obj->fs_index] + obj->fs_speed;
                    if (obj->fs_height[obj->fs_index] > obj->fs_lut_len - 1)
                        obj->fs_height[obj->fs_index] = 0; 
                    CRGB color = obj->fs_lut[obj->fs_height[obj->fs_index]];
                    r.setPixelRGB(x, y, color.r, color.g, color.b);
                }
            }
            break;
        case kBackward:
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    obj->fs_index = y * w + x;
                    obj->fs_height[obj->fs_index] = obj->fs_height[obj->fs_index] - obj->fs_speed;
                    if (obj->fs_height[obj->fs_index] < 0)
                        obj->fs_height[obj->fs_index] = obj->fs_lut_len - 1; 
                    CRGB color = obj->fs_lut[obj->fs_height[obj->fs_index]];
                    r.setPixelRGB(x, y, color.r, color.g, color.b);
                }
            }
            break;
        case kRandomDirection:
            /* ignore */
            break;
    }
    return true;
}
