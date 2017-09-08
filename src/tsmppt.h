#ifndef TSMPPT_H_
#define TSMPPT_H_

#include <string.h>
#include "module.h"

#ifndef TSMPPT_MAX
#define TSMPPT_MAX 2
#endif

#define TSMPPT_IDENTIFIER_STRING "TSMPPT"

typedef Module Tsmppt;

Tsmppt new_tsmppt(uint8_t, Tsmppt);

void tsmppt_init(Tsmppt t);

void tsmppt_read(Tsmppt t, char *, uint16_t);

void tsmppt_destroy(Tsmppt t);

#endif
