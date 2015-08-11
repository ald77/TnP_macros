#include <vector>

#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TString.h"
#include "TFile.h"

using namespace std;

void PrintDirectory(TDirectory &dir, const TString &set, const TString &measured,
		    const TString &wp, const TString &ext = ""){
  TString lower_wp = wp;
  lower_wp.ToLower();
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
      canvas->Print("plots/"+set+"_"+measured+"_"+lower_wp+"_"+ext+name+".pdf");
    }else if(class_name.Contains("TDirectory")){
      TDirectory *sub_dir = static_cast<TDirectory*>(obj);
      if(sub_dir == NULL) continue;
      PrintDirectory(*sub_dir, set, measured, wp, ext+name+"_");
    }
  }
}

void PrintPlots(const TString& set, const TString& measured, const TString &wp){
  TString lower_wp = wp;
  lower_wp.ToLower();
  TFile file(measured == "id" ? "data/eff_"+set+"_id.root"
	     : "data/eff_"+set+"_"+lower_wp+".root","read");
  TString path = "GsfElectronToID/MCtruth_"+wp;
  if(measured != "id"){
    path.ReplaceAll("GsfElectronToID",wp+"ElectronToMiniIso");
  }
  if(set != "mc"){
    path.ReplaceAll("MCtruth_","");
  }
  TDirectory *dir = static_cast<TDirectory*>(file.Get(path));
  if(dir == NULL) return;
  PrintDirectory(*dir, set, measured, wp);
}

int main(){
  vector<TString> set, measured, wp;
  set.push_back("mc");
  set.push_back("data");
  measured.push_back("id");
  measured.push_back("iso");
  wp.push_back("Veto");
  wp.push_back("Loose");
  wp.push_back("Medium");
  wp.push_back("Tight");

  for(size_t iset = 0; iset < set.size(); ++iset){
    for(size_t imeasured = 0; imeasured < measured.size(); ++imeasured){
      for(size_t iwp = 0; iwp < wp.size(); ++iwp){
	PrintPlots(set.at(iset), measured.at(imeasured), wp.at(iwp));
      }
    }
  }
}
