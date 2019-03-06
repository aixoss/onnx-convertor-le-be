#include <iostream>
#include <fstream>
#include <string>
#include "onnx.pb.h"
//#include "onnx-operators.pb.h"

using namespace std;

void print_node(const onnx::NodeProto &node) {

    cout << "Node Name: "<< node.name() << endl;
    for (int i = 0; i < node.input_size(); i++) {
        cout << "input: "<< node.input(i) << endl;
    }
    for (int i = 0; i < node.output_size(); i++) {
        cout <<"output: "<< node.output(i) <<endl;
    }
    cout << "op_type: "<< node.op_type() <<endl;
    cout << "domain: "<<node.domain() << endl;
    
}

int
print_data_type(int type) {
    switch(type) {
        case 1:
            cout <<" Float";
            return 4;
        case 7:
            cout <<" Int64";
            return 8;
        default:
            cout << type;
            exit(-1);   //Not supported
            return 4;
    }
}

/*
inline int endian_swap(unsigned int x)
{
    x = (x>>24) |
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
    return x;
}
inline int endian_swap8(unsigned int x)
{
    x = (x>>24) |
        ((x<<8) & 0x00FF0000) |
        ((x>>8) & 0x0000FF00) |
        (x<<24);
    return x;
}
*/

void SwapBytes(void *pv, size_t n)
{
    assert(n > 0);
    char *p = (char*)pv;
    size_t lo, hi;
    for(lo=0, hi=n-1; hi>lo; lo++, hi--)
    {
        char tmp=p[lo];
        p[lo] = p[hi];
        p[hi] = tmp;
    }
}
#define SWAP(x, n) SwapBytes(&x, n);

void
print_tensor_init(const onnx::TensorProto &init) {
    int size;
    cout<< "Dims: ";
    for (int i = 0; i < init.dims_size(); i++) {
        cout << init.dims(i)<< " ";
    }
    cout <<endl;

    cout<<"Data Type: ";
    size = print_data_type(init.data_type());
    cout << endl;
    //cout <<"doc_string: "<< init.doc_string() <<endl;

    //cout <<"external_data: "<<endl;
    //for (int i=0; i< init.external_data_size(); i++) {
    //    cout << "Key: " << init.external_data(i).key() << " Value: " << init.external_data(i).value() <<endl;
    //}
    const string &data = init.raw_data();
    cout <<"raw_data: ";
    int j=0;
    for( auto i=data.begin(); i < data.end(); i++, j++) {
        cout  << std::hex<<(int)*i << " " ;
        if (j == 30) break;
    }
    cout << std::dec;
    cout << endl;
   

}

int
print_TensorShapeProto(const onnx::TensorShapeProto &shape)
{
    cout << "Shape : ";
    for(int i=0; i < shape.dim_size(); i++) {
        if (shape.dim(i).has_dim_value())
            cout<< shape.dim(i).dim_value() << " ";
        if (shape.dim(i).has_dim_param())
            cout<< shape.dim(i).dim_param() << " ";
    }
    cout << endl;
    return 0;
}



int 
print_TypeProto(const onnx::TypeProto &type)
{
    if (type.has_tensor_type()) {
        const onnx::TypeProto::Tensor &tensor= type.tensor_type();
        if (tensor.has_elem_type())
            print_data_type(tensor.elem_type());
        if (tensor.has_shape()) {
            print_TensorShapeProto(tensor.shape());
        }
    }
    return 0;
}

int 
print_ValueInfoProto(const onnx::ValueInfoProto &data)
{
    cout << " Name: " << data.name() << endl;
    if (data.has_type()) {
        cout << "      Type: ";
        print_TypeProto(data.type());
        cout << endl;
    }
    return 0;
}


int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 2) {
    cerr << "Usage:  " << argv[0] << " model_file " << endl;
    return -1;
    }

    onnx::ModelProto model;
    {
        fstream input(argv[1], ios::in | ios::binary);
        if (!model.ParseFromIstream(&input)) {
            cerr << "Failed to parse model file." <<endl;
            return -1;
        }
    }

    cout<<"ir_version: " << model.ir_version() << endl;

    const onnx::GraphProto &graph = model.graph();

    for (int i = 0; i < graph.node_size(); i++) {
        const onnx::NodeProto &node = graph.node(i);
        print_node(node);
    }

    for (int i=0; i < graph.initializer_size(); i++){
        const onnx::TensorProto &initializer = graph.initializer(i);
        print_tensor_init(initializer);
    }

    cout << "ValueInfoProto input:" << endl;
    for (int i = 0; i < graph.input_size(); i++) {
        const onnx::ValueInfoProto &input = graph.input(i);
        print_ValueInfoProto(input);
    }

    cout << "ValueInfoProto output:" << endl;
    for (int i = 0; i < graph.output_size(); i++) {
        const onnx::ValueInfoProto &output = graph.output(i);
        print_ValueInfoProto(output);
    }

    cout << "ValueInfoProto value_info:" << endl;
    for (int i = 0; i < graph.value_info_size(); i++) {
        const onnx::ValueInfoProto &value_info = graph.value_info(i);
        print_ValueInfoProto(value_info);
    }

        

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
                 
