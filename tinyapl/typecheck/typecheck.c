/* tinyapl/typecheck/typecheck.c - Rank, Shape, Broadcasting & Complex Typechecker
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "typecheck.h"
#include <stdio.h>
#include <string.h>

static int infer_node(IrNode *n, char *err, size_t err_sz) {
    if (!n) return 0;
    if (n->lhs && infer_node(n->lhs, err, err_sz) != 0) return -1;
    if (n->rhs && infer_node(n->rhs, err, err_sz) != 0) return -1;

    switch (n->op) {
        case IR_OP_CONST_F64:
        case IR_OP_VAR:
            return 0;

        case IR_OP_EXTERN_BESSEL0:
        case IR_OP_EXTERN_BESSEL1:
            n->rank = n->lhs->rank;
            memcpy(n->shape, n->lhs->shape, sizeof(n->shape));
            n->dtype = CORRAL_F64;
            return 0;

        case IR_OP_MAG2:
            n->rank = n->lhs->rank;
            memcpy(n->shape, n->lhs->shape, sizeof(n->shape));
            n->dtype = CORRAL_F64;
            return 0;

        case IR_OP_REDUCE_SUM:
            n->rank = 0;
            n->dtype = CORRAL_F64;
            return 0;

        case IR_OP_ADD:
        case IR_OP_SUB:
        case IR_OP_MUL: {
            IrNode *a = n->lhs;
            IrNode *b = n->rhs;
            if (a->rank == 0) {
                n->rank = b->rank;
                memcpy(n->shape, b->shape, sizeof(n->shape));
            } else if (b->rank == 0) {
                n->rank = a->rank;
                memcpy(n->shape, a->shape, sizeof(n->shape));
            } else {
                if (a->rank != b->rank) {
                    snprintf(err, err_sz, "Rank mismatch: %d vs %d", a->rank, b->rank);
                    return -1;
                }
                for (int i = 0; i < a->rank; ++i) {
                    if (a->shape[i] != b->shape[i]) {
                        snprintf(err, err_sz, "Shape mismatch at axis %d: %lld vs %lld",
                                 i, (long long)a->shape[i], (long long)b->shape[i]);
                        return -1;
                    }
                }
                n->rank = a->rank;
                memcpy(n->shape, a->shape, sizeof(n->shape));
            }
            n->dtype = (a->dtype == CORRAL_C128 || b->dtype == CORRAL_C128) ? CORRAL_C128 : CORRAL_F64;
            return 0;
        }

        case IR_OP_RESHAPE:
            n->rank = n->rhs->rank;
            memcpy(n->shape, n->rhs->shape, sizeof(n->shape));
            n->dtype = n->rhs->dtype;
            return 0;

        case IR_OP_ASSIGN:
            n->rank = n->lhs->rank;
            memcpy(n->shape, n->lhs->shape, sizeof(n->shape));
            n->dtype = n->lhs->dtype;
            return 0;
    }
    return 0;
}

int tinyapl_typecheck(IrNode *prog, TinyAplEnv *env, char *err_buf, size_t err_sz) {
    for (IrNode *stmt = prog; stmt != NULL; stmt = stmt->next) {
        if (infer_node(stmt, err_buf, err_sz) != 0) return -1;
        if (env && env->num_syms < 32) {
            int found = -1;
            for (int i = 0; i < env->num_syms; ++i) {
                if (strcmp(env->syms[i].name, stmt->name) == 0) { found = i; break; }
            }
            int idx = (found >= 0) ? found : env->num_syms++;
            snprintf(env->syms[idx].name, sizeof(env->syms[idx].name), "%s", stmt->name);
            env->syms[idx].rank = stmt->rank;
            env->syms[idx].dtype = stmt->dtype;
            memcpy(env->syms[idx].shape, stmt->shape, sizeof(stmt->shape));
        }
    }
    return 0;
}
