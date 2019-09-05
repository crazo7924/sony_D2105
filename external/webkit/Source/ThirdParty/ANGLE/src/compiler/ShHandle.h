//
// Copyright (c) 2002-2012 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef _SHHANDLE_INCLUDED_
#define _SHHANDLE_INCLUDED_

//
// Machine independent part of the compiler private objects
// sent as ShHandle to the driver.
//
// This should not be included by driver code.
//

#include "GLSLANG/ShaderLang.h"

#include "compiler/BuiltInFunctionEmulator.h"    //SOMC KK PATCH Patches_w16.5
#include "compiler/ExtensionBehavior.h"
#include "compiler/InfoSink.h"
#include "compiler/SymbolTable.h"
#include "compiler/VariableInfo.h"

class LongNameMap;    //SOMC KK PATCH Patches_w16.5
class TCompiler;

//
// The base class used to back handles returned to the driver.
//
class TShHandleBase {
public:
    TShHandleBase();
    virtual ~TShHandleBase();
    virtual TCompiler* getAsCompiler() { return 0; }

protected:
    // Memory allocator. Allocates and tracks memory required by the compiler.
    // Deallocates all memory when compiler is destructed.
    TPoolAllocator allocator;
};

//
// The base class for the machine dependent compiler to derive from
// for managing object code from the compile.
//
class TCompiler : public TShHandleBase {
public:
    TCompiler(ShShaderType type, ShShaderSpec spec);
    virtual ~TCompiler();
    virtual TCompiler* getAsCompiler() { return this; }

    bool Init(const ShBuiltInResources& resources);
    bool compile(const char* const shaderStrings[],
                 const int numStrings,
                 int compileOptions);

    // Get results of the last compilation.
    TInfoSink& getInfoSink() { return infoSink; }
    const TVariableInfoList& getAttribs() const { return attribs; }
    const TVariableInfoList& getUniforms() const { return uniforms; }
    int getMappedNameMaxLength() const;    //SOMC KK PATCH Patches_w16.5

protected:
    ShShaderType getShaderType() const { return shaderType; }
    ShShaderSpec getShaderSpec() const { return shaderSpec; }
    // Initialize symbol-table with built-in symbols.
    bool InitBuiltInSymbolTable(const ShBuiltInResources& resources);
    // Clears the results from the previous compilation.
    void clearResults();
//SOMC KK PATCH Patches_w16.5_begin
    // Return true if function recursion is detected. 
    bool detectRecursion(TIntermNode* root);
//SOMC KK PATCH Patches_w16.5_end
    // Returns true if the given shader does not exceed the minimum
    // functionality mandated in GLSL 1.0 spec Appendix A.
    bool validateLimitations(TIntermNode* root);
    // Collect info for all attribs and uniforms.
    void collectAttribsUniforms(TIntermNode* root);
//SOMC KK PATCH Patches_w16.5_begin
    // Map long variable names into shorter ones. 
    void mapLongVariableNames(TIntermNode* root);
//SOMC KK PATCH Patches_w16.5_end
    // Translate to object code.
    virtual void translate(TIntermNode* root) = 0;
//SOMC KK PATCH Patches_w16.5_begin
    // Get built-in extensions with default behavior. 
    const TExtensionBehavior& getExtensionBehavior() const;

    const BuiltInFunctionEmulator& getBuiltInFunctionEmulator() const;
//SOMC KK PATCH Patches_w16.5_end

private:
    ShShaderType shaderType;
    ShShaderSpec shaderSpec;

    // Built-in symbol table for the given language, spec, and resources.
    // It is preserved from compile-to-compile.
    TSymbolTable symbolTable;
    // Built-in extensions with default behavior.
    TExtensionBehavior extensionBehavior;

    BuiltInFunctionEmulator builtInFunctionEmulator;    //SOMC KK PATCH Patches_w16.5

    // Results of compilation.
    TInfoSink infoSink;  // Output sink.
    TVariableInfoList attribs;  // Active attributes in the compiled shader.
    TVariableInfoList uniforms;  // Active uniforms in the compiled shader.

//SOMC KK PATCH Patches_w16.5_begin
    // Cached copy of the ref-counted singleton. 
    LongNameMap* longNameMap;
//SOMC KK PATCH Patches_w16.5_end
};

//
// This is the interface between the machine independent code
// and the machine dependent code.
//
// The machine dependent code should derive from the classes
// above. Then Construct*() and Delete*() will create and 
// destroy the machine dependent objects, which contain the
// above machine independent information.
//
TCompiler* ConstructCompiler(
    ShShaderType type, ShShaderSpec spec, ShShaderOutput output);
void DeleteCompiler(TCompiler*);

#endif // _SHHANDLE_INCLUDED_ 