# sigslot
An lightweight implementation of the observer pattern by c++11

# Features
Very lightweight
Built with modern C++14 features
No dpendency, only c++ stand library
Head file only


# Usage
sigslot<int> sig;
auto id = sig.connect([](int a){
    cout<<a<<endl;>
});
sig(5);


# Building test using CMake
mkdir build
cd to build
cmake ..
make


# Author
ifbao@qq.com


