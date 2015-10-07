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
      TString out_name = ext+"_"+name+".pdf";
      out_name.ReplaceAll("/","_");
      canvas->Print("plots/"+out_name);
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
  PrintPlots("data/eff_data_veto_id.root");
  PrintPlots("data/eff_data_veto_iso.root");
  PrintPlots("data/eff_data_loose_id.root");
  PrintPlots("data/eff_data_loose_iso.root");
  PrintPlots("data/eff_data_medium_id.root");
  PrintPlots("data/eff_data_medium_iso.root");
  PrintPlots("data/eff_data_tight_id.root");
  PrintPlots("data/eff_data_tight_iso.root");
  PrintPlots("data/eff_mc_veto_id.root");
  PrintPlots("data/eff_mc_veto_iso.root");
  PrintPlots("data/eff_mc_loose_id.root");
  PrintPlots("data/eff_mc_loose_iso.root");
  PrintPlots("data/eff_mc_medium_id.root");
  PrintPlots("data/eff_mc_medium_iso.root");
  PrintPlots("data/eff_mc_tight_id.root");
  PrintPlots("data/eff_mc_tight_iso.root");
}
