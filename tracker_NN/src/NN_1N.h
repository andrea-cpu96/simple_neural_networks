#define SENS_NUM 2
#define INPUTS_NUM (SENS_NUM + 1)
#define NEURONS_NUM 2

#define RAND_W() (2 * ((double)rand() / (double)RAND_MAX) - 1) // Range [-1, 1]

typedef struct
{
    double w[INPUTS_NUM];
    int out;
} neuron_handler_t;

typedef struct
{
    const double learning_fact;
    const int attempts_num;
    int attempts_left;
} NN_1N_handler_t;

void NN_1N_create(neuron_handler_t *neuron);
void NN_1N_read_inputs(double data1, double data2);
void NN_1N_elab_output(neuron_handler_t *neuron);
int NN_1N_learn(neuron_handler_t *neuron_1, NN_1N_handler_t *nn_handler, double mark);
