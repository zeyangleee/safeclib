/*------------------------------------------------------------------
 * memcpy32_s
 *
 * October 2008, Bo Berry
 * October 2017, Reini Urban
 *
 * Copyright (c) 2008-2011 Cisco Systems
 * Copyright (c) 2017,2018 Reini Urban
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 */

#ifdef FOR_DOXYGEN
#include "safe_mem_lib.h"
#else
#include "safeclib_private.h"
#include "mem/mem_primitives_lib.h"
#endif

/**
 * @def memcpy32_s(dest,dmax,src,slen)
 * @brief
 *    This function copies at most slen uint32_t's from src to dest, up to
 *    dmax.
 *
 * @remark EXTENSION TO
 *    ISO/IEC JTC1 SC22 WG14 N1172, Programming languages, environments
 *    and system software interfaces, Extensions to the C Library,
 *    Part I: Bounds-checking interfaces
 *
 * @param[out] dest   pointer to the memory that will be replaced by src.
 * @param[in]  dmax   maximum length of the resulting dest, in bytes
 * @param[in]  src    pointer to the memory that will be copied to dest
 * @param[in]  slen   number of uint32_t's to be copied
 *
 * @pre   Neither dest nor src shall be a null pointer.
 * @pre   dmax shall not be 0.
 * @pre   dmax shall not be greater than RSIZE_MAX_MEM and sizeof(dest)
 * @pre   slen shall not be greater than RSIZE_MAX_MEM32 and sizeof(src)
 * @pre   slen shall not be greater than dmax/4.
 * @pre   Copying shall not take place between regions that overlap.
 *
 * @return  If there is a runtime-constraint violation, memcpy32_s
 *          stores zeros in the first dmax bytes of the region pointed to
 *          by dest if dest and dmax are valid.
 * @retval  EOK         when operation is successful or slen = 0
 * @retval  ESNULLP     when dest/src is NULL POINTER
 * @retval  ESZEROL     when dmax = ZERO
 * @retval  ESLEMAX     when dmax > RSIZE_MAX_MEM or slen > RSIZE_MAX_MEM32
 * @retval  EOVERFLOW   when dmax > size of dest (optionally, when the compiler
 *                      knows the object_size statically).
 *                      Or when 4*slen > size of src (optionally, when the
 *                      compiler knows the object_size statically)
 * @retval  ESLEWRNG    when dmax != sizeof(dest) and --enable-error-dmax
 * @retval  ESNOSPC     when 4*slen > dmax
 * @retval  ESOVRLP     when src memory overlaps dest
 *
 * @see
 *    memcpy_s(), memcpy16_s(), memmove_s(), memmove16_s(), memmove32_s()
 *
 */
#ifdef FOR_DOXYGEN
errno_t memcpy32_s(uint32_t *dest, rsize_t dmax,
                   const uint32_t *src, rsize_t slen)
#else
EXPORT errno_t _memcpy32_s_chk(uint32_t *dest, rsize_t dmax,
                               const uint32_t *src, rsize_t slen,
                               const size_t destbos, const size_t srcbos)
#endif
{
    size_t smax; /* in bytes */

    if (unlikely(slen == 0)) { /* Since C11 slen=0 is allowed */
        return EOK;
    }
    CHK_DEST_MEM_NULL("memcpy32_s")
    CHK_DMAX_MEM_ZERO("memcpy32_s")
    smax = slen * 4;
    if (destbos == BOS_UNKNOWN) {
        CHK_DMAX_MEM_MAX("memcpy16_s", RSIZE_MAX_MEM)
        BND_CHK_PTR_BOUNDS(dest, dmax);
        BND_CHK_PTR_BOUNDS(dest, smax);
    } else {
        CHK_DEST_MEM_OVR("memcpy32_s", destbos)
        dmax = destbos;
    }
    CHK_SRC_MEM_NULL_CLEAR("memcpy32_s", src)
    CHK_SLEN_MEM_MAX_NOSPC_CLEAR("memcpy32_s", smax, RSIZE_MAX_MEM)

    if (srcbos == BOS_UNKNOWN) {
        BND_CHK_PTR_BOUNDS(src, smax);
    } else if (unlikely(smax > srcbos)) {
        invoke_safe_mem_constraint_handler("memcmp32_s: slen exceeds src",
                                           (void *)src, ESLEMAX);
        return (RCNEGATE(ESLEMAX));
    }

    /* overlap is disallowed, but allow dest==src */
    if (unlikely(CHK_OVRLP_BUTSAME(dest, dmax / 4, src, slen))) {
        mem_prim_set(dest, dmax, 0);
        MEMORY_BARRIER;
        invoke_safe_mem_constraint_handler("memcpy32_s: overlap undefined",
                                           (void *)dest, ESOVRLP);
        return (RCNEGATE(ESOVRLP));
    }

    /*
     * now perform the copy
     */
    mem_prim_move32(dest, src, slen);

    return (RCNEGATE(EOK));
}
