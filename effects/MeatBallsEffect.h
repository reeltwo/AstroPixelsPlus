static double randomDouble()
{
    return (random(10000)/10000.0);
}

static bool LogicEffectMetaBalls(LogicEngineRenderer& r)
{
    class MetaBallsObject : public LogicEffectObject
    {
    public:
        int mb_r_start = 255;
        int mb_g_start = 0;
        int mb_b_start = 255;
        int mb_dia = 1+random(24);
        int mb_number = 1+random(4);
        int mb_speed = 1+random(4);
        bool mb_random_color = true;
        int mb_random_time = 25;
        int* mb_vx;
        int* mb_vy;
        int* mb_px;
        int* mb_py;
        int* mb_dx;
        int* mb_dy;
        int mb_counter = 0;

        MetaBallsObject(LogicEngineRenderer& r)
        {
            unsigned h = r.height();
            unsigned w = r.width();
            mb_px = new int[mb_number];
            mb_py = new int[mb_number];
            mb_dx = new int[mb_number];
            mb_dy = new int[mb_number];
            mb_vx = new int[mb_number * w];
            mb_vy = new int[mb_number * h];
            for (int j = 0; j < mb_number; j++)
            {
                mb_px[j] = (int)(randomDouble() * w);
                mb_py[j] = (int)(randomDouble() * h);
                mb_dx[j] = (randomDouble() < 0.5) ? -1 : 1;
                mb_dy[j] = (randomDouble() < 0.5) ? -1 : 1;
            }
            Serial.println(randomDouble());
            Serial.println(randomDouble());
            Serial.println(randomDouble());
            Serial.println(randomDouble());
            Serial.println(randomDouble());
        }

        virtual ~MetaBallsObject() override
        {
            delete[] mb_px;
            delete[] mb_py;
            delete[] mb_dx;
            delete[] mb_dy;
            delete[] mb_vx;
            delete[] mb_vy;
        }
    };

    if (r.hasEffectChanged())
    {
        r.setEffectObject(new MetaBallsObject(r));
    }
    MetaBallsObject* obj = (MetaBallsObject*)r.getEffectObject();
    unsigned h = r.height();
    unsigned w = r.width();

    int mb_number = obj->mb_number;
    int* mb_vx = obj->mb_vx;
    int* mb_vy = obj->mb_vy;
    int* mb_px = obj->mb_px;
    int* mb_py = obj->mb_py;
    int* mb_dx = obj->mb_dx;
    int* mb_dy = obj->mb_dy;

    if (obj->mb_random_color)
    {
        if (obj->mb_counter > obj->mb_random_time)
        {
            obj->mb_r_start = random(127);
            obj->mb_g_start = random(127);
            obj->mb_b_start = random(127);
            obj->mb_counter = 0;
        }
        else
        {
            obj->mb_counter++;
        }
    }
    for (int i = 0; i < mb_number; i++)
    {
        mb_px[i] = mb_px[i] + obj->mb_speed * mb_dx[i];
        mb_py[i] = mb_py[i] + obj->mb_speed * mb_dy[i];
        if (mb_px[i] < 0)
            mb_dx[i] = 1;
        else if (mb_px[i] > w)
            mb_dx[i] = -1;

        if (mb_py[i] < 0)
            mb_dy[i] = 1;
        else if (mb_py[i] > h)
            mb_dy[i] = -1;
        for (int x = 0; x < w; x++)
            mb_vx[i*mb_number+x] = (mb_px[i] - x) * (mb_px[i] - x);
        for (int y = 0; y < h; y++)
            mb_vy[i*mb_number+y] = (mb_py[i] - y) * (mb_py[i] - y);
    }
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int R = 0;
            int G = 0;
            int B = 0;
            for (int i = 0; i < mb_number; i++)
            {
                double distance = (mb_vx[i*mb_number+x] + mb_vy[i*mb_number+y] + 1);
                R += (int)(obj->mb_dia / distance * obj->mb_r_start);
                G += (int)(obj->mb_dia / distance * obj->mb_g_start);
                B += (int)(obj->mb_dia / distance * obj->mb_b_start);
            }
            if (R > 255)
                R = 255;
            if (G > 255)
                G = 255;
            if (B > 255)
                B = 255;
            r.setPixelRGB(x, y, R*0.5, G*0.5, B*0.5);
        }
    }
    return true;
}
