/* tinyapl/lowering/c11/c11_lowering.h
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 */
#ifndef TINYAPL_C11_LOWERING_H
#define TINYAPL_C11_LOWERING_H

#include "../../array_ir/array_ir.h"

/* Emit standalone C11 source implementing the IR program. Returns 0 on success. */
int tinyapl_lower_c11(const IrNode *prog, const char *out_path, char *err_buf, size_t err_sz);

#endif /* TINYAPL_C11_LOWERING_H */
