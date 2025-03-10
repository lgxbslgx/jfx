/*
 * Copyright (C) 2021 Apple Inc. All rights reserved.
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

#include "IntRect.h"
#include <wtf/RefCounted.h>
#include <wtf/WeakPtr.h>

namespace WebCore {

class Document;
class FloatRect;
class HTMLElement;
class Node;
class SharedBuffer;
class VisibleSelection;

struct CharacterRange;
struct SimpleRange;
struct TextRecognitionResult;

namespace ImageOverlay {

WEBCORE_EXPORT bool hasOverlay(const HTMLElement&);
WEBCORE_EXPORT bool isDataDetectorResult(const HTMLElement&);
WEBCORE_EXPORT bool isInsideOverlay(const SimpleRange&);
WEBCORE_EXPORT bool isInsideOverlay(const Node&);
WEBCORE_EXPORT bool isOverlayText(const Node&);
WEBCORE_EXPORT bool isOverlayText(const Node*);
void removeOverlaySoonIfNeeded(HTMLElement&);
IntRect containerRect(HTMLElement&);
std::optional<CharacterRange> characterRange(const VisibleSelection&);
bool isInsideOverlay(const VisibleSelection&);

#if ENABLE(IMAGE_ANALYSIS)
enum class CacheTextRecognitionResults : bool { No, Yes };
WEBCORE_EXPORT void updateWithTextRecognitionResult(HTMLElement&, const TextRecognitionResult&, CacheTextRecognitionResults = CacheTextRecognitionResults::Yes);
#endif

class CroppedImage {
    WTF_MAKE_FAST_ALLOCATED;
    WTF_MAKE_NONCOPYABLE(CroppedImage);
public:
    WEBCORE_EXPORT static std::unique_ptr<CroppedImage> install(HTMLElement&, Ref<SharedBuffer>&&, const String&, FloatRect);

    CroppedImage(Document&, HTMLElement&, HTMLElement&, const String& imageURL);
    WEBCORE_EXPORT ~CroppedImage();

    WEBCORE_EXPORT void setVisibility(bool);

private:
    WeakPtr<Document> m_document;
    WeakPtr<HTMLElement> m_host;
    WeakPtr<HTMLElement> m_croppedImageBackdrop;
    String m_imageURL;
};

} // namespace ImageOverlay

} // namespace WebCore
