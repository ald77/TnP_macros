#include "print_scale_factors.hpp"

#include <iomanip>
#include <fstream>

#include "TStyle.h"
#include "TColor.h"
#include "TFile.h"
#include "TString.h"
#include "TDirectory.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"

using namespace std;

namespace{
  const int bands = 999;
  int rainbow[bands];
  int patriotic[bands];
}

int main(){
  GetRainbowPalette();
  GetPatrioticPalette();
  gStyle->SetPaintTextFormat("2.3f");
  gStyle->SetNumberContours(bands);
  gStyle->SetPalette(bands, patriotic);

  PrintScaleFactors("veto_id");
  PrintScaleFactors("veto_iso");
  PrintScaleFactors("loose_id");
  PrintScaleFactors("loose_iso");
  PrintScaleFactors("medium_id");
  PrintScaleFactors("medium_iso");
  PrintScaleFactors("tight_id");
  PrintScaleFactors("tight_iso");
  // PrintScaleFactors("id");
  // PrintScaleFactors("loose");
  // PrintScaleFactors("veto");
  // PrintScaleFactors("medium");
  // PrintScaleFactors("tight");
}

void PrintScaleFactors(const TString &file_ext){
  TFile data_file("data/eff_data_"+file_ext+".root","read");
  TFile mc_file("data/eff_mc_"+file_ext+".root","read");
  if(!(data_file.IsOpen() && mc_file.IsOpen())) return;
  PrintDirectory(data_file, mc_file, file_ext, true);
}

void PrintDirectory(TDirectory &data_dir, TDirectory &mc_dir, const TString &ext, bool get_true){
  TList *keys = mc_dir.GetListOfKeys();
  if(keys == NULL) return;
  for(int i = 0; i <keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == NULL) continue;
    TString name = key->GetName();
    TString data_name = name;
    data_name.ReplaceAll("MCtruth_","");
    data_name.ReplaceAll("_and_mcTrue_true","");
    data_name.ReplaceAll("__mcTrue_true","");
    data_name.ReplaceAll("_mcTrue_true","");

    TObject *data_obj = data_dir.Get(data_name);
    if(data_obj == NULL) continue;
    PrintObjects(data_obj, mc_dir.Get(name), ext, get_true);
    if(data_name != name){
      PrintObjects(data_obj, mc_dir.Get(data_name), ext, false);
    }
  }
}

void PrintObjects(TObject *data_obj,TObject *mc_obj, const TString &ext, bool get_true){
  if(data_obj == NULL || mc_obj == NULL) return;
  TString class_name = data_obj->ClassName();
  TString mc_name = mc_obj->GetName();
  if(class_name == "TCanvas"){
    PrintCanvas(*static_cast<TCanvas*>(data_obj),
                *static_cast<TCanvas*>(mc_obj),
                ext+"_"+mc_name);
  }else if(class_name.Contains("TDirectory")){
    PrintDirectory(*static_cast<TDirectory*>(data_obj),
                   *static_cast<TDirectory*>(mc_obj),
                   ext+"_"+mc_name, get_true);
  }
}

void PrintCanvas(TCanvas &data_can, TCanvas &mc_can, const TString &ext){
  TList *list = mc_can.GetListOfPrimitives();
  if(list == NULL) return;
  for(int i = 0; i < list->GetSize(); ++i){
    TObject *prim = list->At(i);
    if(prim == NULL) continue;
    TString name = prim->GetName();
    TString data_name = name;
    data_name.ReplaceAll("MCtruth_","");
    data_name.ReplaceAll("_and_mcTrue_true","");
    data_name.ReplaceAll("__mcTrue_true","");
    data_name.ReplaceAll("_mcTrue_true","");

    TObject *data_obj = data_can.GetPrimitive(data_name);
    if(data_obj == NULL) continue;
    TObject *mc_obj = mc_can.GetPrimitive(name);
    if(mc_obj == NULL) continue;
    TString class_name = prim->ClassName();
    if(data_obj->ClassName() != class_name
       || mc_obj->ClassName() != class_name) continue;

    if(class_name.Contains("TH1")){
      Print1D(static_cast<TH1*>(data_obj),
              static_cast<TH1*>(mc_obj),
              ext+"_"+name);
    }else if(class_name.Contains("TH2")){
      Print2D(static_cast<TH2*>(data_obj),
              static_cast<TH2*>(mc_obj),
              ext+"_"+name);
    }
  }
}

void Print2D(TH2 const * const h_data_in, TH2 const * const h_mc_in, const TString &ext){
  if(h_data_in == NULL || h_mc_in == NULL) return;

  TH2 *h_data = static_cast<TH2*>(h_data_in->Clone());
  if(h_data == NULL) return;
  TH2 *h_mc = static_cast<TH2*>(h_mc_in->Clone());
  if(h_mc == NULL) return;

  TCanvas canvas;
  canvas.SetLogy();
  gStyle->SetPalette(bands, rainbow);
  h_data->SetMarkerSize(2);
  h_mc->SetMarkerSize(2);

  h_data->Draw("colz");
  h_data->Draw("textesame");
  canvas.Print("plots/2d_data_"+ext+".pdf");
  PrintTable(h_data, "data_"+ext);
  h_mc->Draw("colz");
  h_mc->Draw("textesame");
  canvas.Print("plots/2d_mc_"+ext+".pdf");
  PrintTable(h_mc, "mc_"+ext);

  gStyle->SetPalette(bands, patriotic);
  h_data->Divide(h_mc);
  h_data->SetMinimum(0.8);
  h_data->SetMaximum(1.25);
  canvas.SetLogz();
  h_data->Draw("colz");
  h_data->Draw("textesame");
  canvas.Print("plots/sf_"+ext+".pdf");
  PrintTable(h_data, "sf_"+ext);

  if(h_data != NULL){
    delete h_data;
    h_data = NULL;
  }
  if(h_mc != NULL){
    delete h_mc;
    h_mc = NULL;
  }
}

void Print1D(TH1 const * const h_data_in, TH1 const * const h_mc_in, const TString &ext){
  if(h_data_in == NULL || h_mc_in == NULL) return;

  TH1 *h_data = static_cast<TH1*>(h_data_in->Clone());
  if(h_data == NULL) return;
  TH1 *h_mc = static_cast<TH1*>(h_mc_in->Clone());
  if(h_mc == NULL) return;

  h_data->SetLineColor(kBlack);
  h_mc->SetLineColor(kRed);

  TCanvas canvas;
  h_mc->Draw();
  h_data->Draw("same");
  canvas.Print("plots/1d_"+ext+".pdf");

  if(h_data != NULL){
    delete h_data;
    h_data = NULL;
  }
  if(h_mc != NULL){
    delete h_mc;
    h_mc = NULL;
  }
}

void PrintTable(TH2 const * const histo, const TString &ext){
  int eta = ext.Index("abseta");
  int pt = ext.Index("_et_");
  if(eta<0 || pt<0 || pt<eta) return;

  ofstream file("tables/"+ext+".tex");
  file << "\\documentclass{article}\n\n";

  file << "\\begin{document}\n";
  file << "\\begin{table}\n";
  file << "  \\begin{tabular}{r|rrrrr}\n";
  file << "    \\hline\\hline\n";
  file << "    & \\multicolumn{5}{c}{$p_T$ [GeV]}\\\\\n";
  file << "    $|\\eta|$ & 10-20 & 20-30 & 30-40 & 40-50 & 50-200\\\\\n";
  file << "    \\hline\n";
  PrintLine(file, histo, 1, "0-1.442");
  PrintLine(file, histo, 2, "1.442-1.556");
  PrintLine(file, histo, 3, "1.556-2.5");
  file << "    \\hline\\hline\n";
  file << "  \\end{tabular}\n";
  file << "\\end{table}\n";
  file << "\\end{document}\n";

  file.flush();
  file.close();
}

void PrintLine(ofstream &file, TH2 const * const histo, int bin, const TString &label){
  if(!file.is_open()) return;

  if(histo == NULL || histo->GetNbinsX() < bin || histo->GetNbinsY()<5) return;
  file << "    " << label;
  for(int y = 1; y <= 5; ++y){
    file << " & $"
         << fixed << setprecision(3) << histo->GetBinContent(bin, y)
         << "\\pm"
         << fixed << setprecision(3) << histo->GetBinError(bin, y)
         << "$";
  }
  file << "\\\\\n";
}

void GetPatrioticPalette(){
  const unsigned num = 3;
  double red[num] = {0., 1., 1.};
  double green[num] = {0., 1., 0.};
  double blue[num] = {1., 1., 0.};
  double stops[num] = {0., 0.5, 1.};
  int fi = TColor::CreateGradientColorTable(num,stops,red,green,blue,bands);
  for(int i = 0; i < bands; ++i){
    patriotic[i] = fi + i;
  }
}

void GetRainbowPalette(){
  const unsigned num = 6;
  double red[num] =   {1.,0.,0.,0.,1.,1.};
  double green[num] = {0.,0.,1.,1.,1.,0.};
  double blue[num] =  {1.,1.,1.,0.,0.,0.};
  double stops[num] = {0.,0.2,0.4,0.6,0.8,1.};
  int fi = TColor::CreateGradientColorTable(num,stops,red,green,blue,bands);
  for(int i = 0; i < bands; ++i){
    rainbow[i] = fi+i;
  }
}
