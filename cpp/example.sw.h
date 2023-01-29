/*************************************************************************** 
 *  accelerator interface header                                             
 ***************************************************************************/
   
#define ACCEL_ADDR ((volatile unsigned int *) 0x30000000) 
   
   
// register map 
   
#define GO_REG                             (*(ACCEL_ADDR + 0)) 
#define GO_READY_REG                       (*(ACCEL_ADDR + 1)) 
#define DONE_REG                           (*(ACCEL_ADDR + 2)) 
#define DONE_VALID_REG                     (*(ACCEL_ADDR + 3)) 
#define RELU_REG                           (*(ACCEL_ADDR + 4)) 
#define USE_BIAS_REG                       (*(ACCEL_ADDR + 5)) 
#define MAX_POOL_REG                       (*(ACCEL_ADDR + 6)) 
#define IMAGE_OFFSET_REG                   (*(ACCEL_ADDR + 7)) 
#define WEIGHT_OFFSET_REG                  (*(ACCEL_ADDR + 8)) 
#define BIAS_OFFSET_REG                    (*(ACCEL_ADDR + 9)) 
#define OUTPUT_OFFSET_REG                  (*(ACCEL_ADDR + 10)) 
#define NUM_INPUT_IMAGES_REG               (*(ACCEL_ADDR + 11)) 
#define NUM_OUTPUT_IMAGES_REG              (*(ACCEL_ADDR + 12)) 
#define IMAGE_HEIGHT_REG                   (*(ACCEL_ADDR + 13)) 
#define IMAGE_WIDTH_REG                    (*(ACCEL_ADDR + 14)) 
#define FILTER_HEIGHT_REG                  (*(ACCEL_ADDR + 15)) 
#define FILTER_WIDTH_REG                   (*(ACCEL_ADDR + 16)) 
#define ADDR_OFFSET_LOW_REG                (*(ACCEL_ADDR + 17)) 
#define ADDR_OFFSET_HIGH_REG               (*(ACCEL_ADDR + 18)) 
#define BURST_SIZE_REG                     (*(ACCEL_ADDR + 19)) 



// convenience macros 

#define GO                               { while (!GO_READY_REG); GO_REG = 1; } 
#define WAIT_FOR_DONE                    { while (!DONE_VALID_REG); volatile unsigned int x = DONE_REG; } 
#define SET_RELU(X)                      { RELU_REG = X; } 
#define SET_USE_BIAS(X)                  { USE_BIAS_REG = X; } 
#define SET_MAX_POOL(X)                  { MAX_POOL_REG = X; } 
#define SET_IMAGE_OFFSET(X)              { IMAGE_OFFSET_REG = X; } 
#define SET_WEIGHT_OFFSET(X)             { WEIGHT_OFFSET_REG = X; } 
#define SET_BIAS_OFFSET(X)               { BIAS_OFFSET_REG = X; } 
#define SET_OUTPUT_OFFSET(X)             { OUTPUT_OFFSET_REG = X; } 
#define SET_NUM_INPUT_IMAGES(X)          { NUM_INPUT_IMAGES_REG = X; } 
#define SET_NUM_OUTPUT_IMAGES(X)         { NUM_OUTPUT_IMAGES_REG = X; } 
#define SET_IMAGE_HEIGHT(X)              { IMAGE_HEIGHT_REG = X; } 
#define SET_IMAGE_WIDTH(X)               { IMAGE_WIDTH_REG = X; } 
#define SET_FILTER_HEIGHT(X)             { FILTER_HEIGHT_REG = X; } 
#define SET_FILTER_WIDTH(X)              { FILTER_WIDTH_REG = X; } 
