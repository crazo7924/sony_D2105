//
// Copyright (c) 2002-2010 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "compiler/TranslatorHLSL.h"

//
// This function must be provided to create the actual
// compile object used by higher level code.  It returns
// a subclass of TCompiler.
//
//SOMC KK PATCH Patches_w16.5_begin
TCompiler* ConstructCompiler(
    ShShaderType type, ShShaderSpec spec, ShShaderOutput output)
{
  switch (output) {
    case SH_HLSL_OUTPUT:
      return new TranslatorHLSL(type, spec);
    default:
      return NULL;
  }
}
//SOMC KK PATCH Patches_w16.5_end

//
// Delete the compiler made by ConstructCompiler
//
void DeleteCompiler(TCompiler* compiler)
{
    delete compiler;
}
