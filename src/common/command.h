#ifndef COMMON_COMMAND_H
#define COMMON_COMMAND_H

#define CMD_KIND_STEERING_ANGLE 1
#define CMD_KIND_DRIVE 2

struct Command {
    int8_t kind;
    int8_t value;
};

#endif