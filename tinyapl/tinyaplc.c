/* tinyapl/tinyaplc.c - tinyAPL-to-C11 Compiler Driver
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "parser/tinyapl_parser.h"
#include "typecheck/typecheck.h"
#include "lowering/c11/c11_lowering.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    const char *src = "rho <- |psi|\nP <- +/ rho\n";
    char file_buf[4096];
    const char *out_path = NULL;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            out_path = argv[++i];
        } else {
            FILE *fp = fopen(argv[i], "rb");
            if (fp) {
                size_t n = fread(file_buf, 1, sizeof(file_buf) - 1, fp);
                file_buf[n] = '\0';
                fclose(fp);
                src = file_buf;
            }
        }
    }

    TinyAplEnv env = {0};
    env.syms[0] = (TinyAplSymbol){ "psi", 2, {128, 128}, CORRAL_C128 };
    env.num_syms = 1;

    char err[256] = {0};
    IrNode *prog = tinyapl_parse_program(src, &env, err, sizeof(err));
    if (!prog) {
        fprintf(stderr, "tinyaplc parse error: %s\n", err);
        return 1;
    }

    if (tinyapl_typecheck(prog, &env, err, sizeof(err)) != 0) {
        fprintf(stderr, "tinyaplc typecheck error: %s\n", err);
        ir_node_free(prog);
        return 1;
    }

    FILE *out = out_path ? fopen(out_path, "wb") : stdout;
    if (!out) {
        fprintf(stderr, "tinyaplc: cannot open output %s\n", out_path);
        ir_node_free(prog);
        return 1;
    }

    int rc = tinyapl_lower_to_c11(prog, out);
    if (out_path) fclose(out);
    ir_node_free(prog);
    return rc;
}
