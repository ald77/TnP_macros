#include <vector>

#include "TCanvas.h"
#include "TDirectory.h"
#include "TList.h"
#include "TKey.h"
#include "TString.h"
#include "TFile.h"

using namespace std;

void PrintPlots(const TString& set, const TString& measured, const TString &wp){
  TString lower_wp = wp;
  lower_wp.ToLower();
  TFile file(measured == "id" ? "data/eff_"+set+"_id.root"
	     : "data/eff_"+set+"_"+lower_wp+".root","read");
  TString path = "GsfElectronToID/"+wp+"/fit_eff_plots";
  if(measured != "id"){
    path.ReplaceAll("GsfElectronToID",wp+"ToMiniIso");
  }
  TDirectory *dir = static_cast<TDirectory*>(file.Get(path));
  if(dir == NULL) return;
  TList *keys = dir->GetListOfKeys();
  if(keys == NULL) return;
  for(int i = 0; i < keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == NULL) continue;
    TString name = key->GetName();
    TObject *obj = dir->Get(name);
    if(obj == NULL) continue;
    TString class_name = obj->ClassName();
    if(class_name != "TCanvas") continue;
    TCanvas *canvas = static_cast<TCanvas*>(obj);
    if(canvas == NULL) continue;
    canvas->Print("plots/"+set+"_"+measured+"_"+lower_wp+"_"+name+".pdf");
  }
}

int main(){
  vector<TString> set(2), measured(2), wp(4);
  set.at(0) = "mc";
  set.at(1) = "data";
  measured.at(0) = "id";
  measured.at(1) = "iso";
  wp.at(0) = "Veto";
  wp.at(1) = "Loose";
  wp.at(2) = "Medium";
  wp.at(3) = "Tight";

  for(size_t iset = 0; iset < set.size(); ++iset){
    for(size_t imeasured = 0; imeasured < measured.size(); ++imeasured){
      for(size_t iwp = 0; iwp < wp.size(); ++iwp){
	PrintPlots(set.at(iset), measured.at(imeasured), wp.at(iwp));
      }
    }
  }
}
