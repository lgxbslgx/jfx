/*
 * Copyright (C) 2019-2020 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Highlight.h"
#include "HighlightVisibility.h"
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/text/AtomStringHash.h>

namespace WebCore {

class DOMMapAdapter;
class DOMString;
class Highlight;
class StaticRange;

class HighlightRegister : public RefCounted<HighlightRegister> {
public:
    static Ref<HighlightRegister> create() { return adoptRef(*new HighlightRegister); }

    void initializeMapLike(DOMMapAdapter&);
    void setFromMapLike(String&&, Ref<Highlight>&&);
    void clear();
    bool remove(const String&);


    HighlightVisibility highlightsVisibility() const { return m_highlightVisibility; }
#if ENABLE(APP_HIGHLIGHTS)
    WEBCORE_EXPORT void setHighlightVisibility(HighlightVisibility);
#endif

    WEBCORE_EXPORT void addAnnotationHighlightWithRange(Ref<StaticRange>&&);
    const HashMap<String, Ref<Highlight>>& map() const { return m_map; }

private:
    HighlightRegister() = default;
    HashMap<String, Ref<Highlight>> m_map;

    HighlightVisibility m_highlightVisibility { HighlightVisibility::Hidden };
};

}
