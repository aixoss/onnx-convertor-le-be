# onnx-convertor-le-be
**ONNX Convertor to convert ONNX Models from LE to BE and Vice Versa**

I use this code(onnx_conv.cc) to convert onnx models from Little Endian to Big Endian and vice versa. 
Have done some minimal testing to run my models b/w mac and AIX systems.

## Compile command that i use on MAC

*clang++ -DONNX_NAMESPACE=onnx -DONNX_API= -I /Users/shajithchandran/miniconda3/envs/python2.7/include/ 
onnx_conv.cc onnx.pb.cc -o onnx_conv -std=c++11 -lpthread -L /Users/shajithchandran/miniconda3/envs/python2.7/lib 
-lprotobuf*

## Compile command on AIX

*/usr/bin/g++  -DONNX_NAMESPACE=onnx -DONNX_API=  -maix64  -L/opt/freeware/lib/gcc/powerpc-ibm-aix7.2.0.0/8.1.0/pthread 
-lstdc++ -pthread   -I /usr/local/include  -mvsx -maix64 -O3 -DNDEBUG -fPIC   -std=gnu++11   onnx.pb.cc  onnx_conv.cc -I. -L. -lprotobuf -o onnx_conv*

*Change the paths in the above command as appropriate.*

**Note:** You need to correctly include the correct version of the protobuf header files. The onnx.pb.cc and onnx.pb.h 
can be picked up from the onnx build or you can build it your self using the .proto files from the onnx github.

The onnx_read.cc was written to examine the file content mainly for debugging while developing this tool.

## Run ONNX models from ONNX Model Zoo on Big Endian Machine
Used the converted models from onnx model repo(https://github.com/onnx/models) and was able to successfully do inferencing on Big Endian Machine(AIX).
Steps:
1. First convert the required model to Big Endian
  ```
  ./onnx_conv ./model.onnx ./resnet50.onnx.be
  ```
2. You need to convert the sample input/output files too that are hosted along with the models. Use tensor_conv.cc code to do the same.
  ```
  ./tensor_conv ./test_data_set_0/input_0.pb ./test_data_set_0/input_0.pb.be
  ./tensor_conv ./test_data_set_0/output_0.pb ./test_data_set_0/output_0.pb.be
  ```
3. Take the files to Big Endian machine for inferencing. Sample inferencing code is given in onnx model repo github to compare the output of your program with the reference output.



