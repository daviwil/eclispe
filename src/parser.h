#ifndef _PARSER_H
#define _PARSER_H

#include "./types.h"
#include "./error.h"

Value* parse_form(char *form_string, Error** error);

#endif
