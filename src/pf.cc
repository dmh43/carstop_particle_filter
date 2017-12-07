#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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

float* mat_vec_mul(float* mat, float* vec, int input_length, int result_length) {
    float* result = alloc_float(result_length);
    for (int matrix_row_index = 0; matrix_row_index < result_length; matrix_row_index++) {
        float* matrix_row = &mat[matrix_row_index * input_length];
        result[matrix_row_index] = inner_product(matrix_row, vec, input_length);
    }
    return result;
}

float calc_norm_squared_in(float* vec, float* mat, int vec_length, int result_length) {
    float* first_product_result = mat_vec_mul(mat, vec, vec_length, result_length);
    float result = inner_product(first_product_result, vec, result_length);
    free(first_product_result);
    return result;
}

float calc_unnormalized_importance_weight(float* error, float* measurement_noise_covariance_inv) {
    return exp(0.5 * calc_norm_squared_in(error,
                                          measurement_noise_covariance_inv,
                                          NUM_MEASUREMENT_VARIABLES,
                                          NUM_MEASUREMENT_VARIABLES));
}

void update_particle(systemModel model, float* current_state_estimate, float* current_measurement) {
    float* predicted_measurement = model.estimate_measurement(current_state_estimate);
    float* error = vec_subtract(current_measurement, predicted_measurement, NUM_MEASUREMENT_VARIABLES);
    free(error);
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
