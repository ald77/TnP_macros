#ifndef H_SF
#define H_SF

#include <fstream>

#include "TString.h"
#include "TDirectory.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH2D.h"

class TH1;

void PrintScaleFactors(const TString &file_ext);
void PrintDirectory(TDirectory &data_dir, TDirectory &mc_dir, const TString &ext, bool get_true);
void PrintObjects(TObject *data_obj, TObject *mc_obj, const TString &ext, bool get_true);
void PrintCanvas(TCanvas &data_can, TCanvas &mc_can, const TString &ext);
TH2D TranslateHisto(const TH2 &input);
void Print2D(TH2 const * const h_data_in, TH2 const * const h_mc_in, const TString &ext = "");
void Print1D(TH1 const * const h_data_in, TH1 const * const h_mc_in, const TString &ext = "");
void PrintTable(TH2 const * const histo, const TString &ext);
void PrintLine(std::ofstream &file, TH2 const * const histo, int bin, const TString &label);
void GetPatrioticPalette();
void GetRainbowPalette();

#endif
