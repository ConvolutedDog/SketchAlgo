/*
  compile: gcc -o CountSketch.exe CountSketch.c -std=c99
  run: CountSketch.exe [Memory total usage] [d] [w] [big_flow_threshold_num]
*/
#include "BOBHash32.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


/************************************************/
// config
#define max_size 9999999 // Max size of packets
#define hash_num 10      // Num of hash functions

/*array width*/
int w = 0;

/*array height*/
int d = 0;

/*bit width of map*/
int bitwidth = 0;

/*map[d][w]*/
int map[hash_num][max_size];

/*map used to seek max usage ratio*/
int map_seek_max_usage_ratio[hash_num][max_size];

/*hash_value[d]*/
int hash_value[hash_num];

/*AddOrSub_value[d]*/
int AddOrSub_value[hash_num];

/*global num[]*/
int num[hash_num];

/*global mid_num*/
int mid_num;

/*global row count of txt*/
int row_count = 0;

/*26 char array*/
char strArray[max_size][27];

/*pkt_num1[] by hard calculation*/
int pkt_num1[max_size];

/*pkt_num1[] by soft calculation*/
int pkt_num2[max_size];

/*gloabal row_count after intergrate*/
int last_row_count = 0;

/*tmp array used in quick sort*/
int tmp_array[hash_num];
/************************************************/

/*Read from txt, every row of item in strArray[i]*/
void readtxt(char *str) {
  FILE *f;
  if (NULL == (f = fopen(str, "r"))) {
    fprintf(stderr, "Can not open txt file!\n");
    exit(0);
  }

  fscanf(f, "%d\n", &row_count);

  for (int i = 0; i < row_count; i++) {
    fscanf(f, "%s\n", &strArray[i]);
  }

  fclose(f);
}

/*Quick sort*/
void quickSort(int *arr, int startPos, int endPos) {
  int i, j;
  int key;
  key = arr[startPos];
  i = startPos;
  j = endPos;
  while (i < j) {
    while (arr[j] >= key && i < j)
      --j;
    arr[i] = arr[j];
    while (arr[i] <= key && i < j)
      ++i;
    arr[j] = arr[i];
  }
  arr[i] = key;
  if (i - 1 > startPos)
    quickSort(arr, startPos, i - 1);
  if (endPos > i + 1)
    quickSort(arr, i + 1, endPos);
}

/*Caculate middle value of num[]*/
int midArray(int *array) {
  for (int i = 0; i < d; i++)
    tmp_array[i] = array[i];

  quickSort(tmp_array, 0, d - 1);

  if (d % 2 == 0)
    return (int)((tmp_array[d / 2] + tmp_array[d / 2 - 1]) / 2);
  else
    return (int)(tmp_array[(d - 1) / 2]);
}

/*Caculate min_num of num[]*/
int minArray(int array[]) {
  int min = array[0];
  for (int i = 1; i < d; i++) {
    if (min >= array[i])
      min = array[i];
  }
  return min;
}

/*Initialization of map[d][w]*/
void initmap() {
  for (int i = 0; i < d; i++) {
    for (int j = 0; j < w; j++) {
      map[i][j] = 0;
      map_seek_max_usage_ratio[i][j] = 0;
    }
  }

  for (int j = 0; j < max_size; j++) {
    pkt_num1[j] = 0;
    pkt_num2[j] = 0;
  }
}

/*Print map*/
void printmap() {
  printf("Map info:\n");
  for (int i = 0; i < d; i++) {
    for (int j = 0; j < w; j++) {
      printf(" %d ", map[i][j]);
      if (j == w - 1)
        printf("\n");
    }
  }
  printf("Map info end!\n");
}

/*Hash caculation*/
void hash_caculate(char *str) {
  hash_value[0] = run(str, strlen(str), 750) % w;
  hash_value[1] = run(str, strlen(str), 751) % w;
  hash_value[2] = run(str, strlen(str), 752) % w;
  hash_value[3] = run(str, strlen(str), 753) % w;
  hash_value[4] = run(str, strlen(str), 754) % w;
  hash_value[5] = run(str, strlen(str), 755) % w;
  hash_value[6] = run(str, strlen(str), 756) % w;
  hash_value[7] = run(str, strlen(str), 757) % w;
  hash_value[8] = run(str, strlen(str), 758) % w;
  hash_value[9] = run(str, strlen(str), 759) % w;

  AddOrSub_value[0] = (run(str, strlen(str), 760) % 2 == 1) ? 1 : -1;
  AddOrSub_value[1] = (run(str, strlen(str), 761) % 2 == 1) ? 1 : -1;
  AddOrSub_value[2] = (run(str, strlen(str), 762) % 2 == 1) ? 1 : -1;
  AddOrSub_value[3] = (run(str, strlen(str), 763) % 2 == 1) ? 1 : -1;
  AddOrSub_value[4] = (run(str, strlen(str), 764) % 2 == 1) ? 1 : -1;
  AddOrSub_value[5] = (run(str, strlen(str), 765) % 2 == 1) ? 1 : -1;
  AddOrSub_value[6] = (run(str, strlen(str), 766) % 2 == 1) ? 1 : -1;
  AddOrSub_value[7] = (run(str, strlen(str), 767) % 2 == 1) ? 1 : -1;
  AddOrSub_value[8] = (run(str, strlen(str), 768) % 2 == 1) ? 1 : -1;
  AddOrSub_value[9] = (run(str, strlen(str), 769) % 2 == 1) ? 1 : -1;
}

/*Update map*/
void update(char *str) {
  hash_caculate(str);
  for (int i = 0; i < d; i++) {
    map[i][hash_value[i]] += AddOrSub_value[i];
    if (map[i][hash_value[i]] >= pow(2, bitwidth))
      map[i][hash_value[i]] = pow(2, bitwidth) - 1;
    if (map[i][hash_value[i]] > map_seek_max_usage_ratio[i][hash_value[i]])
      map_seek_max_usage_ratio[i][hash_value[i]] = map[i][hash_value[i]];
  }
}

/*Estimate num*/
int estimate(char *str) {
  mid_num = 0;
  hash_caculate(str);
  for (int i = 0; i < d; i++)
    num[i] = map[i][hash_value[i]] * AddOrSub_value[i];
  mid_num = midArray(num);
  return mid_num;
}

/*Count up the stream num*/
void countup() {
  // printmap();
  for (int i = 0; i < row_count; i++) {
    update(strArray[i]);
  }
  // printmap();
}

/*Integrate the results*/
void intergrate(char *str) {
  FILE *f;
  if (NULL == (f = fopen(str, "r"))) {
    fprintf(stderr, "Can not open txt file!\n");
    exit(0);
  }

  fscanf(f, "%d\n", &last_row_count);

  for (int i = 0; i < last_row_count; i++) {
    fscanf(f, "%s\n", &strArray[i]);
  }

  for (int i = 0; i < last_row_count; i++) {
    fscanf(f, "%d\n", &pkt_num1[i]);
  }

  fclose(f);
}

/*Seek the max value of map*/
int map_max_value() {
  int max = 0;
  for (int i = 0; i < d; i++) {
    for (int j = 0; j < w; j++) {
      if (max < map[i][j])
        max = map[i][j];
    }
  }
  return max;
}

/*Judge bin bits of the num*/
int judge_bits(int num) {
  if (num == 0)
    return 0;
  for (int i = 0; i < 100; i++)
    if (pow(2, (i - 1)) <= num && num < pow(2, i))
      return i;
}

int main(int argc, char **argv) {
  int Total_memory = atoi(argv[1]); // Memory total usage presented by bits
  d = atoi(argv[2]);                // Array height
  w = atoi(argv[3]);                // array width
  int big_flow_threshold_num = atoi(argv[4]); // Big flow threshold

  bitwidth = (int)((double)(Total_memory) / (double)(w * d));

  initmap();

#ifndef DATA_DIR
#define DATA_DIR "data"
#endif

#ifndef INPUT_FILE
#define INPUT_FILE "destination_before_integrate_test_flow.txt"
#endif

#ifndef OUTPUT_FILE
#define OUTPUT_FILE "destination_after_integrate_test_flow.txt"
#endif

  char input_path[256];
  char output_path[256];

  snprintf(input_path, sizeof(input_path), "%s/%s", DATA_DIR, INPUT_FILE);
  snprintf(output_path, sizeof(output_path), "%s/%s", DATA_DIR, OUTPUT_FILE);

  char *str1 = input_path;
  char *str2 = output_path;

  readtxt(str1);

  countup();

  // printmap();

  intergrate(str2);

  for (int i = 0; i < last_row_count; i++) {
    pkt_num2[i] = estimate(strArray[i]);
  }

  for (int m = 0; m < last_row_count; m++) {
    // printf("pkt----->%s     ", strArray[m]);
    // printf("pkt_num1----->%d     ", pkt_num1[m]);
    // printf("pkt_num2----->%d\n", pkt_num2[m]);
  }

  int BigCount = 0;
  double BigARE = 0.0;
  for (int m = 0; m < last_row_count; m++) {
    if (pkt_num1[m] >= big_flow_threshold_num) {
      BigCount++;
      BigARE +=
          (double)(abs(pkt_num1[m] - pkt_num2[m]) / (double)(pkt_num1[m])) /
          last_row_count;
    }
  }
  printf("%d    %lf    ", BigCount, BigARE);

  double ARE = 0.0;
  for (int m = 0; m < last_row_count - 1; m++) {
    ARE += (double)(abs(pkt_num1[m] - pkt_num2[m]) / (double)(pkt_num1[m])) /
           last_row_count;
  }
  printf("%lf    ", ARE);

  double AAE = 0.0;
  for (int m = 0; m < last_row_count - 1; m++) {
    AAE += (double)(abs(pkt_num1[m] - pkt_num2[m])) / last_row_count;
  }
  printf("%lf    ", AAE);

  printf("%d    ", map_max_value());

  int sum_bits_effective = 0;
  for (int i = 0; i < d; i++) {
    for (int j = 0; j < w; j++) {
      int num = map_seek_max_usage_ratio[i][j];
      sum_bits_effective += judge_bits(num);
    }
  }

  double bit_utilization_rate =
      (double)(sum_bits_effective / (double)(Total_memory));
  printf("%d    %f\n", sum_bits_effective, bit_utilization_rate);

  // printf("----------------\n");
  // for(int i=0; i<last_row_count; i++){
  // 	if(pkt_num1[i] >= big_flow_threshold_num){
  // 		printf("%s\n", strArray[i]);
  // 	}
  // }
  // printf("----------------\n");
  // for(int i=0; i<last_row_count; i++){
  // 	if(pkt_num2[i] >= big_flow_threshold_num){
  // 		printf("%s\n", strArray[i]);
  // 	}
  // }
  // printf("----------------\n");

  // printmap();

  return 0;
}