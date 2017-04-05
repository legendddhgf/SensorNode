#ifndef MODULE_H_
#define MODULE_H_

#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define MODULE_IDENTIFIER_STRING "MODULE"

typedef struct Module {
  uint8_t type_num; // determined through runtime based on order of creation
  // Needs something to print in case of default function being called
  const char *type_str; // the string for the kind of device this is
  uint8_t index; // order of module of type(type_num) to be created (start at 0)
  volatile uint8_t *port[8]; // address of The port this device associates with
  volatile uint8_t *pin[8]; // address of The pin this device is associated with
  volatile uint8_t *ddr[8]; // address of The data direction register associated
  uint8_t reg_bit[8]; // bit of the above three registers to index into
  uint8_t pin_count; // number of pins assigned. You can use (0 : pin_count - 1)
  void *(*init)(struct Module); // init function
  void *(*read)(struct Module); // read function
  void *(*write)(struct Module, void *); // write function
  void *(*destroy)(struct Module); // destroy function
} Module;

Module new_module(void); // all modules are created with this function

void *module_init(Module); // default init function
void *module_read(Module); // default read function
void *module_write(Module, void *); // default write function
void *module_destroy(Module); // default destroy function

#endif
