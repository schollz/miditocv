#include "../../utils.h"

void test1() {
  int W, X;
  uint32_t Y;
  float Z;
  char input[] = "1_2_3_4.5";
  int parsed = parse_wxyz(input, &W, &X, &Y, &Z);
  if (!parsed) {
    printf("Failed to parse input\n");
  }
  printf("%d: W: %d, X: %d, Y: %u, Z: %f\n", parsed, W, X, Y, Z);
}

void test2() {
  int W, X;
  uint32_t Y;
  char input[] = "0_0_274204627";
  int parsed = parse_wxyz(input, &W, &X, &Y, NULL);
  if (!parsed) {
    printf("Failed to parse input\n");
  }
  printf("%d: W: %d, X: %d, Y: %u\n", parsed, W, X, Y);
}
int main() {
  test1();
  test2();
  return 0;
}