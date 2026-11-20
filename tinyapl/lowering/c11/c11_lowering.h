/* tinyapl/lowering/c11/c11_lowering.h
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef TINYAPL_C11_LOWERING_H
#define TINYAPL_C11_LOWERING_H

#include "../../array_ir/array_ir.h"
#include <stdio.h>

/* Emits a complete C11 translation unit calling native/include/corral.h */
int tinyapl_lower_to_c11(const IrNode *prog, FILE *out);

#endif /* TINYAPL_C11_LOWERING_H */
