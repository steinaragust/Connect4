// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME Connect4_rflx
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "Game.h"
#include "Game.cpp"

// Header files passed via #pragma extra_include

namespace CppyyLegacy {
   static TClass *Connect4_Dictionary();
   static void Connect4_TClassManip(TClass*);
   static void *new_Connect4(void *p = 0);
   static void *newArray_Connect4(Long_t size, void *p);
   static void delete_Connect4(void *p);
   static void deleteArray_Connect4(void *p);
   static void destruct_Connect4(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Connect4*)
   {
      ::Connect4 *ptr = 0;
      static ::CppyyLegacy::TVirtualIsAProxy* isa_proxy = new ::CppyyLegacy::TIsAProxy(typeid(::Connect4));
      static ::CppyyLegacy::TGenericClassInfo 
         instance("Connect4", "Game.h", 10,
                  typeid(::Connect4), ::CppyyLegacy::Internal::DefineBehavior(ptr, ptr),
                  &Connect4_Dictionary, isa_proxy, 4,
                  sizeof(::Connect4) );
      instance.SetNew(&new_Connect4);
      instance.SetNewArray(&newArray_Connect4);
      instance.SetDelete(&delete_Connect4);
      instance.SetDeleteArray(&deleteArray_Connect4);
      instance.SetDestructor(&destruct_Connect4);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Connect4*)
   {
      return GenerateInitInstanceLocal((::Connect4*)0);
   }
   // Static variable to force the class initialization
   static ::CppyyLegacy::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Connect4*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static ::CppyyLegacy::TClass *Connect4_Dictionary() {
      ::CppyyLegacy::TClass* theClass =::CppyyLegacy::GenerateInitInstanceLocal((const ::Connect4*)0x0)->GetClass();
      Connect4_TClassManip(theClass);
   return theClass;
   }

   static void Connect4_TClassManip(TClass* theClass){
      theClass->CreateAttributeMap();
      TDictAttributeMap* attrMap( theClass->GetAttributeMap() );
      attrMap->AddProperty("file_name","Game.h");
   }

} // end of namespace CppyyLegacy

namespace CppyyLegacy {
   // Wrappers around operator new
   static void *new_Connect4(void *p) {
      return  p ? new(p) ::Connect4 : new ::Connect4;
   }
   static void *newArray_Connect4(Long_t nElements, void *p) {
      return p ? new(p) ::Connect4[nElements] : new ::Connect4[nElements];
   }
   // Wrapper around operator delete
   static void delete_Connect4(void *p) {
      delete ((::Connect4*)p);
   }
   static void deleteArray_Connect4(void *p) {
      delete [] ((::Connect4*)p);
   }
   static void destruct_Connect4(void *p) {
      typedef ::Connect4 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace CppyyLegacy for class ::Connect4

namespace {
  void TriggerDictionaryInitialization_Connect4_rflx_Impl() {
    static const char* headers[] = {
"Game.h",
"Game.cpp",
0
    };
    static const char* includePaths[] = {
"/usr/local/lib/python3.9/site-packages/cppyy_backend/include/",
"/Users/SteinarA/Documents/Personal/Skóli/Vor 2021/Connect-4/c++/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "Connect4_rflx dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate(R"ATTRDUMP(pattern@@@*)ATTRDUMP"))) __attribute__((annotate(R"ATTRDUMP(file_name@@@Game.h)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$Game.h")))  Connect4;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "Connect4_rflx dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "Game.h"
#include "Game.cpp"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"", payloadCode, "@",
"Connect4", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      ::CppyyLegacy::TROOT::RegisterModule("Connect4_rflx",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_Connect4_rflx_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_Connect4_rflx_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_Connect4_rflx() {
  TriggerDictionaryInitialization_Connect4_rflx_Impl();
}
