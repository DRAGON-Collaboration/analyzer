#ifndef ROOTANA_LINKDEF_H
#define ROOTANA_LINKDEF_H

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclasses;

#pragma link C++ class rootana;

#pragma link C++ class rootana::DataPointer+;
#pragma link C++ class rootana::SummaryHist+;
#pragma link C++ class rootana::HistParser+;
#pragma link C++ class rootana::HistBase+;

#pragma link C++ global rootana::gHead;
#pragma link C++ global rootana::gTail;
#pragma link C++ global rootana::gCoinc;

#pragma link C++ function rootana::DataPointer::New(Char_t);
#pragma link C++ function rootana::DataPointer::New(Short_t);
#pragma link C++ function rootana::DataPointer::New(Int_t);
#pragma link C++ function rootana::DataPointer::New(Long_t);
#pragma link C++ function rootana::DataPointer::New(Long64_t);

#pragma link C++ function rootana::DataPointer::New(UChar_t);
#pragma link C++ function rootana::DataPointer::New(UShort_t);
#pragma link C++ function rootana::DataPointer::New(UInt_t);
#pragma link C++ function rootana::DataPointer::New(ULong_t);
#pragma link C++ function rootana::DataPointer::New(ULong64_t);

#pragma link C++ function rootana::DataPointer::New(Float_t);
#pragma link C++ function rootana::DataPointer::New(Double_t);

#pragma link C++ function rootana::DataPointer::New(Char_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(Short_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(Int_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(Long_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(Long64_t*, unsigned);

#pragma link C++ function rootana::DataPointer::New(UChar_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(UShort_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(UInt_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(ULong_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(ULong64_t*, unsigned);

#pragma link C++ function rootana::DataPointer::New(Float_t*, unsigned);
#pragma link C++ function rootana::DataPointer::New(Double_t*, unsigned);


#endif
#endif
