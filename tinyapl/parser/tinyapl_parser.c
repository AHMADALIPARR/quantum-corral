/* tinyapl/parser/tinyapl_parser.c - Lexer & Recursive-Descent Parser for tinyAPL
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "tinyapl_parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOK_EOF = 0,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_ASSIGN,     /* <- or ← */
    TOK_RHO,        /* ⍴ */
    TOK_PLUS,
    TOK_MINUS,
    TOK_MUL,        /* * or × */
    TOK_BAR,        /* | */
    TOK_REDUCE_SUM, /* +/ */
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_NEWLINE
} TokKind;

typedef struct {
    TokKind kind;
    char text[64];
    double num;
} Token;

typedef struct {
    const char *p;
    Token cur;
} Lexer;

static void next_tok(Lexer *lx) {
    for (;;) {
        while (*lx->p == ' ' || *lx->p == '\t' || *lx->p == '\r') lx->p++;
        if (*lx->p == '#') {
            while (*lx->p && *lx->p != '\n') lx->p++;
            continue;
        }
        break;
    }
    if (*lx->p == '\0') { lx->cur.kind = TOK_EOF; return; }
    if (*lx->p == '\n') { lx->p++; lx->cur.kind = TOK_NEWLINE; return; }

    /* UTF-8 APL glyphs: ← (E2 86 90), ⍴ (E2 8D B4), × (C3 97) */
    const unsigned char *u = (const unsigned char *)lx->p;
    if (u[0] == 0xE2 && u[1] == 0x86 && u[2] == 0x90) {
        lx->p += 3; lx->cur.kind = TOK_ASSIGN; return;
    }
    if (u[0] == 0xE2 && u[1] == 0x8D && u[2] == 0xB4) {
        lx->p += 3; lx->cur.kind = TOK_RHO; return;
    }
    if (u[0] == 0xC3 && u[1] == 0x97) {
        lx->p += 2; lx->cur.kind = TOK_MUL; return;
    }

    if (lx->p[0] == '<' && lx->p[1] == '-') { lx->p += 2; lx->cur.kind = TOK_ASSIGN; return; }
    if (lx->p[0] == '+' && lx->p[1] == '/') { lx->p += 2; lx->cur.kind = TOK_REDUCE_SUM; return; }
    if (*lx->p == '+') { lx->p++; lx->cur.kind = TOK_PLUS; return; }
    if (*lx->p == '-') { lx->p++; lx->cur.kind = TOK_MINUS; return; }
    if (*lx->p == '*') { lx->p++; lx->cur.kind = TOK_MUL; return; }
    if (*lx->p == '|') { lx->p++; lx->cur.kind = TOK_BAR; return; }
    if (*lx->p == '(') { lx->p++; lx->cur.kind = TOK_LPAREN; return; }
    if (*lx->p == ')') { lx->p++; lx->cur.kind = TOK_RPAREN; return; }

    if (isdigit((unsigned char)*lx->p) || (*lx->p == '.' && isdigit((unsigned char)lx->p[1]))) {
        char *end = NULL;
        lx->cur.num = strtod(lx->p, &end);
        lx->p = end;
        lx->cur.kind = TOK_NUMBER;
        return;
    }

    if (isalpha((unsigned char)*lx->p) || *lx->p == '_') {
        size_t k = 0;
        while ((isalnum((unsigned char)*lx->p) || *lx->p == '_') && k < 63) {
            lx->cur.text[k++] = *lx->p++;
        }
        lx->cur.text[k] = '\0';
        lx->cur.kind = TOK_IDENT;
        return;
    }

    lx->p++;
    lx->cur.kind = TOK_EOF;
}

static const TinyAplSymbol *lookup_sym(const TinyAplEnv *env, const char *name) {
    if (!env) return NULL;
    for (int i = 0; i < env->num_syms; ++i) {
        if (strcmp(env->syms[i].name, name) == 0) return &env->syms[i];
    }
    return NULL;
}

static IrNode *parse_expr(Lexer *lx, const TinyAplEnv *env, char *err, size_t err_sz);

static IrNode *parse_primary(Lexer *lx, const TinyAplEnv *env, char *err, size_t err_sz) {
    if (lx->cur.kind == TOK_NUMBER) {
        IrNode *n = ir_node_new(IR_OP_CONST_F64, "const");
        n->scalar_val = lx->cur.num;
        n->rank = 0;
        n->dtype = CORRAL_F64;
        next_tok(lx);
        return n;
    }
    if (lx->cur.kind == TOK_IDENT) {
        char id[64];
        snprintf(id, sizeof(id), "%s", lx->cur.text);
        next_tok(lx);

        if (strcmp(id, "J0") == 0 && lx->cur.kind == TOK_LPAREN) {
            next_tok(lx);
            IrNode *arg = parse_expr(lx, env, err, err_sz);
            if (lx->cur.kind == TOK_RPAREN) next_tok(lx);
            IrNode *n = ir_node_new(IR_OP_EXTERN_BESSEL0, "J0");
            n->lhs = arg;
            return n;
        }
        if (strcmp(id, "J1") == 0 && lx->cur.kind == TOK_LPAREN) {
            next_tok(lx);
            IrNode *arg = parse_expr(lx, env, err, err_sz);
            if (lx->cur.kind == TOK_RPAREN) next_tok(lx);
            IrNode *n = ir_node_new(IR_OP_EXTERN_BESSEL1, "J1");
            n->lhs = arg;
            return n;
        }

        IrNode *n = ir_node_new(IR_OP_VAR, id);
        const TinyAplSymbol *sym = lookup_sym(env, id);
        if (sym) {
            n->rank = sym->rank;
            n->dtype = sym->dtype;
            memcpy(n->shape, sym->shape, sizeof(n->shape));
        }
        return n;
    }
    if (lx->cur.kind == TOK_BAR) {
        next_tok(lx);
        IrNode *inner = parse_expr(lx, env, err, err_sz);
        if (lx->cur.kind == TOK_BAR) next_tok(lx);
        IrNode *n = ir_node_new(IR_OP_MAG2, "mag2");
        n->lhs = inner;
        return n;
    }
    if (lx->cur.kind == TOK_REDUCE_SUM) {
        next_tok(lx);
        IrNode *inner = parse_expr(lx, env, err, err_sz);
        IrNode *n = ir_node_new(IR_OP_REDUCE_SUM, "sum");
        n->lhs = inner;
        return n;
    }
    if (lx->cur.kind == TOK_LPAREN) {
        next_tok(lx);
        IrNode *inner = parse_expr(lx, env, err, err_sz);
        if (lx->cur.kind == TOK_RPAREN) next_tok(lx);
        return inner;
    }
    snprintf(err, err_sz, "Unexpected token in primary expression");
    return NULL;
}

static IrNode *parse_expr(Lexer *lx, const TinyAplEnv *env, char *err, size_t err_sz) {
    IrNode *left = parse_primary(lx, env, err, err_sz);
    if (!left) return NULL;

    while (lx->cur.kind == TOK_PLUS || lx->cur.kind == TOK_MINUS ||
           lx->cur.kind == TOK_MUL || lx->cur.kind == TOK_RHO) {
        TokKind op = lx->cur.kind;
        next_tok(lx);
        IrNode *right = parse_expr(lx, env, err, err_sz); /* APL right-to-left precedence */
        if (!right) { ir_node_free(left); return NULL; }
        IrOpKind ir_op = IR_OP_ADD;
        if (op == TOK_MINUS) ir_op = IR_OP_SUB;
        else if (op == TOK_MUL) ir_op = IR_OP_MUL;
        else if (op == TOK_RHO) ir_op = IR_OP_RESHAPE;

        IrNode *bin = ir_node_new(ir_op, "binop");
        bin->lhs = left;
        bin->rhs = right;
        return bin;
    }
    return left;
}

IrNode *tinyapl_parse_program(const char *src, TinyAplEnv *env, char *err_buf, size_t err_sz) {
    Lexer lx = { .p = src };
    next_tok(&lx);

    IrNode *head = NULL, *tail = NULL;
    while (lx.cur.kind != TOK_EOF) {
        while (lx.cur.kind == TOK_NEWLINE) next_tok(&lx);
        if (lx.cur.kind == TOK_EOF) break;

        if (lx.cur.kind != TOK_IDENT) {
            snprintf(err_buf, err_sz, "Expected identifier on LHS of assignment");
            ir_node_free(head);
            return NULL;
        }
        char lhs_name[64];
        snprintf(lhs_name, sizeof(lhs_name), "%s", lx.cur.text);
        next_tok(&lx);

        if (lx.cur.kind != TOK_ASSIGN) {
            snprintf(err_buf, err_sz, "Expected '<-' or '\u2190' after '%s'", lhs_name);
            ir_node_free(head);
            return NULL;
        }
        next_tok(&lx);

        IrNode *rhs = parse_expr(&lx, env, err_buf, err_sz);
        if (!rhs) { ir_node_free(head); return NULL; }

        IrNode *assign = ir_node_new(IR_OP_ASSIGN, lhs_name);
        assign->lhs = rhs;

        if (!head) head = tail = assign;
        else { tail->next = assign; tail = assign; }
    }
    return head;
}
