#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "pf.h"

int num_state_variables = 4;
int num_measurement_variables = 2;

float* step_process (float* y) {
    float* result = alloc_float(num_state_variables);
    result[0] = atan(y[0]) + y[1];
    result[1] = y[1] + 0.3 * y[2];
    result[2] = 0.92 * y[2] - 0.3 * y[3];
    result[3] = 0.3 * y[2] + 0.92 * y[3];
    return result;
}


float* estimate_measurement (float* y) {
    float* result = alloc_float(num_measurement_variables);
    result[0] = 0.1 * y[0] * y[0] * signbit(y[0]);
    result[1] = y[1] - y[2] + y[3];
    return result;
}

int main(int argc, char *argv[]) {
    char filename[] = "../out/pf.dat";
    std::ofstream outfile;
    outfile.open(filename);

    int num_measurements = 100;

    float initial_state[] = {0.0f, 0.0f, 0.0f, 0.0f};
    float init_state_cov_sqrt[] = {sqrt(1.0f), 0.0f, 0.0f, 0.0f,
                                   0.0f, sqrt(0.000001f), 0.0f, 0.0f,
                                   0.0f, 0.0f, sqrt(0.000001f), 0.0f,
                                   0.0f, 0.0f, 0.0f, sqrt(0.000001f)};
    float measurement_noise_cov_sqrt[] = {sqrt(0.1f), 0.0f,
                                          0.0f, sqrt(0.1f)};
    float measurement_noise_cov_inv[] = {10.0f, 0.0f,
                                         0.0f, 10.0f};
    float process_noise_cov_sqrt[] = {sqrt(0.01f), 0.0f, 0.0f, 0.0f,
                                      0.0f, sqrt(0.01f), 0.0f, 0.0f,
                                      0.0f, 0.0f, sqrt(0.01f), 0.0f,
                                      0.0f, 0.0f, 0.0f, sqrt(0.01f)};
    systemModel model = {num_state_variables,
                         num_measurement_variables,
                         initial_state,
                         init_state_cov_sqrt,
                         measurement_noise_cov_sqrt,
                         measurement_noise_cov_inv,
                         process_noise_cov_sqrt,
                         estimate_measurement,
                         step_process};

    float* true_state = alloc_float((num_measurements + 1) * num_state_variables);
    float* measurements = alloc_float(num_measurements * num_measurement_variables);

    memcpy(&true_state[0], initial_state, num_state_variables * sizeof(float));
    add_noise(&true_state[0], init_state_cov_sqrt, num_state_variables);
    for (int t = 0; t < num_measurements; t++) {
        memcpy(&true_state[(t + 1) * num_state_variables],
               step_process(&true_state[t * num_state_variables]),
               num_state_variables * sizeof(float));
        add_noise(&true_state[(t + 1) * num_state_variables],
                  process_noise_cov_sqrt,
                  num_state_variables);
        memcpy(&measurements[t * num_measurement_variables],
               estimate_measurement(&true_state[t * num_state_variables]),
               num_measurement_variables * sizeof(float));
        add_noise(&measurements[t * num_measurement_variables],
                  measurement_noise_cov_sqrt,
                  num_measurement_variables);
    }

    int num_particles = 200;
    float* pf_result = pf(measurements, model, num_measurements, num_particles);


    for (int t = 0; t < num_measurements; t++)
        outfile << t << " " << true_state[t * num_state_variables + 0] << " " << true_state[t * num_state_variables + 1] << " " << pf_result[t * num_state_variables + 0] << " " << pf_result[t * num_state_variables + 1] << std::endl;

    outfile.close();
    return 0;
}
