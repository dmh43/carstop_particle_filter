#define len(coll_ptr) (sizeof(coll_ptr)/sizeof(coll_ptr[0]))
#define alloc_float(num_elems) ((float*) malloc(num_elems * sizeof(float)))
#define NUM_STATE_VARIABLES 2
#define NUM_MEASUREMENT_VARIABLES 2

typedef float* (*float_p_float_p_fptr)(float*);

typedef struct systemModel {
    int state_dimension;
    float initial_state[NUM_STATE_VARIABLES];
    float_p_float_p_fptr estimate_measurement;
    float_p_float_p_fptr estimate_odometry;
} systemModel;

float inner_product(float* vec1, float* vec2, int length);

float* vec_subtract(float* vec1, float* vec2, int length);

float* mat_vec_mul(float* mat, float* vec, int input_length, int result_length);

float calc_norm_squared_in(float* vec, float* mat, int vec_length, int result_length);

float calc_unnormalized_importance_weight(float* error, float* measurement_noise_covariance_inv);

void update_particle(systemModel model, float* current_state_estimate, float* current_measurement);

float* initialize_particles(float* initial_state, float num_particles);

void pf(float* result, float* measurements, systemModel model, int num_particles);
