////////////////////////////////////////////////////////////////////////////////
/// \file rootlogon.C
/// \author D. Connolly
/// \brief Example ROOT startup script
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// Example ROOT startup script to automatically load libDragon.so when an
/// interactive ROOT session is started. Place this script wherever you want,
/// but make sure you alter the path in .rootrc accordingly

#ifndef DOXYGEN_SKIP
{
  cout << "-------------------------------------------------\n";
  cout << "|\t Welcome to ROOT version " << gROOT->GetVersion() << " \t|\n";
  cout << "-------------------------------------------------\n";
  cout << endl;

  //  gStyle->SetPalette(1);
  //  gSystem->Exec("date");

  // Load lMathMore
  gSystem->Load("libMathMore");
  // Load lgsl
  if(gSystem->GetFromPipe("root-config --platform") == "macosx"){
    gSystem->Load("/usr/local/lib/libgsl.dylib");
  } else if(gSystem->GetFromPipe("root-config --platform") == "linux"){
    if(gSystem->GetFromPipe("lsb_release -si") == "Ubuntu"){
      gSystem->Load("/usr/lib64/libgsl.so");
    } else gSystem->Load("/usr/lib/x86_64-linux-gnu/libgsl.so");
  }

  // // Load RooFit
  // if(!TClass::GetClass("RooReal")){
  //   gSystem->Load("libRooFit.so");
  //   using namespace RooFit;
  // }


  if(gROOT->GetVersionInt() < 60000 ){
    // Set default include path
    // This assume you have set the environment variable DRAGONSYS to where your dragon analyzer is installed
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}/src/"));
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}/src/utils/"));
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}src/midas"));
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}/lib/"));

    // Load dragon libraries //
    if(!TClass::GetClass("dragon::Coinc")){
      gSystem->Load("libDragon.so");
    }
  } else {
    // Set default include path
    // This assume you have set the environment variable DRAGONSYS to where your dragon analyzer is installed
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}/src/"));
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}/src/utils/"));
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}src/midas"));
    gInterpreter->AddIncludePath(gSystem->ExpandPathName("${DRAGONSYS}/lib/"));

    // If you have *both* ROOT 5.xx/xx and ROOT 6.xx/xx installed, uncomment the 4 lines below and comment 4 the lines above
    // gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/packages/dragon/analyzer6/src/"));
    // gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/packages/dragon/analyzer6/src/midas"));
    // gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/packages/dragon/analyzer6/src/utils/"));
    // gInterpreter->AddIncludePath(gSystem->ExpandPathName("${HOME}/packages/dragon/analyzer6/lib/"));

    // In ROOT6, it seems to be necessary to load header files in order to use some classes interactively
    // gROOT->ProcessLine("#include <Database.hxx>");
    // gROOT->ProcessLine("#include <RootAnalysis.hxx>");
    // gROOT->ProcessLine("#include <Dragon.hxx>");
    // gROOT->ProcessLine("#include <TStamp.hxx>");
    // gROOT->ProcessLine("#include <stdint.h>"); // for uint32_t, etc.

    // Load dragon libraries //
    if(!TClass::GetClass("dragon::Coinc")){
      gSystem->Load("libDragon.so");
    }
  }

  //===== COLOR PALETTES =====//
  gStyle->SetNumberContours(99);
  // gStyle->SetPalette(50);
  gStyle->SetPalette(51); // deep sea
  // gStyle->SetPalette(52); // gray-scale
  // gStyle->SetPalette(53); // darkbody radiator
  // gStyle->SetPalette(54); // 2 hue darkblue -> brightyellow
  // gStyle->SetPalette(55); // Rainbow palette
  // gStyle->SetPalette(56); // inverted darkbody radiator
  // gStyle->SetPalette(60);

  // Set some style preferences //
  gROOT->SetStyle("Modern");
  gStyle->SetLineWidth(2);
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetHistLineColor(1);
  gStyle->SetHistLineWidth(2);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetFuncColor(1);
  gStyle->SetFuncWidth(2);

  gStyle->SetCanvasBorderMode(1);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTopMargin(0.1);
  gStyle->SetPadBottomMargin(0.1);
  gStyle->SetPadLeftMargin(0.105);
  gStyle->SetPadRightMargin(0.105);

  gStyle->SetStatBorderSize(0);
  gStyle->SetStatX(0.88);
  gStyle->SetStatY(0.88);
  gStyle->SetStatH(0.15);
  gStyle->SetStatW(0.2);

  Float_t font = 0.04;
  gStyle->SetLabelSize(font,"X");
  gStyle->SetLabelSize(font,"Y");
  gStyle->SetLabelSize(font,"Z");
  gStyle->SetTextSize(font);

  gStyle->SetTitleSize(font,"X");
  gStyle->SetTitleSize(font,"Y");
  gStyle->SetTitleSize(font,"Z");

  gStyle->SetTitleOffset(1.2,"X");
  gStyle->SetTitleOffset(1.2,"Y");
  gStyle->SetOptStat("emri");
  gStyle->SetOptTitle(kTRUE);

  //===== COLORS =====//
  // TRIUMF primary colour palette (+ black && white)
  TColor *tr_cyan      = new TColor(2011,58/255.0, 193/255.0, 227/255.0); // Pantone Process Cyan
  // TRIUMF secondary colour palette
  TColor *cool_gray_10 = new TColor(2012,128/255.0, 130/255.0, 133/255.0);
  TColor *cool_gray_2  = new TColor(2013,209/255.0, 211/255.0, 212/255.0);
  TColor *cool_gray_1  = new TColor(2014,241/255.0, 242/255.0, 242/255.0);
  TColor *tr_yellow    = new TColor(2015,225/255.0, 203/255.0, 5/255.0); // Pantone P 7-8 C
  // TRIUMF tertiary colour palette
  TColor *tr_green     = new TColor(2016,117/255.0, 192/255.0, 67/255.0); //Pantone 376 C
  TColor *tr_navy      = new TColor(2017,35/255.0, 35/255.0, 89/255.0); // Pantone P 105-8 C
  TColor *tr_salmon    = new TColor(2018,243/255.0, 113/255.0, 94/255.0); //Pantone P 55-6 C
  TColor *tr_orange    = new TColor(2019,217/255.0, 132/255.0, 76/255.0); //Pantone P 55-6 C
  // TRIUMF old color palette
  TColor *tr_blue      = new TColor(2020,0.067,0.247,0.486);
  TColor *tr_wine      = new TColor(2021,0.455,0.141,0.122);
  TColor *tr_green2    = new TColor(2022,0.314,0.502,0.255);
  TColor *tr_orange2   = new TColor(2023,0.839,0.38,0.18);
  TColor *tr_gray1     = new TColor(2024,0.353,0.314,0.318);
  TColor *tr_blue2     = new TColor(2025,0.121,0.285,0.488);

}

#endif // DOXYGEN_SKIP
