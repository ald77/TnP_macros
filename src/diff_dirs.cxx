#include "diff_dirs.hpp"

#include <cmath>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <memory>

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
  string current_name = "";
}

int main(){
  GetRainbowPalette();
  GetPatrioticPalette();
  gStyle->SetPaintTextFormat("2.3f");
  gStyle->SetNumberContours(bands);
  gStyle->SetPalette(bands, patriotic);

  vector<TString> dirs = {"~/cmssw/CMSSW_7_4_15/src/PhysicsTools/TagAndProbe/test/2015_11_19_normal",
			  "~/cmssw/CMSSW_7_4_15/src/PhysicsTools/TagAndProbe/test/2015_11_20_expbkg_v2",
			  "~/cmssw/CMSSW_7_4_15/src/PhysicsTools/TagAndProbe/test/2015_11_20_cbres",
			  "~/cmssw/CMSSW_7_4_15/src/PhysicsTools/TagAndProbe/test/2015_11_20_redrange_v2"};

  PrintScaleFactors(dirs, "foid2d");
  PrintScaleFactors(dirs, "loose");
  PrintScaleFactors(dirs, "loose2d");
  PrintScaleFactors(dirs, "medium");
  PrintScaleFactors(dirs, "mvatightconvihit0chg_act");
  PrintScaleFactors(dirs, "mvatightconvihit0chg_eta");
  PrintScaleFactors(dirs, "mvatightmulti_act");
  PrintScaleFactors(dirs, "mvatightmulti_eta");
  PrintScaleFactors(dirs, "mvatightmultiemu_act");
  PrintScaleFactors(dirs, "mvatightmultiemu_eta");
  PrintScaleFactors(dirs, "mvavlooseconvihit1_act");
  PrintScaleFactors(dirs, "mvavlooseconvihit1_eta");
  PrintScaleFactors(dirs, "mvavloosemini4_act");
  PrintScaleFactors(dirs, "mvavloosemini4_eta");
  PrintScaleFactors(dirs, "mvavloosemini_act");
  PrintScaleFactors(dirs, "mvavloosemini_eta");
  PrintScaleFactors(dirs, "tight");
  PrintScaleFactors(dirs, "tight2d3d");
  PrintScaleFactors(dirs, "tightid2d3d");
  PrintScaleFactors(dirs, "veto");
}

void PrintScaleFactors(const vector<TString> &dirs, const TString &file_ext){
  vector<TFile*> data_files, mc_files;
  for(const auto &dir: dirs){
    data_files.push_back(new TFile(dir+"/eff_data_"+file_ext+".root","read"));
    mc_files.push_back(new TFile(dir+"/eff_mc_"+file_ext+".root","read"));
    if(data_files.back() == nullptr || !data_files.back()->IsOpen()){
      throw runtime_error(string("Could not open data file"));
    }
    if(mc_files.back() == nullptr || !mc_files.back()->IsOpen()){
      throw runtime_error(string("Could not open mc file"));
    }
  }
  current_name = file_ext;
  PrintDirectory(Convert<TDirectory>(data_files), Convert<TDirectory>(mc_files),
		 file_ext, true);
  current_name = "";
  for(auto &file: data_files){
    if(file != nullptr){
      delete file;
      file = nullptr;
    }
  }
  for(auto &file: mc_files){
    if(file != nullptr){
      delete file;
      file = nullptr;
    }
  }
}

void PrintDirectory(const vector<TDirectory*> &data_dirs,
		    const vector<TDirectory*> &mc_dirs,
		    const TString &ext, bool get_true){
  if(data_dirs.size() != mc_dirs.size()) throw runtime_error("Different number of data and MC directories");
  if(data_dirs.size() == 0) throw runtime_error("No directories found!");
  string data_path = data_dirs.front()->GetPath();
  string mc_path = mc_dirs.front()->GetPath();
  if(data_path.find("cnt_enf") != string::npos
     || mc_path.find("cnt_eff") != string::npos){
    cout << data_path << endl;
    cout << mc_path << endl;
    cout << endl;
  }
  TList *keys = mc_dirs.front()->GetListOfKeys();
  if(keys == nullptr) return;
  for(int i = 0; i <keys->GetSize(); ++i){
    TObject *key = keys->At(i);
    if(key == nullptr) continue;
    TString name = key->GetName();
    TString data_name = name;
    data_name.ReplaceAll("MCtruth_","");
    data_name.ReplaceAll("_and_mcTrue_true","");
    data_name.ReplaceAll("__mcTrue_true","");
    data_name.ReplaceAll("_mcTrue_true","");
    data_name.ReplaceAll("cnt_eff","fit_eff");

    vector<TObject*> data_objs(data_dirs.size());
    vector<TObject*> mc_objs(mc_dirs.size());
    vector<TObject*> mc_data_objs(mc_dirs.size());

    if(data_dirs.front()->Get(data_name) == nullptr) continue;
    if(mc_dirs.front()->Get(name) == nullptr) continue;
    bool do_mc_data = (data_name != name) && (mc_dirs.front()->Get(data_name) != nullptr);

    for(size_t idir = 0; idir < data_dirs.size(); ++idir){
      data_objs.at(idir) = data_dirs.at(idir)->Get(data_name);
      mc_objs.at(idir) = mc_dirs.at(idir)->Get(name);
      mc_data_objs.at(idir) = mc_dirs.at(idir)->Get(data_name);
      if(data_objs.at(idir) == nullptr) throw runtime_error(string("Could not get data object "+data_name));
      if(mc_objs.at(idir) == nullptr) throw runtime_error(string("Could not get mc object "+name));
      if(do_mc_data && mc_objs.at(idir) == nullptr) throw runtime_error(string("Could not get mc object "+name));
    }
    PrintObjects(data_objs, mc_objs, ext, get_true);
    if(do_mc_data){
      PrintObjects(data_objs, mc_data_objs, ext, false);
    }
  }
}

void PrintObjects(const vector<TObject*> &data_objs,
		  const vector<TObject*> &mc_objs,
		  const TString &ext, bool get_true){
  if(data_objs.size() != mc_objs.size()) throw runtime_error("Different number of data and MC objects");
  if(data_objs.size() == 0) throw runtime_error("No objects found!");
  for(const auto data_obj: data_objs){
    if(data_obj == nullptr) throw runtime_error("Null data object");
  }
  for(const auto mc_obj: mc_objs){
    if(mc_obj == nullptr) throw runtime_error("Null mc object");
  }
  TString class_name = data_objs.front()->ClassName();
  TString mc_name = mc_objs.front()->GetName();
  if(class_name == "TCanvas"){
    PrintCanvas(Convert<TCanvas>(data_objs),
		Convert<TCanvas>(mc_objs), ext+"_"+mc_name);
  }else if(class_name.Contains("TDirectory")){
    PrintDirectory(Convert<TDirectory>(data_objs),
		   Convert<TDirectory>(mc_objs), ext+"_"+mc_name, get_true);
  }
}

void PrintCanvas(const vector<TCanvas*> &data_cans,
		 const vector<TCanvas*> &mc_cans,
		 const TString &ext){
  if(data_cans.size() != mc_cans.size()) throw runtime_error("Different number of data and MC canvases");
  if(data_cans.size() == 0) throw runtime_error("No data canvases");

  TList *list = mc_cans.front()->GetListOfPrimitives();
  if(list == nullptr) return;
  for(int i = 0; i < list->GetSize(); ++i){
    TObject *prim = list->At(i);
    if(prim == nullptr) continue;
    TString name = prim->GetName();
    TString data_name = name;
    data_name.ReplaceAll("MCtruth_","");
    data_name.ReplaceAll("_and_mcTrue_true","");
    data_name.ReplaceAll("__mcTrue_true","");
    data_name.ReplaceAll("_mcTrue_true","");

    bool need_continue = false;
    vector<TObject*> data_objs(data_cans.size());
    vector<TObject*> mc_objs(mc_cans.size());
    for(size_t ican = 0; ican < data_cans.size(); ++ican){
      data_objs.at(ican) = data_cans.at(ican)->GetPrimitive(data_name);
      mc_objs.at(ican) = mc_cans.at(ican)->GetPrimitive(name);
      if(data_objs.at(ican) == nullptr || mc_objs.at(ican) == nullptr) need_continue = true;
    }
    if(need_continue) continue;

    TString class_name = prim->ClassName();
    for(size_t iobj = 0; iobj < data_objs.size(); ++iobj){
      if(data_objs.at(iobj)->ClassName() != class_name) need_continue = true;
      if(mc_objs.at(iobj)->ClassName() != class_name) need_continue = true;
    }
    if(need_continue) continue;

    if(class_name.Contains("TH2")){
      Print2D(Convert<TH2>(data_objs),
	      Convert<TH2>(mc_objs),
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
    if(iaxis == nullptr || oaxis == nullptr) continue;
    oaxis->SetTitle(iaxis->GetTitle());
    ostringstream oss;
    oss << iaxis->GetBinLowEdge(ix) << "-" << iaxis->GetBinUpEdge(ix) << flush;
    oaxis->SetBinLabel(ix, oss.str().c_str());
  }

  for(int iy = 1; iy <= ny; ++iy){
    const TAxis *iaxis = input.GetYaxis();
    TAxis *oaxis = output.GetYaxis();
    if(iaxis == nullptr || oaxis == nullptr) continue;
    oaxis->SetTitle(iaxis->GetTitle());
    ostringstream oss;
    oss << iaxis->GetBinLowEdge(iy) << "-" << iaxis->GetBinUpEdge(iy) << flush;
    oaxis->SetBinLabel(iy, oss.str().c_str());
  }

  return output;
}

void Print2D(const vector<TH2*> &data_hists,
	     const vector<TH2*> &mc_hists,
	     const TString &ext){
  if(data_hists.size() != mc_hists.size()) throw runtime_error("Different number of data and MC hists");
  if(data_hists.size() == 0) throw runtime_error("No histograms");
  if(data_hists.front() == nullptr) throw runtime_error("Leading data histogram is null");
  TH2D hpretty = TranslateHisto(*data_hists.front());
  TH2D hresult = *static_cast<TH2D*>(data_hists.front());
  for(int ix = 0; ix <= hresult.GetNbinsX()+1; ++ix){
    for(int iy = 0; iy <= hresult.GetNbinsY()+1; ++iy){
      hpretty.SetBinContent(ix, iy, 0.);
      hpretty.SetBinError(ix, iy, 1.);
      hresult.SetBinContent(ix, iy, 0.);
      hresult.SetBinError(ix, iy, 1.);
    }
  }

  vector<vector<vector<double> > > stats(hresult.GetNbinsX(), vector<vector<double> >(hresult.GetNbinsY(), vector<double>(0)));
  vector<vector<vector<double> > > vals(hresult.GetNbinsX(), vector<vector<double> >(hresult.GetNbinsY(), vector<double>(0)));
  for(size_t i = 0; i < data_hists.size(); ++i){
    if(data_hists.at(i) == nullptr || mc_hists.at(i) == nullptr) continue;
    TH2D hdata = TranslateHisto(*data_hists.at(i));
    TH2D hmc = TranslateHisto(*mc_hists.at(i));
    hdata.Divide(&hmc);
    for(int ix = 0; ix <= hresult.GetNbinsX()+1; ++ix){
      for(int iy = 0; iy <= hresult.GetNbinsY()+1; ++iy){
	if(ix >= 1 && ix <= hresult.GetNbinsX()
	   && iy >= 1 && iy <= hresult.GetNbinsY()){
	  double this_content = hdata.GetBinContent(ix, iy);
	  double this_error = hdata.GetBinError(ix, iy);
	  if(!std::isnan(this_content) && !std::isnan(this_error)){
	    vals.at(ix-1).at(iy-1).push_back(this_content);
	    stats.at(ix-1).at(iy-1).push_back(this_error);
	  }
	}
      }
    }
  }

  bool include_stats = true;
  double stat_mult = include_stats ? 1. : 0.;
  for(int ix = 0; ix <= hresult.GetNbinsX()+1; ++ix){
    for(int iy = 0; iy <= hresult.GetNbinsY()+1; ++iy){
      int iix = (ix < 1) ? 1 : ((ix > hresult.GetNbinsX()) ? hresult.GetNbinsX() : ix);
      int iiy = (iy < 1) ? 1 : ((iy > hresult.GetNbinsY()) ? hresult.GetNbinsY() : iy);
      vector<double> good_vals = RemoveOutliers(vals.at(iix-1).at(iiy-1));
      double maxval = *max_element(good_vals.cbegin(), good_vals.cend());
      double minval = *min_element(good_vals.cbegin(), good_vals.cend());
      double err = maxval-minval;
      double content = GoodVal(vals.at(iix-1).at(iiy-1));
      double min_error = 0.005;
      double full_error = hypot(hypot(err, min_error), stat_mult*GoodVal(stats.at(iix-1).at(iiy-1)));
      hpretty.SetBinContent(ix, iy, content);
      hpretty.SetBinError(ix, iy, full_error);
      hresult.SetBinContent(ix, iy, content);
      hresult.SetBinError(ix, iy, full_error);
    }
  }

  FixOverflow(hpretty);
  FixOverflow(hresult);

  if(ext.Contains("_cnt_eff_plots_probe_sc_et_") && IncludePlot()){
    TFile out_file("merged_result.root","update");
    out_file.cd();
    bool do_pretty = false;
    hresult.SetName(FixName().c_str());
    hresult.SetTitle(FixName().c_str());
    hpretty.SetName(FixName().c_str());
    hpretty.SetTitle(FixName().c_str());
    if(do_pretty){
      hpretty.Write();
    }else{
      hresult.Write();
    }
    out_file.Close();
  }

  TCanvas canvas;
  gStyle->SetPalette(bands, patriotic);
  canvas.SetLogz();
  hpretty.SetMarkerSize(2);
  hpretty.SetMinimum(0.5);
  hpretty.SetMaximum(2.0);
  hpretty.Draw("colz");
  hpretty.Draw("textesame");
  canvas.Print("diffs/sf_"+ext+".pdf");
  canvas.Print("diffs/sf_"+ext+".png");
  PrintTable(&hpretty, "diffs_sf_"+ext);
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

  if(histo == nullptr || histo->GetNbinsX() < bin || histo->GetNbinsY()<5) return;
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

double Median(vector<double> v){
  if(v.size() == 0) return 0.;
  sort(v.begin(), v.end());
  bool is_odd = (v.size() % 2);
  size_t mid = floor(0.5*(v.size()-1));
  return is_odd ? v.at(mid) : 0.5*(v.at(mid)+v.at(mid+1));
}

vector<double> RemoveOutliers(const vector<double> &v){
  if(v.size() <= 2) return v;
  double mean = accumulate(v.cbegin(), v.cend(), 0.)/v.size();
  double sigma = 0.;
  for(const auto &x: v){
    double delta = x-mean;
    sigma += delta*delta;
  }
  sigma = sqrt(sigma/v.size());
  if(sigma <= 0.) return v;
  vector<double> out;
  for(const auto &x: v){
    if(fabs(x-mean)<=sqrt(2.)*sigma) out.push_back(x);
  }
  return out;
}

double GoodVal(const vector<double> &v){
  return Median(RemoveOutliers(v));
}

string FixName(){
  string pretty_name = "BADBADBADBADBAD";
  if(current_name == "foid2d"){
    pretty_name = "MVAVLooseFO_and_IDEmu_and_TightIP2D";
  }else if(current_name == "loose"){
    pretty_name = "CutBasedLoose";
  }else if(current_name == "loose2d"){
    pretty_name = "MVAVLoose_and_TightIP2D";
  }else if(current_name == "medium"){
    pretty_name = "CutBasedMedium";
  }else if(current_name == "mvatightconvihit0chg_act"){
    pretty_name = "ConvVeto_and_MissInnerHits0_and_ChgConsistent_vs_RelActivity";
  }else if(current_name == "mvatightconvihit0chg_eta"){
    pretty_name = "ConvVeto_and_MissInnerHits0_and_ChgConsistent_vs_AbsEta";
  }else if(current_name == "mvatightmulti_act"){
    pretty_name = "MultiIsoTight_vs_RelActivity";
  }else if(current_name == "mvatightmulti_eta"){
    pretty_name = "MultiIsoTight_vs_AbsEta";
  }else if(current_name == "mvatightmultiemu_act"){
    pretty_name = "MultiIsoTight_and_IsoEmu_vs_RelActivity";
  }else if(current_name == "mvatightmultiemu_eta"){
    pretty_name = "MultiIsoTight_and_IsoEmu_vs_AbsEta";
  }else if(current_name == "mvavlooseconvihit1_act"){
    pretty_name = "ConvVeto_and_MissInnerHits1_vs_RelActivity";
  }else if(current_name == "mvavlooseconvihit1_eta"){
    pretty_name = "ConvVeto_and_MissInnerHits1_vs_AbsEta";
  }else if(current_name == "mvavloosemini4_act"){
    pretty_name = "MiniIso0p4_vs_RelActivity";
  }else if(current_name == "mvavloosemini4_eta"){
    pretty_name = "MiniIso0p4_vs_AbsEta";
  }else if(current_name == "mvavloosemini_act"){
    pretty_name = "MiniIso0p1_vs_RelActivity";
  }else if(current_name == "mvavloosemini_eta"){
    pretty_name = "MiniIso0p1_vs_AbsEta";
  }else if(current_name == "tight"){
    pretty_name = "CutBasedTight";
  }else if(current_name == "tight2d3d"){
    pretty_name = "MVATight_and_TightIP2D_and_TightIP3D";
  }else if(current_name == "tightid2d3d"){
    pretty_name = "MVATight_and_IDEmu_and_TightIP2D_and_TightIP3D";
  }else if(current_name == "veto"){
    pretty_name = "CutBasedVeto";
  }
  return pretty_name;
}

bool IncludePlot(){
  if(current_name == "foid2d"){
    return true;
  }else if(current_name == "loose"){
    return true;
  }else if(current_name == "loose2d"){
    return true;
  }else if(current_name == "medium"){
    return true;
  }else if(current_name == "mvatightconvihit0chg_act"){
    return false;
  }else if(current_name == "mvatightconvihit0chg_eta"){
    return true;
  }else if(current_name == "mvatightmulti_act"){
    return false;
  }else if(current_name == "mvatightmulti_eta"){
    return true;
  }else if(current_name == "mvatightmultiemu_act"){
    return false;
  }else if(current_name == "mvatightmultiemu_eta"){
    return true;
  }else if(current_name == "mvavlooseconvihit1_act"){
    return false;
  }else if(current_name == "mvavlooseconvihit1_eta"){
    return true;
  }else if(current_name == "mvavloosemini4_act"){
    return true;
  }else if(current_name == "mvavloosemini4_eta"){
    return true;
  }else if(current_name == "mvavloosemini_act"){
    return true;
  }else if(current_name == "mvavloosemini_eta"){
    return true;
  }else if(current_name == "tight"){
    return true;
  }else if(current_name == "tight2d3d"){
    return true;
  }else if(current_name == "tightid2d3d"){
    return true;
  }else if(current_name == "veto"){
    return true;
  }else{
    return false;
  }
}

void FixOverflow(TH2D &h){
  int nx = h.GetNbinsX();
  int ny = h.GetNbinsY();
  for(int iy = 0; iy < ny; ++iy){
    h.SetBinContent(0, iy, h.GetBinContent(1, iy));
    h.SetBinError(0, iy, h.GetBinError(1, iy));
    h.SetBinContent(nx+1, iy, h.GetBinContent(nx, iy));
    h.SetBinError(nx+1, iy, h.GetBinError(nx, iy));
  }
  for(int ix = 0; ix < nx; ++ix){
    h.SetBinContent(ix, 0, h.GetBinContent(ix, 1));
    h.SetBinError(ix, 0, h.GetBinError(ix, 1));
    h.SetBinContent(ix, ny+1, h.GetBinContent(ix, ny));
    h.SetBinError(ix, ny+1, h.GetBinError(ix, ny));
  }
}
