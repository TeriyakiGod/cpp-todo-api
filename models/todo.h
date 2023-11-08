#ifndef TODO_MODEL_H
#define TODO_MODEL_H
#include <string>

typedef struct {
    std::string id;
    std::string title;
    std::string description;
    std::string status;
} todo;

#endif // TODO_MODEL_H