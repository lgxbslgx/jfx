/*
 * Copyright (c) 2021 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "pas_config.h"

#if LIBPAS_ENABLED

#include "pagesize64k_heap_config.h"

#if PAS_ENABLE_PAGESIZE64K

#include "pagesize64k_heap.h"
#include "pas_designated_intrinsic_heap.h"
#include "pas_heap_config_utils_inlines.h"

pas_heap_config pagesize64k_heap_config = PAGESIZE64K_HEAP_CONFIG;

PAS_BASIC_HEAP_CONFIG_DEFINITIONS(
    pagesize64k, PAGESIZE64K,
    .allocate_page_should_zero = false,
    .intrinsic_view_cache_capacity = pas_heap_runtime_config_zero_view_cache_capacity);

void pagesize64k_heap_config_activate(void)
{
    pas_designated_intrinsic_heap_initialize(&pagesize64k_common_primitive_heap.segregated_heap,
                                             &pagesize64k_heap_config);
}

#endif /* PAS_ENABLE_PAGESIZE64K */

#endif /* LIBPAS_ENABLED */
