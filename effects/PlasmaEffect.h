static bool LogicEffectPlasma(LogicEngineRenderer& r)
{
    class PlasmaObject : public LogicEffectObject
    {
    public:
        int plasma_step_width = 1;
        int plasma_cell_size_x = 3;
        int plasma_cell_size_y = 3;
        uint8_t plasma_lut[1537][3];
        float plasma_counter = 0.0F;

        PlasmaObject()
        {
            int i;
            for (i = 0; i < 256; i++)
            {
                plasma_lut[i + 0][0] = 255;
                plasma_lut[i + 0][1] = i;
                plasma_lut[i + 0][2] = 0;
            }
            for (i = 0; i < 256; i++)
            {
                plasma_lut[i + 256][0] = 255 - i;
                plasma_lut[i + 256][1] = 255;
                plasma_lut[i + 256][2] = 0;
            }
            for (i = 0; i < 256; i++)
            {
                plasma_lut[i + 512][0] = 0;
                plasma_lut[i + 512][1] = 255;
                plasma_lut[i + 512][2] = i;
            }
            for (i = 0; i < 256; i++)
            {
                plasma_lut[i + 768][0] = 0;
                plasma_lut[i + 768][1] = 255 - i;
                plasma_lut[i + 768][2] = 255;
            }
            for (i = 0; i < 256; i++)
            {
                plasma_lut[i + 1024][0] = i;
                plasma_lut[i + 1024][1] = 0;
                plasma_lut[i + 1024][2] = 255;
            }
            for (i = 0; i < 256; i++)
            {
                plasma_lut[i + 1280][0] = 255;
                plasma_lut[i + 1280][1] = 0;
                plasma_lut[i + 1280][2] = 255 - i;
            }
        }
    };

    if (r.hasEffectChanged())
    {
        r.setEffectObject(new PlasmaObject());
        r.clear();
    }
    PlasmaObject* obj = (PlasmaObject*)r.getEffectObject();
    unsigned h = r.height();
    unsigned w = r.width();

    obj->plasma_counter += obj->plasma_step_width / 10.0;
    double calc1 = sin((obj->plasma_counter * 0.006));
    double calc2 = sin((obj->plasma_counter * -0.06));
    double xc = 25.0;
    for (int x = 0; x < w; x++)
    {
        xc += (obj->plasma_cell_size_x / 10.0);
        double yc = 25.0;
        double s1 = 768.0 + 768.0 * sin(xc) * calc1;
        for (int y = 0; y < h; y++)
        {
            yc += (obj->plasma_cell_size_y / 10.0);
            double s2 = 768.0 + 768.0 * sin(yc) * calc2;
            double s3 = 768.0 + 768.0 * sin((xc + yc + (obj->plasma_counter / 10.0)) / 2.0);
            int pixel = (int)((s1 + s2 + s3) / 3.0);
            r.setPixelRGB(
                x, y,
                obj->plasma_lut[pixel][0],
                obj->plasma_lut[pixel][1],
                obj->plasma_lut[pixel][2]);
        }
    }
    return true;
}
