#ifndef C_UTIL_H
#define C_UTIL_H

class Util {
  public:
    static void print_free_memory ();
    static void print_partial_array (uint8_t* msj, uint8_t len);
    static bool is_equal_pointer_char_array (uint8_t* first, uint8_t* second, uint8_t length);
    static uint8_t get_index_by_char (char* str, char chr);
    static char* generateClientID ();
};

#endif
