static bool LogicEffectFractal(LogicEngineRenderer& r)
{
    class FractalObject : public LogicEffectObject
    {
    public:
        int* color;
        int* level;
        int grow_size = 1+random(3);
        bool distortion = true;
        int dist_strength = 10+random(30);

        FractalObject(LogicEngineRenderer& r)
        {
            unsigned h = r.height();
            unsigned w = r.width();
            color = new int[w*h];
            level = new int[w*h];
            memset(color, '\0', sizeof(color[0]) * w * h);
            memset(level, '\0', sizeof(level[0]) * w * h);
            for (int x = 0; x < w; x++)
            {
                color[x + (h / 3 * 1)*w] = 1;
                color[x + (h / 3 * 2)*w] = 3;
            }
            for (int y = 0; y < h; y++)
            {
                color[w / 3 * 1 + y*w] = 2;
                color[w / 3 * 2 + y*w] = 3;
            }
        }

        virtual ~FractalObject() override
        {
            delete[] color;
            delete[] level;
        }
    };

    if (r.hasEffectChanged())
    {
        r.setEffectObject(new FractalObject(r));
    }
    FractalObject* obj = (FractalObject*)r.getEffectObject();
    unsigned h = r.height();
    unsigned w = r.width();

    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            int pos_x = 0;
            int pos_y = 0;
            while (pos_x == 0 && pos_y == 0)
            {
                pos_x = random(3);
                pos_y = random(3);
            }
            pos_x = x - 1 + pos_x;
            pos_y = y - 1 + pos_y;
            if (pos_x < 0)
                pos_x = w - 1;
            if (pos_y < 0)
                pos_y = h - 1;
            if (pos_x > w - 1)
                pos_x = 0;
            if (pos_y > h - 1)
                pos_y = 0;
            if (obj->color[x + y*w] == 0 && obj->color[pos_x + pos_y*w] != 0
                && obj->level[pos_x + pos_y*w] < obj->grow_size)
            {
                obj->color[x + y*w] = obj->color[pos_x + pos_y*w];
                obj->level[x + y*w] = obj->level[pos_x + pos_y*w] + 1;
            }
            else
            {
                switch (obj->color[x + y*w])
                {
                    case 1:
                        if (obj->color[pos_x + pos_y*w] == 3)
                        {
                            obj->level[x + y*w] = obj->level[x + y*w] - 1;
                            obj->level[pos_x + pos_y*w] = obj->level[pos_x + pos_y*w] + 1;
                        }
                        if (obj->color[pos_x + pos_y*w] == 2)
                        {
                            obj->level[x + y*w] = obj->level[x + y*w] + 1;
                            obj->level[pos_x + pos_y*w] = obj->level[pos_x + pos_y*w] - 1;
                        }
                        break;
                    case 2:
                        if (obj->color[pos_x + pos_y*w] == 1)
                        {
                            obj->level[x + y*w] = obj->level[x + y*w] - 1;
                            obj->level[pos_x + pos_y*w] = obj->level[pos_x + pos_y*w] + 1;
                        }
                        if (obj->color[pos_x + pos_y*w] == 3)
                        {
                            obj->level[x + y*w] = obj->level[x + y*w] + 1;
                            obj->level[pos_x + pos_y*w] = obj->level[pos_x + pos_y*w] - 1;
                        }
                        break;
                    case 3:
                        if (obj->color[pos_x + pos_y*w] == 2)
                        {
                            obj->level[x + y*w] = obj->level[x + y*w] - 1;
                            obj->level[pos_x + pos_y*w] = obj->level[pos_x + pos_y*w] + 1;
                        }
                        if (obj->color[pos_x + pos_y*w] == 1)
                        {
                            obj->level[x + y*w] = obj->level[x + y*w] + 1;
                            obj->level[pos_x + pos_y*w] = obj->level[pos_x + pos_y*w] - 1;
                        }
                        break;
                }
                if (obj->level[x + y*w] < 0)
                    obj->level[x + y*w] = 0;
                if (obj->level[x + y*w] > obj->grow_size)
                {
                    obj->color[x + y*w] = obj->color[pos_x + pos_y*w];
                    obj->level[x + y*w] = 0;
                }
            }
        }
    }
    for (int x = 0; x < w; x++)
    {
        for (int y = 0; y < h; y++)
        {
            if (obj->distortion)
            {
                int r = random(10000);
                if (r < obj->dist_strength)
                    obj->color[x + y*w] = random(3) + 1;
            }
            CRGB temp_color;
            temp_color.r = 0;
            temp_color.g = 0;
            temp_color.b = 0;
            switch (obj->color[x + y*w])
            {
                case 1:
                    temp_color.r = 255;
                    break;
                case 2:
                    temp_color.g = 255;
                    break;
                case 3:
                    temp_color.b = 255;
                    break;
            }
            r.setPixelRGB(x, y, temp_color.r, temp_color.g, temp_color.b);
        }
    }
    return true;
}
