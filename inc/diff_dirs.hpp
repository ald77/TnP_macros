#ifndef H_DIFF_DIRS
#define H_DIFF_DIRS

#include <fstream>
#include <string>
#include <vector>

#include "TString.h"
#include "TDirectory.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH2D.h"

class TH1;

void PrintScaleFactors(const std::vector<TString> &dirs, const TString &file_ext);
void PrintDirectory(const std::vector<TDirectory*> &data_dirs, const std::vector<TDirectory*> &mc_dirs,
		    const TString &ext, bool get_true);
void PrintObjects(const std::vector<TObject*> &data_objs, const std::vector<TObject*> &mc_objs,
		  const TString &ext, bool get_true);
void PrintCanvas(const std::vector<TCanvas*> &data_cans, const std::vector<TCanvas*> &mc_cans, const TString &ext);
TH2D TranslateHisto(const TH2 &input);
void Print2D(const std::vector<TH2*> &h_data_in, const std::vector<TH2*> &h_mc_in, const TString &ext = "");
void PrintTable(TH2 const * const histo, const TString &ext);
void PrintLine(std::ofstream &file, TH2 const * const histo, int bin, const TString &label);
void GetPatrioticPalette();
void GetRainbowPalette();

std::vector<double> RemoveOutliers(const std::vector<double> &v);
double Median(std::vector<double> v);
double GoodVal(const std::vector<double> &v);

std::string FixName();
template<typename To, typename From>
std::vector<To*> Convert(const std::vector<From*> &in){
  std::vector<To*> out(in.size());
  for(size_t i = 0; i < in.size(); ++i){
    out.at(i) = static_cast<To*>(in.at(i));
  }
  return out;
}

#endif
