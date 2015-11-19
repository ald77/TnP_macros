#include "print_scale_factors.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

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

  PrintScaleFactors("foid2d");
  PrintScaleFactors("loose");
  PrintScaleFactors("loose2d");
  PrintScaleFactors("medium");
  PrintScaleFactors("mvavlooseconvihit1_act");
  PrintScaleFactors("mvavlooseconvihit1_eta");
  PrintScaleFactors("mvavloosemini4_act");
  PrintScaleFactors("mvavloosemini4_eta");
  PrintScaleFactors("mvavloosemini_act");
  PrintScaleFactors("mvavloosemini_eta");
  PrintScaleFactors("mvatightconvihit0chg_act");
  PrintScaleFactors("mvatightconvihit0chg_eta");
  PrintScaleFactors("mvatightmulti_act");
  PrintScaleFactors("mvatightmulti_eta");
  PrintScaleFactors("mvatightmultiemu_act");
  PrintScaleFactors("mvatightmultiemu_eta");
  PrintScaleFactors("tight");
  PrintScaleFactors("tight2d3d");
  PrintScaleFactors("tightid2d3d");
  PrintScaleFactors("veto");
}

void PrintScaleFactors(const TString &file_ext){
  TFile data_file("data/eff_data_"+file_ext+".root","read");
  TFile mc_file("data/eff_mc_"+file_ext+".root","read");
  if(!(data_file.IsOpen() && mc_file.IsOpen())) return;
  PrintDirectory(data_file, mc_file, file_ext, true);
}

void PrintDirectory(TDirectory &data_dir, TDirectory &mc_dir, const TString &ext, bool get_true){
  string data_path = data_dir.GetPath();
  string mc_path = mc_dir.GetPath();
  if(data_path.find("cnt_enf") != string::npos
     || mc_path.find("cnt_eff") != string::npos){
    cout << data_path << endl;
    cout << mc_path << endl;
    cout << endl;
  }
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
    data_name.ReplaceAll("cnt_eff","fit_eff");

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

TH2D TranslateHisto(const TH2 &input){
  int nx = input.GetNbinsX();
  int ny = input.GetNbinsY();
  TH2D output(input.GetName(), input.GetTitle(), nx, 0.5, nx+0.5, ny, 0.5, ny+0.5);
  output.Sumw2();
  output.SetStats(false);
  output.SetMarkerSize(2);
  output.SetLabelSize(0.05,"XYZ");
  output.SetTitleSize(0.05,"XYZ");
  for(int ix = 0; ix <= nx+1; ++ix){
    for(int iy = 0; iy <= ny+1; ++iy){
      output.SetBinContent(ix, iy, input.GetBinContent(ix, iy));
      output.SetBinError(ix, iy, input.GetBinError(ix, iy));
    }
  }
  
  for(int ix = 1; ix <= nx; ++ix){
    const TAxis *iaxis = input.GetXaxis();
    TAxis *oaxis = output.GetXaxis();
    if(iaxis == NULL || oaxis == NULL) continue;
    oaxis->SetTitle(iaxis->GetTitle());
    ostringstream oss;
    oss << iaxis->GetBinLowEdge(ix) << "-" << iaxis->GetBinUpEdge(ix) << flush;
    oaxis->SetBinLabel(ix, oss.str().c_str());
  }

  for(int iy = 1; iy <= ny; ++iy){
    const TAxis *iaxis = input.GetYaxis();
    TAxis *oaxis = output.GetYaxis();
    if(iaxis == NULL || oaxis == NULL) continue;
    oaxis->SetTitle(iaxis->GetTitle());
    ostringstream oss;
    oss << iaxis->GetBinLowEdge(iy) << "-" << iaxis->GetBinUpEdge(iy) << flush;
    oaxis->SetBinLabel(iy, oss.str().c_str());
  }

  return output;
}

void Print2D(TH2 const * const h_data_in, TH2 const * const h_mc_in, const TString &ext){
  if(h_data_in == NULL || h_mc_in == NULL) return;

  TH2D h_data = TranslateHisto(*h_data_in);
  TH2D h_mc = TranslateHisto(*h_mc_in);

  TCanvas canvas;
  gStyle->SetPalette(bands, rainbow);
  h_data.SetMarkerSize(2);
  h_mc.SetMarkerSize(2);

  h_data.SetMinimum(0.0);
  h_data.SetMaximum(1.0);
  h_data.Draw("colz");
  h_data.Draw("textesame");
  canvas.Print("plots/2d_data_"+ext+".pdf");
  canvas.Print("plots/2d_data_"+ext+".png");
  PrintTable(&h_data, "data_"+ext);
  h_mc.SetMinimum(0.0);
  h_mc.SetMaximum(1.0);
  h_mc.Draw("colz");
  h_mc.Draw("textesame");
  canvas.Print("plots/2d_mc_"+ext+".pdf");
  canvas.Print("plots/2d_mc_"+ext+".png");
  PrintTable(&h_mc, "mc_"+ext);

  gStyle->SetPalette(bands, patriotic);
  h_data.Divide(&h_mc);
  h_data.SetMinimum(0.5);
  h_data.SetMaximum(2.0);
  canvas.SetLogz();
  h_data.Draw("colz");
  h_data.Draw("textesame");
  canvas.Print("plots/sf_"+ext+".pdf");
  canvas.Print("plots/sf_"+ext+".png");
  PrintTable(&h_data, "sf_"+ext);
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
  canvas.Print("plots/1d_"+ext+".png");

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
