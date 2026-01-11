/*
  compile: gcc -o ASketch.exe Asketch.c -std=c99
  run: ASketch.exe [filter_size] [bitwidth_map] [d] [w] [big_flow_threshold_num]
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

/*map width*/
int w = 0;

/*map height*/
int d = 0;

/*bit width of map*/
int bitwidth_map = 0;

/*map [d][w]*/
int map[hash_num][max_size];

/*filter size*/
int filter_size = 0;

/*key array of filter*/
char *pkt_key[max_size];

/*old_count array of filter*/
int old_count[max_size];

/*new_count array of filter*/
int new_count[max_size];

/*hash_value[d]*/
int hash_value[hash_num];

/*global num[]*/
int num[hash_num];

/*global min_num*/
int min_num;

/*global row count of txt*/
int row_count = 0;

/*26 char array*/
char strArray[max_size][27];
char strArrayIntegrate[max_size][27];

/*pkt_num1[] by hard calculation*/
int pkt_num1[max_size];

/*pkt_num1[] by soft calculation*/
int pkt_num2[max_size];

/*gloabal row_count after intergrate*/
int last_row_count = 0;
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
  for (int i = 0; i < d; i++)
    for (int j = 0; j < w; j++)
      map[i][j] = 0;

  for (int j = 0; j < max_size; j++) {
    pkt_key[j] = "\0";
    old_count[j] = 0;
    new_count[j] = 0;
    pkt_num1[j] = 0;
    pkt_num2[j] = 0;
  }
}

/*Print filter*/
void printfilter() {
  printf("Filter info:\n");
  for (int i = 0; i < filter_size; i++)
    printf(" %s ", pkt_key[i]);
  printf("\n");
  for (int i = 0; i < filter_size; i++)
    printf(" %d ", new_count[i]);
  printf("\n");
  for (int i = 0; i < filter_size; i++)
    printf(" %d ", old_count[i]);
  printf("\n");
  printf("Filter info end!\n");
}

void printpktkey() {
  printf("Filter pkt key info:\n");
  for (int i = 0; i < filter_size; i++)
    printf(" %s \n", pkt_key[i]);
  printf("\n");
  printf("Filter pkt key info end!\n");
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
}

/*Judge filter exist pkt(return index) or not(return -1)*/
int judge_exist(char *str) {
  for (int i = 0; i < filter_size; i++)
    if (strcmp(pkt_key[i], str) == 0)
      return i;
  return -1;
}

/*Judge filter exist NULL(return index) or not(return -1)*/
int judge_null() {
  for (int i = 0; i < filter_size; i++)
    if (pkt_key[i] == "\0")
      return i;
  return -1;
}

/*Find the min num in filter(return index)*/
int find_min() {
  int min_result = max_size;
  int min_index = -1;
  for (int i = 0; i < filter_size; i++) {
    if (new_count[i] < min_result) {
      min_result = new_count[i];
      min_index = i;
    }
  }
  return min_index;
}

/*Update map*/
void update(char *str, int num11) {
  int filter_index = judge_exist(str);
  if (filter_index != -1) { // pkt exists
    new_count[filter_index] += 1;
  } else { ////pkt dosen't exists
    int null_index = judge_null();
    if (null_index != -1) { // null exists
      pkt_key[null_index] = str;
      new_count[null_index] = 1;
      old_count[null_index] = 0;
    } else { // null dosen't exists
      hash_caculate(str);
      for (int i = 0; i < d; i++) {
        map[i][hash_value[i]]++;
        if (map[i][hash_value[i]] >= pow(2, bitwidth_map))
          map[i][hash_value[i]] = pow(2, bitwidth_map) - 1;
      }
      for (int i = 0; i < d; i++)
        num[i] = map[i][hash_value[i]];

      min_num = minArray(num);

      int min_index = find_min();
      if (min_num > new_count[min_index]) {
        if (new_count[min_index] - old_count[min_index] > 0) {
          hash_caculate(pkt_key[min_index]);
          for (int i = 0; i < d; i++) {
            map[i][hash_value[i]] +=
                new_count[min_index] - old_count[min_index];
            if (map[i][hash_value[i]] >= pow(2, bitwidth_map))
              map[i][hash_value[i]] = pow(2, bitwidth_map) - 1;
          }
          pkt_key[min_index] = str;
          new_count[min_index] = min_num;
          old_count[min_index] = min_num;
        }
      }
    }
  }
}

/*Count up the stream num*/
void countup() {
  // printmap();
  for (int i = 0; i < row_count; i++) {
    update(strArray[i], i);
  }
  // printmap();
}

/*Estimate the flow size*/
void estimate() {
  for (int i = 0; i < last_row_count; i++) {
    int filter_index = judge_exist(strArrayIntegrate[i]);
    if (filter_index != -1)
      pkt_num2[i] = new_count[filter_index];
    else {
      hash_caculate(strArrayIntegrate[i]);
      for (int j = 0; j < d; j++)
        num[j] = map[j][hash_value[j]];

      pkt_num2[i] = minArray(num);
    }
  }
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
    fscanf(f, "%s\n", &strArrayIntegrate[i]);
  }

  for (int i = 0; i < last_row_count; i++) {
    fscanf(f, "%d\n", &pkt_num1[i]);
  }

  fclose(f);
}

/*Seek the max value of filter*/
int filter_max_value() {
  int max = 0;
  for (int i = 0; i < filter_size; i++)
    if (new_count[i] > max)
      max = new_count[i];
  return max;
}

/*Seek the max value of map*/
int map_max_value() {
  int max = 0;
  for (int i = 0; i < d; i++)
    for (int j = 0; j < w; j++)
      if (max < map[i][j])
        max = map[i][j];
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

  // int Total_memory = atoi(argv[1]); //Memory total usage presented by bits
  filter_size = atoi(argv[1]);  // filter size
  bitwidth_map = atoi(argv[2]); // map bitwidth
  w = atoi(argv[3]);
  d = atoi(argv[4]);
  int big_flow_threshold_num = atoi(argv[5]);

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

  intergrate(str2);

  estimate();

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

  int sum_bits_effective = 0;
  for (int i = 0; i < d; i++) {
    for (int j = 0; j < w; j++) {
      int num = map[i][j];
      sum_bits_effective += judge_bits(num);
    }
  }
  for (int i = 0; i < filter_size; i++) {
    int num = new_count[i];
    sum_bits_effective += judge_bits(num);
    num = old_count[i];
    sum_bits_effective += judge_bits(num);
  }
  sum_bits_effective += 104 * filter_size;

  double bit_utilization_rate =
      (double)(sum_bits_effective /
               (double)((104 + 14 * 2) * filter_size + w * d * bitwidth_map));
  printf("%d    %f\n", sum_bits_effective, bit_utilization_rate);

  return 0;
}