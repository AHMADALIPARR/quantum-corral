/* verification/abi/test_abi_layout.c - Verify CorralArray ABI & Row/Col Layout
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "corral.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

int main(void) {
    int64_t shape[2] = { 4, 3 };
    CorralArray row = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);
    CorralArray col = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_COL_MAJOR);
    CorralArray roundtrip = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_ROW_MAJOR);

    assert(row.rank == 2 && row.numel == 12);
    assert(row.strides[0] == 3 && row.strides[1] == 1);
    assert(col.strides[0] == 1 && col.strides[1] == 4);

    double *r = (double *)row.data;
    for (int i = 0; i < 12; ++i) r[i] = (double)(i + 1);

    assert(corral_array_transpose_layout(&row, &col) == 0);
    assert(corral_array_transpose_layout(&col, &roundtrip) == 0);

    double *rt = (double *)roundtrip.data;
    for (int i = 0; i < 12; ++i) {
        assert(fabs(rt[i] - r[i]) < 1e-12);
    }

    corral_array_free(&row);
    corral_array_free(&col);
    corral_array_free(&roundtrip);
    printf("[PASS] ABI layout & stride round-trip verification\n");
    return 0;
}
