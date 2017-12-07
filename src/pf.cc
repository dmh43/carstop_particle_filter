#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "pf.h"

float inner_product(float* vec1, float* vec2, int length) {
    float acc = 0;
    for (int vector_index = 0; vector_index < length; vector_index++) {
        acc += vec1[vector_index] * vec2[vector_index];
    }
    return acc;
}

float* vec_subtract(float* vec1, float* vec2, int length) {
    float* result = alloc_float(length);
    for (int vector_index = 0; vector_index < length; vector_index++) {
        result[vector_index] = vec1[vector_index] - vec2[vector_index];
    }
    return result;
}

void vec_mutate_divide(float* vec, float divisor, int length) {
    for (int vector_index = 0; vector_index < length; vector_index++) {
        vec[vector_index] = vec[vector_index] / divisor;
    }
}

float sum(float* vec, int length) {
    float acc = 0;
    for (int vector_index = 0; vector_index < length; vector_index++) {
        acc += vec[vector_index];
    }
    return acc;
}

float* cumsum(float* vec, int length) {
    float* result = alloc_float(length);
    for (int vector_index = 0; vector_index < length; vector_index++) {
        if (vector_index == 0) {
            result[vector_index] = vec[vector_index];
        } else {
            result[vector_index] = result[vector_index - 1] + vec[vector_index];
        }
    }
    return result;
}

float* mat_vec_mul(float* mat, float* vec, int input_length, int result_length) {
    float* result = alloc_float(result_length);
    for (int matrix_row_index = 0; matrix_row_index < result_length; matrix_row_index++) {
        float* matrix_row = &mat[matrix_row_index * input_length];
        result[matrix_row_index] = inner_product(matrix_row, vec, input_length);
    }
    return result;
}

float calc_norm_squared_in(float* vec, float* mat, int vec_length) {
    float* first_product_result = mat_vec_mul(mat, vec, vec_length, vec_length);
    float result = inner_product(first_product_result, vec, vec_length);
    free(first_product_result);
    return result;
}

void vec_mutate_add(float* vec1, float* vec2, int length) {
    for (int vector_index = 0; vector_index < length; vector_index++) {
        vec1[vector_index] += vec2[vector_index];
    }
}

float normal_rand() {
    float u = ((float) rand() / (RAND_MAX)) * 2 - 1;
    float v = ((float) rand() / (RAND_MAX)) * 2 - 1;
    float r = u * u + v * v;
    if (r == 0 || r > 1) return normal_rand();
    float c = sqrt(-2 * log(r) / r);
    return u * c;
}

float* random_normal_vector(int length) {
    float* result = alloc_float(length);
    for (int vector_index = 0; vector_index < length; vector_index++) {
        result[vector_index] = normal_rand();
    }
    return result;
}

void add_noise(float* vec, float* noise_covariance_sqrt, int length) {
    float* random = random_normal_vector(length);
    float* first_result = mat_vec_mul(noise_covariance_sqrt, random, length, length);
    vec_mutate_add(vec, first_result, length);
    free(first_result);
    free(random);
}

float calc_unnormalized_importance_weight(systemModel model, float* current_state_estimate, float* current_measurement) {
    float* predicted_measurement = model.estimate_measurement(current_state_estimate);
    float* error = vec_subtract(current_measurement, predicted_measurement, model.num_measurement_variables);
    float unnormalized_weight = exp(0.5 * calc_norm_squared_in(error,
                                                               model.measurement_noise_covariance_inv,
                                                               model.num_measurement_variables));
    free(error);
    return unnormalized_weight;
}
}

float* initialize_particles(float* initial_state, float num_particles) {
    float* particles = alloc_float(num_particles * NUM_STATE_VARIABLES);
    for (int particle_index = 0; particle_index < num_particles; particle_index++) {
        memcpy(&particles[particle_index], initial_state, NUM_STATE_VARIABLES * sizeof(float));
    }
    return particles;
}

void pf(float* result, float* measurements, systemModel model, int num_particles) {
    int num_samples = len(measurements);
    float* particles = initialize_particles(model.initial_state, num_particles);
    for (int sample_index = 0; sample_index < num_samples; sample_index++) {
        float* current_measurement = &measurements[sample_index * NUM_MEASUREMENT_VARIABLES];
        for (int particle_index = 0; particle_index < num_particles; particle_index++) {
            float* current_state_estimate = &particles[particle_index];
            update_particle(model, current_state_estimate, current_measurement);
        }
    }
    free(particles);
}
