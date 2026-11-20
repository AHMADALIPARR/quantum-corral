/* tinyapl/parser/tinyapl_parser.h
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 */
#ifndef TINYAPL_PARSER_H
#define TINYAPL_PARSER_H

#include "../array_ir/array_ir.h"

typedef struct {
    char name[64];
    int32_t rank;
    int64_t shape[CORRAL_MAX_RANK];
    CorralDType dtype;
} TinyAplSymbol;

typedef struct {
    TinyAplSymbol syms[32];
    int num_syms;
} TinyAplEnv;

/* Parse tinyAPL program into a linked list of IR_OP_ASSIGN statements */
IrNode *tinyapl_parse_program(const char *src, TinyAplEnv *env, char *err_buf, size_t err_sz);

#endif /* TINYAPL_PARSER_H */
