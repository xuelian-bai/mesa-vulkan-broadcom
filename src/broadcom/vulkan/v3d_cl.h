/*
 * Copyright © 2014-2017 Broadcom
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef VC5_CL_H
#define VC5_CL_H

#include <stdint.h>
#include "v3d_bo.h"

struct v3d_job;
struct v3d_cl;

/**
 * Undefined structure, used for typechecking that you're passing the pointers
 * to these functions correctly.
 */
struct v3d_cl_out;

/** A reference to a BO used in the CL packing functions */
struct v3d_cl_reloc {
        struct v3d_bo *bo;
        uint32_t offset;
};

/**
 * Reference to a BO with its associated offset, used in the pack process.
 */
static inline struct v3d_cl_reloc
cl_address(struct v3d_bo *bo, uint32_t offset)
{
        struct v3d_cl_reloc reloc = {
                .bo = bo,
                .offset = offset,
        };
        return reloc;
}

static inline void cl_pack_emit_reloc(struct v3d_cl *cl, const struct v3d_cl_reloc *);

#define __gen_user_data struct v3d_cl
#define __gen_address_type struct v3d_cl_reloc
#define __gen_address_offset(reloc) (((reloc)->bo ? (reloc)->bo->offset : 0) + \
                                     (reloc)->offset)
#define __gen_emit_reloc cl_pack_emit_reloc

struct v3d_cl {
        void *base;
        struct v3d_job *job;
        struct v3d_cl_out *next;
        struct v3d_bo *bo;
        uint32_t size;
};

void v3d_job_add_bo(struct v3d_job *job, struct v3d_bo *bo);

/**
 * Helper function called by the XML-generated pack functions for filling in
 * an address field in shader records.
 *
 * Since we have a private address space as of VC5, our BOs can have lifelong
 * offsets, and all the kernel needs to know is which BOs need to be paged in
 * for this exec.
 */
static inline void
cl_pack_emit_reloc(struct v3d_cl *cl, const struct v3d_cl_reloc *reloc)
{
        if (reloc->bo)
                v3d_job_add_bo(cl->job, reloc->bo);
}

#endif /* VC5_CL_H */