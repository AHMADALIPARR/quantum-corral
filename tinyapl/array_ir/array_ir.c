/* tinyapl/array_ir/array_ir.c
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 */
#include "array_ir.h"
#include <stdlib.h>
#include <string.h>

IrNode *ir_node_new(IrOpKind op, const char *name) {
    IrNode *n = (IrNode *)calloc(1, sizeof(IrNode));
    if (!n) return NULL;
    n->op = op;
    n->dtype = CORRAL_F64;
    if (name) {
        strncpy(n->name, name, sizeof(n->name) - 1);
    }
    return n;
}

void ir_node_free(IrNode *node) {
    if (!node) return;
    ir_node_free(node->lhs);
    ir_node_free(node->rhs);
    ir_node_free(node->next);
    free(node);
}
