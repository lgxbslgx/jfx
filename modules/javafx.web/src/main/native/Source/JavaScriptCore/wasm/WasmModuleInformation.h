/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
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

#if ENABLE(WEBASSEMBLY)

#include "WasmBranchHints.h"
#include "WasmFormat.h"

#include <wtf/BitVector.h>
#include <wtf/HashMap.h>

namespace JSC { namespace Wasm {

struct ModuleInformation : public ThreadSafeRefCounted<ModuleInformation> {

    using BranchHints = HashMap<uint32_t, BranchHintMap, IntHash<uint32_t>, WTF::UnsignedWithZeroKeyHashTraits<uint32_t>>;

    ModuleInformation();
    ModuleInformation(const ModuleInformation&) = delete;
    ModuleInformation(ModuleInformation&&) = delete;

    static Ref<ModuleInformation> create()
    {
        return adoptRef(*new ModuleInformation);
    }

    JS_EXPORT_PRIVATE ~ModuleInformation();

    size_t functionIndexSpaceSize() const { return importFunctionSignatureIndices.size() + internalFunctionSignatureIndices.size(); }
    bool isImportedFunctionFromFunctionIndexSpace(size_t functionIndex) const
    {
        ASSERT(functionIndex < functionIndexSpaceSize());
        return functionIndex < importFunctionSignatureIndices.size();
    }
    SignatureIndex signatureIndexFromFunctionIndexSpace(size_t functionIndex) const
    {
        return isImportedFunctionFromFunctionIndexSpace(functionIndex)
            ? importFunctionSignatureIndices[functionIndex]
            : internalFunctionSignatureIndices[functionIndex - importFunctionSignatureIndices.size()];
    }

    size_t exceptionIndexSpaceSize() const { return importExceptionSignatureIndices.size() + internalExceptionSignatureIndices.size(); }
    bool isImportedExceptionFromExceptionIndexSpace(size_t exceptionIndex) const
    {
        ASSERT(exceptionIndex < exceptionIndexSpaceSize());
        return exceptionIndex < importExceptionSignatureIndices.size();
    }
    SignatureIndex signatureIndexFromExceptionIndexSpace(size_t exceptionIndex) const
    {
        return isImportedExceptionFromExceptionIndexSpace(exceptionIndex)
            ? importExceptionSignatureIndices[exceptionIndex]
            : internalExceptionSignatureIndices[exceptionIndex - importExceptionSignatureIndices.size()];
    }

    uint32_t importFunctionCount() const { return importFunctionSignatureIndices.size(); }
    uint32_t internalFunctionCount() const { return internalFunctionSignatureIndices.size(); }
    uint32_t importExceptionCount() const { return importExceptionSignatureIndices.size(); }
    uint32_t internalExceptionCount() const { return internalExceptionSignatureIndices.size(); }

    // Currently, our wasm implementation allows only one memory and table.
    // If we need to remove this limitation, we would have MemoryInformation and TableInformation in the Vectors.
    uint32_t memoryCount() const { return memory ? 1 : 0; }
    uint32_t tableCount() const { return tables.size(); }
    uint32_t elementCount() const { return elements.size(); }
    uint32_t dataSegmentsCount() const { return numberOfDataSegments; }

    const TableInformation& table(unsigned index) const { return tables[index]; }

    const BitVector& referencedFunctions() const { return m_referencedFunctions; }
    void addReferencedFunction(unsigned index) const { m_referencedFunctions.set(index); }

    bool isDeclaredFunction(uint32_t index) const { return m_declaredFunctions.contains(index); }
    void addDeclaredFunction(uint32_t index) { m_declaredFunctions.set(index); }

    bool isDeclaredException(uint32_t index) const { return m_declaredExceptions.contains(index); }
    void addDeclaredException(uint32_t index) { m_declaredExceptions.set(index); }

    bool hasMemoryImport() const { return memory.isImport(); }

    BranchHint getBranchHint(uint32_t functionOffset, uint32_t branchOffset) const
    {
        auto it = branchHints.find(functionOffset);
        return it == branchHints.end()
            ? BranchHint::Invalid
            : it->value.getBranchHint(branchOffset);
    }

    Vector<Import> imports;
    Vector<SignatureIndex> importFunctionSignatureIndices;
    Vector<SignatureIndex> internalFunctionSignatureIndices;
    Vector<SignatureIndex> importExceptionSignatureIndices;
    Vector<SignatureIndex> internalExceptionSignatureIndices;
    Vector<Ref<Signature>> usedSignatures;

    MemoryInformation memory;

    Vector<FunctionData> functions;

    Vector<Export> exports;
    std::optional<uint32_t> startFunctionIndexSpace;
    Vector<Segment::Ptr> data;
    Vector<Element> elements;
    Vector<TableInformation> tables;
    Vector<GlobalInformation> globals;
    unsigned firstInternalGlobal { 0 };
    uint32_t codeSectionSize { 0 };
    Vector<CustomSection> customSections;
    Ref<NameSection> nameSection;
    BranchHints branchHints;
    uint32_t numberOfDataSegments { 0 };

    BitVector m_declaredFunctions;
    BitVector m_declaredExceptions;
    mutable BitVector m_referencedFunctions;
};


} } // namespace JSC::Wasm

#endif // ENABLE(WEBASSEMBLY)
