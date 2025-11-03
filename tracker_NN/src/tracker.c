#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

#include "NN_1N.h"

#define EPOCHS_NUM 100
#define ATTEMPTS_NUM 1000
#define LEARNING_FACT 0.2

#define SLEEP_ENABLED 0

enum POS
{
  POS_DX = 0,
  POS_DX_UP,
  POS_UP,
  POS_SX_UP,
  POS_SX,
  POS_SX_DOWN,
  POS_DOWN,
  POS_DX_DOWN,
};

enum MOVE
{
  TURN_RIGHT = 0,
  TURN_LEFT,
};

typedef struct
{
  int x;
  int y;
} point_t;

point_t fish = {
    .x = 5,
    .y = 5,
};

point_t fish_s1 = {
    .x = 6,
    .y = 4,
};

point_t fish_s2 = {
    .x = 6,
    .y = 6,
};

point_t source = {
    .x = 5,
    .y = 2,
};

enum POS position;
int action;
neuron_handler_t neuron_1;
double sens[SENS_NUM] = {0};

static void evaluate_pos(void);
static void read_sensor_data(void);
static void execute_action(void);
static double output_evaluation(void);

static void sim_1(void);
static void sim_2(void);

static void draw_scene(point_t fish_s1, point_t fish_s2, point_t source);

int main(void)
{
  /* seed random used by NN_1N_learn */
  srand((unsigned)time(NULL));

  sim_1();

  for (int i = 0; i < 1; i++)
  {
    printf("\n");
    sim_2();
  }

  return 0;
}

static void sim_1(void)
{
  NN_1N_handler_t nn_handler = {
      .attempts_num = ATTEMPTS_NUM,
      .learning_fact = LEARNING_FACT,
      .attempts_left = ATTEMPTS_NUM,
  };

  NN_1N_create(&neuron_1);

  evaluate_pos();

  read_sensor_data();
  NN_1N_read_inputs(sens[0], sens[1]);

  for (int epoch = 0; epoch < EPOCHS_NUM; epoch++)
  {
    printf("Epoch %d/%d\n", epoch + 1, EPOCHS_NUM);
    source.x = rand() % 10 + 10;
    source.y = rand() % 10 + 10;

    while (1)
    {
#if (SLEEP_ENABLED == 1)
      sleep(1);
#endif

      NN_1N_elab_output(&neuron_1);
      execute_action();

      read_sensor_data();
      NN_1N_read_inputs(sens[0], sens[1]);

      double mark = output_evaluation();

      if (NN_1N_learn(&neuron_1, &nn_handler, mark))
      {
        printf("Learning completed!\n");
        printf("Final position: s1(%d, %d); s2(%d, %d)\n",
               fish_s1.x, fish_s1.y, fish_s2.x, fish_s2.y);
        printf("Final weights: ");
        for (int i = 0; i < INPUTS_NUM; i++)
        {
          printf("%f ", neuron_1.w[i]);
        }
        break;
      }
      evaluate_pos();
    }
  }
  evaluate_pos();
}

static void sim_2(void)
{
  static double mark_best = 0.0;
  double mark = 0.0;
  int flag = 0;

  fish_s1.x = 10;
  fish_s1.y = 9;

  fish_s2.x = 10;
  fish_s2.y = 11;

  fish.x = 9;
  fish.y = 10;

  source.x = rand() % 20;
  source.y = rand() % 20;

  evaluate_pos();

  read_sensor_data();
  NN_1N_read_inputs(sens[0], sens[1]);

  draw_scene(fish_s1, fish_s2, source);
  do
  {
    sleep(1);

    NN_1N_elab_output(&neuron_1);
    execute_action();

    read_sensor_data();
    NN_1N_read_inputs(sens[0], sens[1]);

    mark = output_evaluation();
    if (mark > mark_best)
    {
      mark_best = mark;
    }

    draw_scene(fish_s1, fish_s2, source);

    if ((mark < mark_best))
    {
      flag = 1;
    }

    if((flag == 1) && (mark == mark_best))
    {
      break;
    }
  } while (1);

  evaluate_pos();
}

static void evaluate_pos(void)
{
  if (fish_s1.x > fish_s2.x && fish_s1.y == fish_s2.y)
  {
    position = POS_DOWN;
    printf("POS_DOWN\n");
  }
  else if (fish_s1.x > fish_s2.x && fish_s1.y < fish_s2.y)
  {
    position = POS_DX_DOWN;
    printf("POS_DX_DOWN\n");
  }
  else if (fish_s1.x == fish_s2.x && fish_s1.y < fish_s2.y)
  {
    position = POS_DX;
    printf("POS_DX\n");
  }
  else if (fish_s1.x < fish_s2.x && fish_s1.y < fish_s2.y)
  {
    position = POS_DX_UP;
    printf("POS_DX_UP\n");
  }
  else if (fish_s1.x < fish_s2.x && fish_s1.y == fish_s2.y)
  {
    position = POS_UP;
    printf("POS_UP\n");
  }
  else if (fish_s1.x < fish_s2.x && fish_s1.y > fish_s2.y)
  {
    position = POS_SX_UP;
    printf("POS_SX_UP\n");
  }
  else if (fish_s1.x == fish_s2.x && fish_s1.y > fish_s2.y)
  {
    position = POS_SX;
    printf("POS_SX\n");
  }
  else if (fish_s1.x > fish_s2.x && fish_s1.y > fish_s2.y)
  {
    position = POS_SX_DOWN;
    printf("POS_SX_DOWN\n");
  }
}

static void read_sensor_data(void)
{
  sens[0] = (1.0 / (sqrt(pow(fish_s1.x - source.x, 2) + pow(fish_s1.y - source.y, 2)) + 1.0));
  sens[1] = (1.0 / (sqrt(pow(fish_s2.x - source.x, 2) + pow(fish_s2.y - source.y, 2)) + 1.0));
}

static void execute_action(void)
{
  if (neuron_1.out)
  {
    action = TURN_RIGHT;

    switch (position)
    {
    case POS_DX:
      fish_s1.y++;
      fish_s2.x--;
      position = POS_DX_DOWN;
      break;
    case POS_DX_DOWN:
      fish_s1.y++;
      fish_s2.x--;
      position = POS_DOWN;
      break;
    case POS_DOWN:
      fish_s1.x--;
      fish_s2.y--;
      position = POS_SX_DOWN;
      break;
    case POS_SX_DOWN:
      fish_s1.x--;
      fish_s2.y--;
      position = POS_SX;
      break;
    case POS_SX:
      fish_s1.y--;
      fish_s2.x++;
      position = POS_SX_UP;
      break;
    case POS_SX_UP:
      fish_s1.y--;
      fish_s2.x++;
      position = POS_UP;
      break;
    case POS_UP:
      fish_s1.x++;
      fish_s2.y++;
      position = POS_DX_UP;
      break;
    case POS_DX_UP:
      fish_s1.x++;
      fish_s2.y++;
      position = POS_DX;
      break;
    default:
      break;
    }
    printf("Action: %s; s1(%d, %d); s2(%d, %d)\n", "TURN_RIGHT", fish_s1.x, fish_s1.y, fish_s2.x, fish_s2.y);
  }
  else
  {
    action = TURN_LEFT;

    switch (position)
    {
    case POS_DX:
      fish_s1.x--;
      fish_s2.y--;
      position = POS_DX_UP;
      break;
    case POS_DX_UP:
      fish_s1.x--;
      fish_s2.y--;
      position = POS_UP;
      break;
    case POS_UP:
      fish_s1.y++;
      fish_s2.x--;
      position = POS_SX_UP;
      break;
    case POS_SX_UP:
      fish_s1.y++;
      fish_s2.x--;
      position = POS_SX;
      break;
    case POS_SX:
      fish_s1.x++;
      fish_s2.y++;
      position = POS_SX_DOWN;
      break;
    case POS_SX_DOWN:
      fish_s1.x++;
      fish_s2.y++;
      position = POS_DOWN;
      break;
    case POS_DOWN:
      fish_s1.y--;
      fish_s2.x++;
      position = POS_DX_DOWN;
      break;
    case POS_DX_DOWN:
      fish_s1.y--;
      fish_s2.x++;
      position = POS_DX;
      break;
    default:
      break;
    }
    printf("Action: %s; s1(%d, %d); s2(%d, %d)\n", "TURN_LEFT", fish_s1.x, fish_s1.y, fish_s2.x, fish_s2.y);
  }
}

static double output_evaluation(void)
{
  double mark = ((1.0 / (fabs(sens[0] - sens[1]) + 1.0)) + ((sens[0] + sens[1]) / 2.0)) * 1000.0;
  printf("Output evaluation mark: %f\n", mark);
  return mark;
}

static void draw_scene(point_t fish_s1, point_t fish_s2, point_t source)
{
  const int width = 20;
  const int height = 20;

  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      if (x == fish_s1.x && y == fish_s1.y)
      {
        printf("*");
      }
      else if (x == fish_s2.x && y == fish_s2.y)
      {
        printf("*");
      }
      else if (x == fish.x && y == fish.y)
      {
        printf("o");
      }
      else if (x == source.x && y == source.y)
      {
        printf("S");
      }
      else
      {
        printf(".");
      }
    }
    printf("\n");
  }
}
