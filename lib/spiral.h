#ifndef LIB_SPIRAL_H
#define LIB_SPIRAL_H

#define NUM_POINTS 1024
#define N_TURNS 4
#define MAX_RADIUS 0.5f

// Helper function to calculate the square spiral coordinates
void square_spiral_three_turns_flipped_x(float *t_values, float *x_values,
                                         float *y_values, int n_points) {
  for (int i = 0; i < n_points; i++) {
    float t = t_values[i];
    int layer = (int)(t * N_TURNS * 4);  // Determine which "side" of the spiral
    float pos_in_layer =
        fmodf(t * N_TURNS * 4, 1.0f);  // Position within the layer (0 to 1)
    float radius = MAX_RADIUS * (1.0f - t);

    if (layer % 4 == 0) {           // Right side, moving up
      x_values[i] = 0.5f - radius;  // Flip x-axis
      y_values[i] = 0.5f + radius * (2.0f * pos_in_layer - 1.0f);
    } else if (layer % 4 == 1) {  // Top side, moving left
      x_values[i] =
          0.5f - radius * (1.0f - 2.0f * pos_in_layer);  // Flip x-axis
      y_values[i] = 0.5f + radius;
    } else if (layer % 4 == 2) {    // Left side, moving down
      x_values[i] = 0.5f + radius;  // Flip x-axis
      y_values[i] = 0.5f + radius * (1.0f - 2.0f * pos_in_layer);
    } else {  // Bottom side, moving right
      x_values[i] =
          0.5f + radius * (1.0f - 2.0f * pos_in_layer);  // Flip x-axis
      y_values[i] = 0.5f - radius;
    }
  }
}

// Main function to map input (0 to 1023) to x-y coordinates along the spiral
void spiral_coordinate(int input_value, float *x, float *y) {
  if (input_value < 0 || input_value > 1023) {
    fprintf(stderr, "Input value must be between 0 and 1023.\n");
    exit(EXIT_FAILURE);
  }

  // Normalize the input to a value between 0 and 1
  float t_normalized = input_value / 1023.0f;

  // Generate t values
  float t_values[NUM_POINTS];
  for (int i = 0; i < NUM_POINTS; i++) {
    t_values[i] = i / (float)(NUM_POINTS - 1);
  }

  // Generate spiral coordinates
  float x_values[NUM_POINTS], y_values[NUM_POINTS];
  square_spiral_three_turns_flipped_x(t_values, x_values, y_values, NUM_POINTS);

  // Find the closest point corresponding to the input value
  int index = (int)(t_normalized * 1023);
  *x = x_values[index];
  *y = y_values[index];
}

#endif