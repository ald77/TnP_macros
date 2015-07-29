#include <iostream>

#include "TCanvas.h"
#include "TString.h"
#include "TFile.h"
#include "TH2F.h"
#include "TH1D.h"

using namespace std;

void ProcessID(TFile& file, const TString& wp, const TString &ext){
  TCanvas *in_can = static_cast<TCanvas*>(file.Get("GsfElectronToID/"+wp+"/fit_eff_plots/probe_sc_abseta_probe_sc_et_PLOT"));
  TH2F *hist2 = static_cast<TH2F*>(in_can->GetPrimitive("probe_sc_abseta_probe_sc_et_PLOT"));
  TCanvas out_can;
  out_can.cd();
  TH1D *hist1 = hist2->ProjectionX();
  hist1->Draw("e1p");
  out_can.Print(ext+"_id_"+wp+".pdf");
}

void ProcessID(const TString& file_name){
  TFile file(file_name, "READ");
  TString ext;
  if(file_name.Contains("_mc_")){
    ext = "mc";
  }else{
    ext = "data";
  }

  ProcessID(file, "Veto", ext);
  ProcessID(file, "Loose", ext);
  ProcessID(file, "Medium", ext);
  ProcessID(file, "Tight", ext);
}

void ProcessIso(const TString& file_name){
  TFile file(file_name, "READ");
  TString ext;
  if(file_name.Contains("_mc_")){
    ext = "mc";
  }else{
    ext = "data";
  }

  TString wp;
  if(file_name.Contains("veto")){
    wp = "Veto";
  }else if(file_name.Contains("loose")){
    wp = "Loose";
  }else if(file_name.Contains("medium")){
    wp = "Medium";
  }else{
    wp = "Tight";
  }

  TCanvas *in_can = static_cast<TCanvas*>(file.Get(wp+"ElectronToMiniIso/"+wp+"/fit_eff_plots/probe_sc_abseta_probe_sc_et_PLOT"));
  TH2F *hist2 = static_cast<TH2F*>(in_can->GetPrimitive("probe_sc_abseta_probe_sc_et_PLOT"));
  TCanvas out_can;
  out_can.cd();
  TH1D *hist1 = hist2->ProjectionX();
  hist1->Draw("e1p");
  out_can.Print(ext+"_iso_"+wp+".pdf");
}

int main(){
  ProcessID("~/eff_mc_id.root");
  ProcessID("~/eff_data_id.root");
  ProcessIso("~/eff_mc_veto.root");
  ProcessIso("~/eff_mc_loose.root");
  ProcessIso("~/eff_mc_medium.root");
  ProcessIso("~/eff_mc_tight.root");
  ProcessIso("~/eff_data_veto.root");
  ProcessIso("~/eff_data_loose.root");
  ProcessIso("~/eff_data_medium.root");
  ProcessIso("~/eff_data_tight.root");
}
