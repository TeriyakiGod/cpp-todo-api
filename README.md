# C++ Todo API

This is a simple Todo API written in C++. It allows you to manage your tasks easily from the command line.

## Getting Started

Follow these instructions to get a copy of the project up and running on your local machine.

## Prerequisites

- cmake
- g++
- doxygen

### How to run
Clone the Repository
```bash
git clone https://github.com/your-username/cpp-todo-app.git
cd cpp-todo-app
```

Initialize and Update vcpkg Submodule
```bash
git submodule update --init --recursive
```

Build the Project
```bash
mkdir build
cd build
cmake ..
make
```
Building the project should get all the vcpkg dependencies, generate documentation and of course build the app.
Run the app after building.
```bash
./cpp-todo-api
```
The documentation should be available at the root (localhost:8080/)
