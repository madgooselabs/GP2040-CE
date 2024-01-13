#ifndef _GPGFX_UI_TYPES_H_
#define _GPGFX_UI_TYPES_H_

#include <string>
#include <functional>
#include <vector>

typedef struct {
    std::string label;
    std::function<void()> action;
} MenuEntry;

#endif