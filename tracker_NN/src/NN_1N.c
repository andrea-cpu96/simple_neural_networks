#include "NN_1N.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double sens_old[SENS_NUM] = {0};
double input[INPUTS_NUM] = {0};

void NN_1N_read_inputs(double data1, double data2)
{
    input[0] = 1.0; // bias input
    input[1] = data1;
    input[2] = data2;
}

void NN_1N_elab_output(neuron_handler_t *neuron_1)
{
    double sum = 0;

    for (int i = 0; i < INPUTS_NUM; i++)
    {
        sum += (input[i] * neuron_1->w[i]);
    }

    if (sum > 0)
    {
        neuron_1->out = 1;
    }
    else
    {
        neuron_1->out = 0;
    }
}

void NN_1N_create(neuron_handler_t *neuron)
{
    for (int i = 0; i < INPUTS_NUM; i++)
    {
        neuron->w[i] = 0.0;
    }
}

int NN_1N_learn(neuron_handler_t *neuron_1, NN_1N_handler_t *nn_handler, double mark)
{
    static double mark_prev = 0.0;
    static double mark_best = 0.0;
    int out_desired = 0;

    if (nn_handler->attempts_left > 0)
    {
        nn_handler->attempts_left--;
    }

    if (mark > mark_best)
    {
        mark_best = mark;
    }

    if (mark > mark_prev)
    {
        out_desired = neuron_1->out;
    }
    else
    {
        out_desired = !neuron_1->out;
    }

    for(int i = 0; i < INPUTS_NUM; i++)
    {
        neuron_1->w[i] += (out_desired - neuron_1->out) * input[i] * nn_handler->learning_fact;
    }

    mark_prev = mark;

    printf("Mark: %f; Best mark: %f; Attempts left: %d\n", mark, mark_best, nn_handler->attempts_left);

    if (nn_handler->attempts_left == 0)
    {
        if(mark >= mark_best)
        {
            nn_handler->attempts_left = nn_handler->attempts_num;
            mark_prev = 0.0;
            mark_best = 0.0;
            return 1; // Learning completed
        }
    }

    return 0;
}
