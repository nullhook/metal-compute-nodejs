#include "Metal/MTLComputePipeline.hpp"
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <cstddef>
#include <iostream>

extern "C" float adder_gpu( float a, float b ) {
  const char* shaderSrc = R"(
    #include <metal_stdlib>
    using namespace metal;

    kernel void add(const device float2 *in [[ buffer(0) ]],
                    device float  *out [[ buffer(1) ]],
                    uint id [[ thread_position_in_grid ]]) {
      out[id] = in[id].x + in[id].y;
    }
  )";

  MTL::Device* device = MTL::CreateSystemDefaultDevice();
  MTL::CommandQueue* command_queue = device->newCommandQueue();

  NS::Error* lib_error = nullptr;
  MTL::Library* library = device->newLibrary(NS::String::string(shaderSrc, NS::StringEncoding::UTF8StringEncoding), nullptr, &lib_error);

  if ( !library ) {
    __builtin_printf( "%s", lib_error->localizedDescription()->utf8String() );
    assert( false );
  }

  MTL::CommandBuffer* command_buffer = command_queue->commandBuffer();
  MTL::ComputeCommandEncoder* encoder = command_buffer->computeCommandEncoder();

  MTL::Function* compute_func = library->newFunction(NS::String::string("add", NS::StringEncoding::UTF8StringEncoding));

  MTL::NewComputePipelineStateCompletionHandlerFunction complete_handler = [encoder](MTL::ComputePipelineState* state, NS::Error* error) -> void {
    encoder->setComputePipelineState(state);
    std::cout << state << "\n";
  };
  
  NS::Error* foo_error = nullptr;
  MTL::ComputePipelineState* computePSO = device->newComputePipelineState(compute_func, &foo_error);

  if ( !computePSO ) {
      __builtin_printf( "%s", foo_error->localizedDescription()->utf8String() );
      assert(false);
  }

  encoder->setComputePipelineState(computePSO);

  float input[] = {a,b};
  int input_len = sizeof(input);
  int output_len = sizeof(float);

  encoder->setBuffer(device->newBuffer(input, input_len, {}), 0, 0);

  MTL::Buffer* output_buffer = device->newBuffer(output_len, {});
  encoder->setBuffer(output_buffer, 0, 1);

  auto threadPerGroup = MTL::Size::Make(1, 1, 1);
  auto groupsPerGrid = MTL::Size::Make(1, 1, 1);

  encoder->dispatchThreadgroups(threadPerGroup, groupsPerGrid);
  encoder->endEncoding();

  command_buffer->commit();
  command_buffer->waitUntilCompleted();

  float* output = static_cast<float*>(output_buffer->contents());

  return (*output);
}

extern "C" float adder_cpu( float a, float b ) {
  return a+b;
}
