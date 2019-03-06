#include <iostream>
#include <fstream>
#include <string>
#include "onnx.pb.h"
//#include "onnx-operators.pb.h"

using namespace std;

void print_node(onnx::NodeProto *node) {
/*
    cout << "Node Name: "<< node.name() << endl;
    for (int i = 0; i < node.input_size(); i++) {
        cout << "input: "<< node.input(i) << endl;
    }
    for (int i = 0; i < node.output_size(); i++) {
        cout <<"output: "<< node.output(i) <<endl;
    }
    cout << "op_type: "<< node.op_type() <<endl;
    cout << "domain: "<<node.domain() << endl;
    */
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
print_tensor_init(onnx::TensorProto *init) {
    int size;
    cout<< "Dims: ";
    for (int i = 0; i < init->dims_size(); i++) {
        cout << init->dims(i)<< " ";
    }
    cout <<endl;

    cout<<"Data Type: ";
    size = print_data_type(init->data_type());
    cout << endl;
    //cout <<"doc_string: "<< init.doc_string() <<endl;

    //cout <<"external_data: "<<endl;
    //for (int i=0; i< init.external_data_size(); i++) {
    //    cout << "Key: " << init.external_data(i).key() << " Value: " << init.external_data(i).value() <<endl;
    //}
    string *data = init->mutable_raw_data();
    //cout <<"raw_data: " << data <<endl;
    char* ptr = (char*)data->c_str();
    cout <<"Raw Data: Converted "<<data->length() << endl;
    for (int i =0; i < data->length(); i += size) {
        //cout << hex << ptr[i] << " ";
        //ptr[i] = endian_swap(ptr[i]);
        SWAP(ptr[i], size)
        //cout << ptr[i] << " ";
    }
    //init.mutable_raw_data();
    cout << endl;
   

}


int main(int argc, char* argv[]) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (argc != 3) {
    cerr << "Usage:  " << argv[0] << " model_file output_file" << endl;
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

    onnx::GraphProto *graph = model.mutable_graph();

    for (int i = 0; i < graph->node_size(); i++) {
        onnx::NodeProto *node = graph->mutable_node(i);
        print_node(node);
    }

    for (int i=0; i < graph->initializer_size(); i++){
        onnx::TensorProto *initializer = graph->mutable_initializer(i);
        print_tensor_init(initializer);
    }

    {
        fstream output(argv[2], ios::out | ios::trunc | ios::binary);
        if (!model.SerializeToOstream(&output)) {
            cerr << "Failed to write." << endl;
            return -1;
        }
    }


        

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
                 
