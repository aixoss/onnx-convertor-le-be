#include <iostream>
#include <fstream>
#include <string>
#include "onnx.pb.h"
//#include "onnx-operators.pb.h"

using namespace std;

int
print_data_type(int type) {
    switch(type) {
        case 1:
            cout <<" Float";
            return 4;
        case 2:
            cout << " UInt8";
            return 1;
        case 3:
            cout << " Int8";
            return 1;
        case 4:
            cout << " UInt16";
            return 2;
        case 5:
            cout << " Int16";
            return 2;
        case 6:
            cout << " Int32";
            return 4;
        case 7:
            cout << " Int64";
            return 8;
        case 8:
            cout << " String";
            return 1;
        case 9:
            cout << " Bool";
            return 1;
        case 10:
            cout << " Float16";
            return 2;
        case 11:
            cout << " Double";
            return 8;
        case 12:
            cout << " Uint32";
            return 4;
        case 13:
            cout << " Uint64";
            return 8;
        case 14:
            cout << " Complex64";
            return 4;
        case 15:
            cout << " Complex128";
            return 8;
        case 16:
            cout << " Bfloat16";
            return 2;
        default:
            cout << "Unknown type. Exiting.." << type;
            exit(-1);   //Not supported
            return 0;
    }
}

void SwapBytes(char *p, size_t n)
{
    size_t lo, hi;
    for(lo=0, hi=n-1; hi>lo; lo++, hi--)
    {
        char tmp=p[lo];
        p[lo] = p[hi];
        p[hi] = tmp;
    }
}

void
conv_tensor_init(onnx::TensorProto *init) {
    int size;
    cout<< "Dims: ";
    for (int i = 0; i < init->dims_size(); i++) {
        cout << init->dims(i)<< " ";
    }
    cout <<endl;

    cout<<"Data Type: ";
    size = print_data_type(init->data_type());
    cout << endl;

    if (!init->has_raw_data()) {
        cout <<"Endian Conversion not needed."<< endl;
        return;
    }

    string *data = init->mutable_raw_data();
    char* ptr = (char*)data->c_str();
    for (int i =0; i < data->length(); i += size) {
        if (size != 1)
            SwapBytes(&ptr[i], size); //Convert the endianess
    }
    cout <<"Raw Data: Converted "<<data->length() << " Bytes" << endl;
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

    cout<<"Model ir_version: " << model.ir_version() << endl;

    onnx::GraphProto *graph = model.mutable_graph();

    for (int i=0; i < graph->initializer_size(); i++){
        onnx::TensorProto *initializer = graph->mutable_initializer(i);
        conv_tensor_init(initializer);
    }

    {
        fstream output(argv[2], ios::out | ios::trunc | ios::binary);
        if (!model.SerializeToOstream(&output)) {
            cerr << "Failed to write." << endl;
            return -1;
        }
    }


    google::protobuf::ShutdownProtobufLibrary();

    cout << "Model successfully converted.."<< endl;
    return 0;
}
                 
