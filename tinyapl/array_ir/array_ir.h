/* tinyapl/array_ir/array_ir.h - Typed Array Intermediate Representation
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#ifndef TINYAPL_ARRAY_IR_H
#define TINYAPL_ARRAY_IR_H

#include "corral.h"

typedef enum {
    IR_OP_CONST_F64 = 0,
    IR_OP_VAR,
    IR_OP_RESHAPE,      /* rho */
    IR_OP_ADD,          /* + */
    IR_OP_SUB,          /* - */
    IR_OP_MUL,          /* * / × */
    IR_OP_MAG2,         /* |psi|^2 -> real^2 + imag^2 */
    IR_OP_REDUCE_SUM,   /* +/ */
    IR_OP_EXTERN_BESSEL0,
    IR_OP_EXTERN_BESSEL1,
    IR_OP_ASSIGN
} IrOpKind;

typedef struct IrNode {
    IrOpKind op;
    char name[64];
    double scalar_val;
    int32_t rank;
    int64_t shape[CORRAL_MAX_RANK];
    CorralDType dtype;
    struct IrNode *lhs;
    struct IrNode *rhs;
    struct IrNode *next;
} IrNode;

IrNode *ir_node_new(IrOpKind op, const char *name);
void ir_node_free(IrNode *node);

#endif /* TINYAPL_ARRAY_IR_H */
