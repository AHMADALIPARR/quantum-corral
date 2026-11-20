/* native/bridge/matlab_adapter.c - MATLAB Engine API Adapter
 *
 * Copyright (c) 2026 AHMAD ALI PARR / SNAPKITTYWEST
 * Licensed under BSL-1.0 and FSL-1.1-MIT (Competing Use waived).
 * See LICENSE and LICENSE.FSL.
 */
#include "matlab_adapter.h"
#include <string.h>

#ifdef USE_MATLAB_ENGINE
#include "engine.h"
#endif

int corral_matlab_eval_density(
    const CorralArray *re_row_major,
    const CorralArray *im_row_major,
    CorralArray *rho_row_major
) {
    if (!re_row_major || !im_row_major || !rho_row_major) return -1;
    if (re_row_major->rank != 2 || im_row_major->rank != 2 || rho_row_major->rank != 2) return -1;

    int64_t nx = re_row_major->shape[0];
    int64_t ny = re_row_major->shape[1];
    if (im_row_major->shape[0] != nx || im_row_major->shape[1] != ny) return -1;
    if (rho_row_major->shape[0] != nx || rho_row_major->shape[1] != ny) return -1;

#ifdef USE_MATLAB_ENGINE
    Engine *ep = engOpen(NULL);
    if (ep) {
        int64_t shape[2] = { nx, ny };
        CorralArray re_col = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_COL_MAJOR);
        CorralArray im_col = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_COL_MAJOR);
        CorralArray rho_col = corral_array_alloc(2, shape, CORRAL_F64, CORRAL_LAYOUT_COL_MAJOR);

        corral_array_transpose_layout(re_row_major, &re_col);
        corral_array_transpose_layout(im_row_major, &im_col);

        mxArray *mx_re = mxCreateDoubleMatrix((mwSize)nx, (mwSize)ny, mxREAL);
        mxArray *mx_im = mxCreateDoubleMatrix((mwSize)nx, (mwSize)ny, mxREAL);
        memcpy(mxGetPr(mx_re), re_col.data, (size_t)(nx * ny) * sizeof(double));
        memcpy(mxGetPr(mx_im), im_col.data, (size_t)(nx * ny) * sizeof(double));

        engPutVariable(ep, "re_in", mx_re);
        engPutVariable(ep, "im_in", mx_im);
        engEvalString(ep, "rho_out = corral_field(re_in, im_in);");

        mxArray *mx_rho = engGetVariable(ep, "rho_out");
        if (mx_rho) {
            memcpy(rho_col.data, mxGetPr(mx_rho), (size_t)(nx * ny) * sizeof(double));
            corral_array_transpose_layout(&rho_col, rho_row_major);
            mxDestroyArray(mx_rho);
        }

        mxDestroyArray(mx_re);
        mxDestroyArray(mx_im);
        corral_array_free(&re_col);
        corral_array_free(&im_col);
        corral_array_free(&rho_col);
        engClose(ep);
        return 0;
    }
#endif

    /* Layout-verified fallback calling shared C11/Fortran contract */
    corral_density(
        (const double *)re_row_major->data,
        (const double *)im_row_major->data,
        (double *)rho_row_major->data,
        (int)nx,
        (int)ny
    );
    return 0;
}
