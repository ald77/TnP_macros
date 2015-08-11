#include "print_scale_factors.hpp"

#include <iostream>

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
  
  PrintScaleFactors("id");
  PrintScaleFactors("loose");
  PrintScaleFactors("veto");
  PrintScaleFactors("medium");
  PrintScaleFactors("tight");
}

void PrintScaleFactors(const TString &file_ext){
  //cout << "Scale factors: " << file_ext << endl;
  TFile data_file("data/eff_data_"+file_ext+".root","read");
  TFile mc_file("data/eff_mc_"+file_ext+".root","read");
  if(!(data_file.IsOpen() && mc_file.IsOpen())) return;
  PrintDirectory(data_file, mc_file, file_ext);
}

void PrintDirectory(TDirectory &data_dir, TDirectory &mc_dir, const TString &ext){
  //cout << "Directory: " << data_dir.GetName() << ", " << mc_dir.GetName() << endl;
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
    PrintObjects(data_obj, mc_dir.Get(name), ext);
    PrintObjects(data_obj, mc_dir.Get(data_name), ext);
  }
}

void PrintObjects(TObject *data_obj,TObject *mc_obj, const TString &ext){
  if(data_obj == NULL || mc_obj == NULL) return;
  //cout << "Objects: " << data_obj->GetName() << ", " << mc_obj->GetName() << endl;
  TString class_name = data_obj->ClassName();
  TString mc_name = mc_obj->GetName();
  if(class_name == "TCanvas"){
    PrintCanvas(*static_cast<TCanvas*>(data_obj),
                *static_cast<TCanvas*>(mc_obj),
                ext+"_"+mc_name);
  }else if(class_name.Contains("TDirectory")){
    PrintDirectory(*static_cast<TDirectory*>(data_obj),
                   *static_cast<TDirectory*>(mc_obj),
                   ext+"_"+mc_name);
  }
}

void PrintCanvas(TCanvas &data_can, TCanvas &mc_can, const TString &ext){
  //cout << "Canvas: " << data_can.GetName() << ", " << mc_can.GetName() << endl;
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
  //cout << "2D: " << h_data_in->GetName() << ", " << h_mc_in->GetName() << endl;
  TH2 *h_data = static_cast<TH2*>(h_data_in->Clone());
  if(h_data == NULL) return;
  TH2 *h_mc = static_cast<TH2*>(h_mc_in->Clone());
  if(h_mc == NULL) return;

  TCanvas canvas;
  gStyle->SetPalette(bands, rainbow);
  h_data->Draw("colz");
  h_data->Draw("textesame");
  canvas.Print("plots/2d_data_"+ext+".pdf");
  h_mc->Draw("colz");
  h_mc->Draw("textesame");
  canvas.Print("plots/2d_mc_"+ext+".pdf");

  gStyle->SetPalette(bands, patriotic);
  h_data->Divide(h_mc);
  h_data->SetMinimum(0.8);
  h_data->SetMaximum(1.25);
  canvas.SetLogz();
  h_data->Draw("colz");
  h_data->Draw("textesame");
  canvas.Print("plots/sf_"+ext+".pdf");

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
  //cout << "1D: " << h_data_in->GetName() << ", " << h_mc_in->GetName() << endl;
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
