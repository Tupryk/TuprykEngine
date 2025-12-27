struct feature
{
    float (*cost_func)(float*, int);
    void (*delta_cost_func)(float*, int, float*);
};
