# metal-compute-nodejs

This is an experiment in which an extern C function has been created and compiled into a shared library. This shared library can be utilized by the new FFI function in Node.js, calling a Metal compute shader to perform an addition operation. The function can be executed by running the following command in the terminal

`./node -p "require('node:ffi').getNativeFunction('./libmetal-compute.dylib', 'adder_gpu', 'float', ['float', 'float'])(1.0, 2.0)"`
