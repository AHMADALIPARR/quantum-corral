/* tinyapl/typecheck/typecheck.h
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 */
#ifndef TINYAPL_TYPECHECK_H
#define TINYAPL_TYPECHECK_H

#include "../array_ir/array_ir.h"
#include "../parser/tinyapl_parser.h"

/* Infers rank, shape, dtype and validates broadcasting/contraction invariants. Returns 0 on success. */
int tinyapl_typecheck(IrNode *prog, TinyAplEnv *env, char *err_buf, size_t err_sz);

#endif /* TINYAPL_TYPECHECK_H */
