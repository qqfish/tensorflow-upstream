/* Copyright 2020 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/core/profiler/utils/hardware_type_utils.h"

#include "absl/strings/match.h"
#include "tensorflow/core/platform/logging.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/profiler/protobuf/hardware_types.pb.h"

namespace tensorflow {
namespace profiler {
namespace {

// Get theoretical upperbound of single precision FMA throughput of the GPU per
// cycle per streaming multiprocessor.
// https://docs.nvidia.com/cuda/cuda-c-programming-guide/index.html#arithmetic-instructions__throughput-native-arithmetic-instructions
uint32 GetFmaMaxThroughputPerSMPerCycle(const DeviceCapabilities& device_cap) {
  if (device_cap.dev_manufacturer() == "Nvidia"){
    // return 128;
    uint32 n_fp32_cores = 0;
    uint32 n_tc_cores = 0;
    switch (device_cap.compute_capability().major()) {
      case 2:
        // Fermi
        n_fp32_cores = 32;
        break;
      case 3:
        // Kepler
        n_fp32_cores = 192;
        break;
      case 5:
        // Maxwell
        n_fp32_cores = 128;
        break;
      case 6:
        // Pascal
        if (device_cap.compute_capability().minor() > 0) {
          // Pascal SM61/62
          n_fp32_cores = 128;
        } else {
          // Pascal SM60
          n_fp32_cores = 64;
        }
        break;
      case 7:
        // Volta and Turing
        n_fp32_cores = 64;
        n_tc_cores = 8;
        break;
      case 8:
        // Ampere
        if (device_cap.compute_capability().minor() >= 6) {
          // Ampere SM86
          n_fp32_cores = 128;
        } else {
          // Ampere SM80
          n_fp32_cores = 64;
        }
        n_tc_cores = 4;
        break;
      default:
        LOG(ERROR) << "Invalid GPU compute capability.";
        break;
    }
    // GPU TensorCore can execute 64 FMAs per cycle.
    // https://devblogs.nvidia.com/programming-tensor-cores-cuda-9/
    return n_fp32_cores + n_tc_cores * 64;
  }else if (device_cap.dev_manufacturer() == "AMD"){
    uint32_t n_xdlops = 0;
    uint32_t n_fp32_cores = 0;

    if (device_cap.compute_capability().major() <= 9){
      n_fp32_cores = 64;
    }
    else {
      n_fp32_cores = 32;
    }
    // TODO(rocm-profiler): 
    return n_fp32_cores + n_xdlops * 1;
  }
  else{
    LOG(ERROR) << "Unknown device manufacturer " << device_cap.dev_manufacturer();
    return {};
  }
}

}  // namespace

double GetFlopMaxThroughputPerSM(const DeviceCapabilities& device_cap) {
  // One FMA = 2 floating point operations, one multiply and one add.
  return GetFmaMaxThroughputPerSMPerCycle(device_cap) * 2 *
         device_cap.clock_rate_in_ghz();
}

absl::string_view GpuModelName(const DeviceCapabilities& device_cap) {
  if (device_cap.dev_manufacturer() == "Nvidia"){
    switch (device_cap.compute_capability().major()) {
      case 2:
        return "Nvidia GPU (Fermi)";
      case 3:
        return "Nvidia GPU (Kepler)";
      case 5:
        return "Nvidia GPU (Maxwell)";
      case 6:
        return "Nvidia GPU (Pascal)";
      case 7:
        if (device_cap.compute_capability().minor() < 5) {
          return "Nvidia GPU (Volta)";
        } else {
          return "Nvidia GPU (Turing)";
        }
      case 8:
        return "Nvidia GPU (Ampere)";
      default:
        return "Nvidia GPU";
    }
  }
  else if (device_cap.dev_manufacturer() == "AMD"){
    switch (device_cap.compute_capability().major()){
      case 9:
        return "AMD GPU - gfx9";
      case 10:
        return "AMD GPU - gfx10";
      case 11:
        return "AMD GPU - gfx11";    
      default:
        return "AMD GPU";  

    }
  }
  else{
    LOG(ERROR) << "Unknown GPU manufacturer " << device_cap.dev_manufacturer();
    return {};
  }
}

HardwareType ParseHardwareType(absl::string_view device_type) {
  if (absl::StrContains(device_type, "GPU")) return HardwareType::GPU;
  if (device_type == "CPU") return HardwareType::CPU_ONLY;
  if (device_type == "TPU") return HardwareType::TPU;
  return HardwareType::UNKNOWN_HARDWARE;
}

bool HasDevice(HardwareType x) { return x > tensorflow::profiler::CPU_ONLY; }

}  // namespace profiler
}  // namespace tensorflow
