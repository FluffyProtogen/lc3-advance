#pragma once

#include "parser.h"

__attribute__((section(".ewram"))) extern char obj_buf[3000][4];

bool write_to_object(const Instructions *instructions);
