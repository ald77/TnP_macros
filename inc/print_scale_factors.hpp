#ifndef H_SF
#define H_SF

#include "TString.h"
#include "TDirectory.h"
#include "TString.h"
#include "TCanvas.h"

class TH1;
class TH2;

void PrintScaleFactors(const TString &file_ext);
void PrintDirectory(TDirectory &data_dir, TDirectory &mc_dir, const TString &ext = "");
void PrintObjects(TObject *data_obj, TObject *mc_obj, const TString &ext = "");
void PrintCanvas(TCanvas &data_can, TCanvas &mc_can, const TString &ext = "");
void Print2D(TH2 const * const h_data_in, TH2 const * const h_mc_in, const TString &ext = "");
void Print1D(TH1 const * const h_data_in, TH1 const * const h_mc_in, const TString &ext = "");
void GetPatrioticPalette();
void GetRainbowPalette();

#endif
