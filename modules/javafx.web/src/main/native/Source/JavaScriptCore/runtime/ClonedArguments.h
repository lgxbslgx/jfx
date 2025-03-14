/*
 * Copyright (C) 2015-2022 Apple Inc. All rights reserved.
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

#pragma once

#include "ArgumentsMode.h"
#include "JSObject.h"

namespace JSC {

// This is an Arguments-class object that we create when you do function.arguments, or you say
// "arguments" inside a function in strict mode. It behaves almpst entirely like an ordinary
// JavaScript object. All of the arguments values are simply copied from the stack (possibly via
// some sophisticated ValueRecovery's if an optimizing compiler is in play) and the appropriate
// properties of the object are populated. The only reason why we need a special class is to make
// the object claim to be "Arguments" from a toString standpoint, and to avoid materializing the
// caller/callee/@@iterator properties unless someone asks for them.
class ClonedArguments final : public JSNonFinalObject {
public:
    using Base = JSNonFinalObject;
    static constexpr unsigned StructureFlags = Base::StructureFlags | OverridesGetOwnPropertySlot | OverridesGetOwnSpecialPropertyNames | OverridesPut;

    template<typename CellType, SubspaceAccess mode>
    static GCClient::IsoSubspace* subspaceFor(VM& vm)
    {
        static_assert(!CellType::needsDestruction, "");
        return &vm.clonedArgumentsSpace();
    }

private:
    ClonedArguments(VM&, Structure*, Butterfly*);

public:
    static ClonedArguments* createEmpty(VM&, Structure*, JSFunction* callee, unsigned length);
    static ClonedArguments* createEmpty(JSGlobalObject*, JSFunction* callee, unsigned length);
    static ClonedArguments* createWithInlineFrame(JSGlobalObject*, CallFrame* targetFrame, InlineCallFrame*, ArgumentsMode);
    static ClonedArguments* createWithMachineFrame(JSGlobalObject*, CallFrame* targetFrame, ArgumentsMode);
    static ClonedArguments* createByCopyingFrom(JSGlobalObject*, Structure*, Register* argumentsStart, unsigned length, JSFunction* callee);

    static Structure* createStructure(VM&, JSGlobalObject*, JSValue prototype);
    static Structure* createSlowPutStructure(VM&, JSGlobalObject*, JSValue prototype);

    DECLARE_VISIT_CHILDREN;

    DECLARE_INFO;

private:
    static Structure* createStructure(VM&, JSGlobalObject*, JSValue prototype, IndexingType);

    static bool getOwnPropertySlot(JSObject*, JSGlobalObject*, PropertyName, PropertySlot&);
    static void getOwnSpecialPropertyNames(JSObject*, JSGlobalObject*, PropertyNameArray&, DontEnumPropertiesMode);
    static bool put(JSCell*, JSGlobalObject*, PropertyName, JSValue, PutPropertySlot&);
    static bool deleteProperty(JSCell*, JSGlobalObject*, PropertyName, DeletePropertySlot&);
    static bool defineOwnProperty(JSObject*, JSGlobalObject*, PropertyName, const PropertyDescriptor&, bool shouldThrow);

    bool specialsMaterialized() const { return !m_callee; }
    void materializeSpecials(JSGlobalObject*);
    void materializeSpecialsIfNecessary(JSGlobalObject*);

    WriteBarrier<JSFunction> m_callee; // Set to nullptr when we materialize all of our special properties.
};

static const PropertyOffset clonedArgumentsLengthPropertyOffset = 100;

} // namespace JSC
