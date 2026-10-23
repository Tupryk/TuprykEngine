#include "utils.h"


void cppn(float* pos, float* out, agent_t* agent)
{
    // Evaluating selected NEAT neural network from a population
    float px = pos[0]; float py = pos[1]; float pz = pos[2];
    
    float r = sqrt(px*px + py*py + pz*pz);
    float theta = atan2(py, px);
    float phi = atan2(pz, hypot(px, py));
    
    nn_input[0] = r;
    nn_input[1] = cos(theta);
    nn_input[2] = cos(phi);  // ???
    nn_input[3] = px; nn_input[4] = py; nn_input[5] = pz;
    
    float net_out[out_dim];
    agent_feed(agent, in_dim, out_dim, nn_input, net_out);

    int max_id = 1;
    for (int i = 2; i < 5; i++)
    {
        if (net_out[max_id] < net_out[i]) max_id = i;
    }

    out[0] = net_out[0];  // Exists
    out[6] = 0.f;  // Fixed
    
    switch(max_id-1)
    {
        case 0:
        {
            // Green: Actuated at 20%
            out[1] = 1000.f;  // Stiffness
            out[2] = 0.2f;  // CTE
            out[3] = 0.f; out[4] = 1.f; out[5] = 0.f;  // Color
            break;
        }
        case 1:
        {
            // Light Blue: Soft and pasive
            out[1] = 50.f;  // Stiffness
            out[2] = 0.f;  // CTE
            out[3] = 0.f; out[4] = 1.f; out[5] = 1.f;  // Color
            break;
        }
        case 2:
        {
            // Green: Actuated at 20% (couter to Green)
            out[1] = 1000.f;  // Stiffness
            out[2] = -0.2f;  // CTE
            out[3] = 1.f; out[4] = 0.f; out[5] = 0.f;  // Color
            break;
        }
        case 3:
        {
            // Dark Blue: Hard and pasive
            out[1] = 1000.f;  // Stiffness
            out[2] = 0.f;  // CTE
            out[3] = 0.f; out[4] = 0.f; out[5] = 1.f;  // Color
            break;
        }
    }
}
