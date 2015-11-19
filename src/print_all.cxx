#include <vector>

#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TString.h"
#include "TFile.h"

using namespace std;

void PrintDirectory(TDirectory &dir, const TString &ext = ""){
  TList *keys = dir.GetListOfKeys();
  if(keys == NULL) return;
  for(int i = 0; i < keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == NULL) continue;
    TString name = key->GetName();
    if(name == "w") continue;
    TObject *obj = dir.Get(name);
    if(obj == NULL) continue;
    TString class_name = obj->ClassName();
    if(class_name == "TCanvas"){
      TCanvas *canvas = static_cast<TCanvas*>(obj);
      if(canvas == NULL) continue;
      TString out_name = ext+"_"+name;
      out_name.ReplaceAll("/","_");
      canvas->Print("plots/"+out_name+".pdf");
    }else if(class_name.Contains("TDirectory")){
      TDirectory *sub_dir = static_cast<TDirectory*>(obj);
      if(sub_dir == NULL) continue;
      PrintDirectory(*sub_dir, ext+"_"+name);
    }
  }
}

void PrintPlots(TString file_name){
  TFile file(file_name, "read");
  file_name.ReplaceAll(".root","");
  file_name.ReplaceAll("data/eff_","");
  TList *keys = file.GetListOfKeys();
  if(keys == NULL) return;
  for(int i = 0; i < keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == NULL) continue;
    TString name = key->GetName();
    if(name == "w") continue;
    TObject *obj = file.Get(name);
    if(obj == NULL) continue;
    TString class_name = obj->ClassName();
    if(class_name == "TCanvas"){
      TCanvas *canvas = static_cast<TCanvas*>(obj);
      if(canvas == NULL) continue;
      canvas->Print("plots/"+file_name+name+".pdf");
    }else if(class_name.Contains("TDirectory")){
      TDirectory *sub_dir = static_cast<TDirectory*>(obj);
      if(sub_dir == NULL) continue;
      PrintDirectory(*sub_dir, file_name);
    }
  }
}

int main(){
  PrintPlots("data/eff_data_foid2d.root");
  PrintPlots("data/eff_data_loose.root");
  PrintPlots("data/eff_data_loose2d.root");
  PrintPlots("data/eff_data_medium.root");
  PrintPlots("data/eff_data_mvavlooseconvihit1_act.root");
  PrintPlots("data/eff_data_mvavlooseconvihit1_eta.root");
  PrintPlots("data/eff_data_mvavloosemini4_act.root");
  PrintPlots("data/eff_data_mvavloosemini4_eta.root");
  PrintPlots("data/eff_data_mvavloosemini_act.root");
  PrintPlots("data/eff_data_mvavloosemini_eta.root");
  PrintPlots("data/eff_data_mvatightconvihit0chg_act.root");
  PrintPlots("data/eff_data_mvatightconvihit0chg_eta.root");
  PrintPlots("data/eff_data_mvatightmulti_act.root");
  PrintPlots("data/eff_data_mvatightmulti_eta.root");
  PrintPlots("data/eff_data_mvatightmultiemu_act.root");
  PrintPlots("data/eff_data_mvatightmultiemu_eta.root");
  PrintPlots("data/eff_data_tight.root");
  PrintPlots("data/eff_data_tight2d3d.root");
  PrintPlots("data/eff_data_tightid2d3d.root");
  PrintPlots("data/eff_data_veto.root");
  PrintPlots("data/eff_mc_foid2d.root");
  PrintPlots("data/eff_mc_loose.root");
  PrintPlots("data/eff_mc_loose2d.root");
  PrintPlots("data/eff_mc_medium.root");
  PrintPlots("data/eff_mc_mvavlooseconvihit1_act.root");
  PrintPlots("data/eff_mc_mvavlooseconvihit1_eta.root");
  PrintPlots("data/eff_mc_mvavloosemini4_act.root");
  PrintPlots("data/eff_mc_mvavloosemini4_eta.root");
  PrintPlots("data/eff_mc_mvavloosemini_act.root");
  PrintPlots("data/eff_mc_mvavloosemini_eta.root");
  PrintPlots("data/eff_mc_mvatightconvihit0chg_act.root");
  PrintPlots("data/eff_mc_mvatightconvihit0chg_eta.root");
  PrintPlots("data/eff_mc_mvatightmulti_act.root");
  PrintPlots("data/eff_mc_mvatightmulti_eta.root");
  PrintPlots("data/eff_mc_mvatightmultiemu_act.root");
  PrintPlots("data/eff_mc_mvatightmultiemu_eta.root");
  PrintPlots("data/eff_mc_tight.root");
  PrintPlots("data/eff_mc_tight2d3d.root");
  PrintPlots("data/eff_mc_tightid2d3d.root");
  PrintPlots("data/eff_mc_veto.root");
}
