# C++ Todo API

This is a simple Todo API written in C++. It allows you to manage your tasks easily from the command line.

## Getting Started

Follow these instructions to get a copy of the project up and running on your local machine.

### Clone the Repository

```bash
git clone https://github.com/your-username/cpp-todo-app.git
cd cpp-todo-app
```
Initialize and Update vcpkg Submodule

```bash
git submodule update --init --recursive
```

Install Dependencies with vcpkg
```bash
./vcpkg/vcpkg install --triplet x64-linux
```

Build the Project
```bash
mkdir build
cd build
cmake ..
make
```