#include "particle.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../LinAlg/tensor.h"
#include "../Stochastic/sample.h"


struct ParticleSim* particle_sim_init(int init_particle_count, int max_particle_count)
{
    struct ParticleSim* ps = (struct ParticleSim*) malloc(sizeof(struct ParticleSim));
    
    ps->count = init_particle_count;
    ps->max_count = max_particle_count;
    ps->t = 0;
    ps->tau = 0.01f;
    ps->max_vel = 100.f;
    ps->max_age = 1000.f;
    
    ps->links = (pstack_t**) malloc(sizeof(pstack_t*) * ps->max_count);
    for (int i = 0; i < ps->max_count; i++) ps->links[i] = stack_init();
    ps->link_data = stack_init();

    ps->pos = new_tensor_matrix(max_particle_count, 3, NULL);
    ps->vel = new_tensor_matrix(max_particle_count, 3, NULL);
    ps->color = new_tensor_matrix(max_particle_count, 3, NULL);
    ps->sizes = new_tensor_vector(max_particle_count, NULL);
    ps->energy = new_tensor_vector(max_particle_count, NULL);
    ps->age = new_tensor_vector(max_particle_count, NULL);

    tensor_fill_uniform(ps->pos, -50.f, 50.f);
    tensor_fill_uniform(ps->color, 0.5f, 1.f);
    tensor_fill_uniform(ps->sizes, 0.25f, 2.f);
    tensor_fill_uniform(ps->age, 0.f, 5.f);
    for (int i = 0; i < init_particle_count; i++) ps->energy->values[i] = 0.7f;

    return ps;
}

void particle_sim_free(struct ParticleSim* ps)
{
    tensor_free(ps->pos);
    tensor_free(ps->vel);
    tensor_free(ps->color);
    tensor_free(ps->sizes);
    tensor_free(ps->energy);
    tensor_free(ps->age);
    for (int i = 0; i < ps->max_count; i++) stack_free(ps->links[i], NULL);
    free(ps->links);
    stack_free(ps->link_data, free);
    free(ps);
}

// void particle_sim_rk4_step(struct ParticleSim* ps, void (*dydt)(struct ParticleSim*, tensor_t*))
// {
//     k1 = f(t, x)
//     k2 = f(t + h * 0.5f, x + h * k1 * 0.5f)
//     k3 = f(t + h * 0.5f, x + h * k2 * 0.5f)
//     k4 = f(t + h, x + h * k3)

//     x += (h/6) * (k1 + 2.f * k2 + 2.f * k3 + k4)
//     ps->t++;
// }

void particle_sim_euler_step(struct ParticleSim* ps, void (*dydt)(struct ParticleSim*, tensor_t*))
{
    tensor_t* tmp = tensor_copy_shape(ps->pos);

    // Position update
    tensor_scalar_mult(ps->vel, ps->tau, tmp);
    tensor_add(ps->pos, tmp, ps->pos);

    // Velocity update
    dydt(ps, tmp);
    tensor_scalar_mult(tmp, ps->tau, tmp);
    tensor_add(ps->vel, tmp, ps->vel);

    ps->t++;

    tensor_free(tmp);
}

void particle_sim_resolve_collisions(struct ParticleSim* ps)
{
    float normal[3];
    float impulse[3];
    float rel_vel[3];
    float dist_vec[3];
    float restitution = 1.f;
    float mass = 1.f;

    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->energy->values[i] <= 0.f) continue;

        for (int j = i+1; j < ps->max_count; j++)
        {
            if (ps->energy->values[j] <= 0.f) continue;

            float* p1 = &ps->pos->values[i*3];
            float* p2 = &ps->pos->values[j*3];
            float r1 = ps->sizes->values[i];
            float r2 = ps->sizes->values[j];

            dist_vec[0] = p2[0] - p1[0];
            dist_vec[1] = p2[1] - p1[1];
            dist_vec[2] = p2[2] - p1[2];

            float dist = sqrtf(
                dist_vec[0] * dist_vec[0] +
                dist_vec[1] * dist_vec[1] +
                dist_vec[2] * dist_vec[2]
            );

            if (dist < r1 + r2 && fabsf(dist) > 0.001f) // Collision!!!
            {
                // Resolve Position
                normal[0] = dist_vec[0] / dist;
                normal[1] = dist_vec[1] / dist;
                normal[2] = dist_vec[2] / dist;
                float overlap = (r1 + r2) - dist;

                p1[0] -= normal[0] * overlap * 0.5f;
                p1[1] -= normal[1] * overlap * 0.5f;
                p1[2] -= normal[2] * overlap * 0.5f;

                p2[0] += normal[0] * overlap * 0.5f;
                p2[1] += normal[1] * overlap * 0.5f;
                p2[2] += normal[2] * overlap * 0.5f;
            
                // Resolve Velocity
                float* v1 = &ps->vel->values[i*3];
                float* v2 = &ps->vel->values[j*3];

                rel_vel[0] = v2[0] - v1[0];
                rel_vel[1] = v2[1] - v1[1];
                rel_vel[2] = v2[2] - v1[2];
                float vel_dot = normal[0] * rel_vel[0] + normal[1] * rel_vel[1] + normal[2] * rel_vel[2];

                if (vel_dot <= 0.f)
                {
                    float mm = 1.f / mass + 1.f / mass;
                    float tmp = -(1.f + restitution) * vel_dot / mm;

                    impulse[0] = tmp * normal[0];
                    impulse[1] = tmp * normal[1];
                    impulse[2] = tmp * normal[2];

                    v1[0] -= impulse[0] / mass;
                    v1[1] -= impulse[1] / mass;
                    v1[2] -= impulse[2] / mass;

                    v2[0] += impulse[0] / mass;
                    v2[1] += impulse[1] / mass;
                    v2[2] += impulse[2] / mass;
                }
            }
        }
    }
}

void particle_sim_cap_vels(struct ParticleSim* ps)
{
    for (int i = 0; i < ps->max_count; i++)
    {
        float* vel = &ps->vel->values[i*3];
        float vel_norm = sqrtf(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);
        
        if (vel_norm > ps->max_vel)
        {
            vel[0] *= ps->max_vel / vel_norm;
            vel[1] *= ps->max_vel / vel_norm;
            vel[2] *= ps->max_vel / vel_norm;
        }
    }
}

void particle_sim_wrap_pos(struct ParticleSim* ps, float bounds)
{
    float* pos = ps->pos->values;

    for (int i = 0; i < ps->max_count * 3; i++)
    {
        if (pos[i] > bounds) pos[i] = bounds;
        if (pos[i] < -bounds) pos[i] = -bounds;
    }
}

void particle_sim_remove_dead_links(struct ParticleSim* ps, int particle_id)
{
    pstack_t* s = ps->links[particle_id];

    while (s->size)
    {
        link_t* link = stack_pop(s);

        stack_pop_elem(ps->link_data, link->data_elem);

        if (link->from == particle_id)
        {
            stack_pop_elem(ps->links[link->to], link->to_elem);
        }
        else
        {
            stack_pop_elem(ps->links[link->from], link->from_elem);
        }

        free(link);
    }
}

void particle_sim_update_energy(struct ParticleSim* ps)
{
    float base_weight = 0.05f;
    
    float sun_effect = -10.f;  // Barrier on z-axis
    float sun_max_effect = 50.f;
    float sun_weight = 0.2f;

    float* energy = ps->energy->values;
    tensor_scalar_add(ps->age, ps->tau, ps->age);

    for (int i = 0; i < ps->max_count; i++)
    {
        if (energy[i] <= 0.f) continue;

        float* pos = &ps->pos->values[i * 3];
        
        // Minus Energy
        energy[i] -= base_weight * ps->tau;

        // Plus Energy
        if (pos[2] > sun_effect)
        {
            float sun_energy = (pos[2] - sun_effect) / (sun_max_effect - sun_effect);
            if (sun_energy > 1.f) sun_energy = 1.f;
            energy[i] += sun_energy * sun_weight * ps->tau;
        }
        
        if (ps->age->values[i] >= ps->max_age) energy[i] = 0.f;  // Death from old-age.
        if (energy[i] <= 0.f)
        {
            ps->count--;
            particle_sim_remove_dead_links(ps, i);
        }
    }
    tensor_clip(ps->energy, 0.f, 1.f);
}

void particle_sim_duplicate_particles(struct ParticleSim* ps)
{
    if (ps->count == ps->max_count) return;

    float duplicate_thresh = 0.7f;
    float duplicate_cost = 0.1f;

    float* energy = ps->energy->values;

    for (int i = 0; i < ps->max_count; i++)
    {
        if (energy[i] <= duplicate_thresh) continue;
        int i3 = i*3;
        
        for (int j = 0; j < ps->max_count; j++)
        {
            if (energy[j] <= 0.f)
            {
                int j3 = j * 3;

                float* pos_parent = &ps->pos->values[i3];
                float* vel_parent = &ps->vel->values[i3];
                float* col_parent = &ps->color->values[i3];
                float parent_size = ps->sizes->values[i];

                float* pos_child = &ps->pos->values[j3];
                float* vel_child = &ps->vel->values[j3];
                float* col_child = &ps->color->values[j3];

                // Position
                pos_child[0] = pos_parent[0] + rand_uni(-parent_size, parent_size);
                pos_child[1] = pos_parent[1] + rand_uni(-parent_size, parent_size);
                pos_child[2] = pos_parent[2] + rand_uni(-parent_size, parent_size);

                // Velocity
                vel_child[0] = pos_child[0] - pos_parent[0];
                vel_child[1] = pos_child[1] - pos_parent[1];
                vel_child[2] = pos_child[2] - pos_parent[2];

                // Color
                for (int c = 0; c < 3; c++)
                {
                    col_child[c] = col_parent[c] + rand_uni(-0.1f, 0.1f);
                    if (col_child[c] > 1.f) col_child[c] = 1.f;
                    if (col_child[c] < 0.f) col_child[c] = 0.f;
                }

                ps->sizes->values[j] = parent_size + rand_uni(-0.1f, 0.1f);
                ps->age->values[j] = 0.f;
                
                energy[j] = 0.5f;

                energy[i] -= duplicate_cost;
                ps->count++;
                if (ps->count == ps->max_count) return;
                break;
            }
        }
    }
    tensor_clip(ps->sizes, 0.1f, 2.5f);
}

void particle_sim_run_genes(struct ParticleSim* ps)
{
    float expansion_speed = 0.5f;
    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->energy->values[i] <= 0.f) continue;
        
        if (ps->color->values[i*3+1] < 1.f && ps->color->values[i*3+2] < 1.f)
        {
            float delta = expansion_speed * ps->tau;
            ps->sizes->values[i] += ((ps->t + 1) / 100) % 2 ? -delta : delta;
        }
    }
}

void particle_sim_resolve_links(struct ParticleSim* ps)
{
    tensor_t* pos_delta = tensor_copy_shape(ps->pos);
    float expansion_speed = 0.01f;
    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->energy->values[i] <= 0.f) continue;

        struct stack_elem* current_elem = ps->links[i]->next;
        while (current_elem != NULL)
        {
            link_t* link = (link_t*) current_elem->data;
            current_elem = current_elem->next;
            
            int idx = (link->from == i) ? link->to : link->from;
            if (idx < i || ps->energy->values[idx] <= 0.f) continue;
    
            float size_i = ps->sizes->values[i];
            float target = ps->sizes->values[idx] + size_i;
    
            int i3 = i * 3;
            int l3 = idx * 3;
    
            float dl[3] = {
                ps->pos->values[l3]   - ps->pos->values[i3],
                ps->pos->values[l3+1] - ps->pos->values[i3+1],
                ps->pos->values[l3+2] - ps->pos->values[i3+2]
            };
    
            float dist  = sqrtf(dl[0]*dl[0] + dl[1]*dl[1] + dl[2]*dl[2]);
    
            float scale  = (target  - dist)  / dist  * 0.5f;
    
            for (int c = 0; c < 3; c++) {
                float f  = dl[c] * scale;
    
                pos_delta->values[i3+c] -= f;
                pos_delta->values[l3+c] += f;
            }
        }
    }
    tensor_add(ps->pos, pos_delta, ps->pos);
    tensor_free(pos_delta);
}

void particle_sim_distribute_energy(struct ParticleSim* ps)
{
    // This computes the graph laplacian! (kind of?)
    float energy_disipation_delta = 0.01f;
    tensor_t* energy_delta = tensor_copy_shape(ps->energy);
    
    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->energy->values[i] <= 0.f) continue;

        float node_count = 1.f;
        float mean = ps->energy->values[i];
        
        struct stack_elem* current_elem = ps->links[i]->next;
        while (current_elem != NULL)
        {
            link_t* link = (link_t*) current_elem->data;
            current_elem = current_elem->next;

            int idx = (link->from == i) ? link->to : link->from;
            if (ps->energy->values[idx] <= 0.f) continue;
            
            mean += ps->energy->values[idx];
            node_count++;
        }
        mean /= node_count;
        energy_delta->values[i] = (mean - ps->energy->values[i]) * ps->tau;
    }
    tensor_add(ps->energy, energy_delta, ps->energy);
    tensor_free(energy_delta);
}
