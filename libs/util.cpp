#include <Arduino.h>
#include "util.h"

extern unsigned int __bss_end;
extern unsigned int __heap_start;
extern void *__brkval;

void Util::print_free_memory () {
  int free_memory;

  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }

  Serial.print(F("Free memory: "));
  Serial.println(free_memory);
};

void Util::print_partial_array (uint8_t* msj, uint8_t len) {
  for (uint8_t i = 0; i < len; ++i) {
    Serial.print((char)msj[i]);
  }
  Serial.println(F(""));
};

bool Util::is_equal_pointer_char_array (uint8_t* first, uint8_t* second, uint8_t length) {
  bool equal = true;

  for (int i = 0; i < length; ++i) {
    if (first[i] != second[i]) {
      equal = false;
      break;
    }
  }

  return equal;
};

uint8_t Util::get_index_by_char (char* str, char chr) {
  return strchr(str, chr) - str;
};

char* Util::generateClientID () {
  char* cID = new char[23]();
  for (int i=0; i<22; i+=1)
    cID[i] = (char)random(1, 256);
  return cID;
};

