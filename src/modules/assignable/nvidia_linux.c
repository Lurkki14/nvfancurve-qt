#include "/opt/cuda/include/nvml.h"
#include <X11/Xlib.h>
#include <string.h>

#include <tc_assignable.h>
#include <tc_common.h>

#define MAX_GPUS 32

// Local function declarations
static int8_t init();
static int8_t close();
static int8_t generate_assignable_tree();

static uint32_t gpu_count;
static nvmlDevice_t nvml_handles[MAX_GPUS];
static Display *dpy;
static tc_assignable_node_t *root_node;

static int8_t init() {
  // Initialize library
  if (nvmlInit_v2() != NVML_SUCCESS) {
    return TC_EGENERIC;
  }

  // Query GPU count
  if (nvmlDeviceGetCount(&gpu_count) != NVML_SUCCESS) {
    return TC_EGENERIC;
  }

  if (gpu_count > MAX_GPUS) {
    return TC_ENOMEM;
  }

  // Get nvml handles
  uint32_t valid_count = 0;
  for (uint8_t i = 0; i < gpu_count; i++) {
    nvmlDevice_t dev;

    if (nvmlDeviceGetHandleByIndex_v2(i, &dev) == NVML_SUCCESS) {
      nvml_handles[valid_count] = dev;
      valid_count++;
    }
  }
  gpu_count = valid_count;

  // Get X11 display
  if ((dpy = XOpenDisplay(NULL)) == NULL) {
    return TC_EGENERIC;
  }

  // Generate the tree structure of assignables for every GPU. Free in close().
  generate_assignable_tree();

  return TC_SUCCESS;
}

static int8_t close() {

}

static int8_t generate_assignable_tree() {
  // Allocate memory for root node
  root_node = tc_assignable_node_new();

  for (uint32_t i = 0; i < gpu_count; i++) {
    // Get GPU name and use it as the root item for GPU
    char gpu_name[NVML_DEVICE_NAME_BUFFER_SIZE];

    if (nvmlDeviceGetName(nvml_handles[i], gpu_name, NVML_DEVICE_NAME_BUFFER_SIZE) != NVML_SUCCESS) {
        continue;
    }
    // Got the name, append the item to the root item
    tc_assignable_node_t *gpu_name_node = tc_assignable_node_new();
    gpu_name_node->name = strdup(gpu_name);

    // Append to the root node
    if (tc_assignable_node_add_child(root_node, gpu_name_node) != TC_SUCCESS) {

    }
  }
}
