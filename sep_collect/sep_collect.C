Int_t collector(TFile* f, Int_t n_septant, TString gen, TString metric, TString p_type, TString p_nrg, Int_t i, Int_t r, std::vector<Double_t> scale){

    std::vector<Double_t> scale_err(scale.size(), 0.0);
    std::vector<Double_t> rate(scale.size(), 0.0);
    std::vector<Double_t> rate_qerr(scale.size(), 0.0);
    std::vector<Double_t> rate_err(scale.size(), 0.0);
    for(Int_t s=0; s<n_septant; s++){ 
        TString g_name = Form("%s_%s_%s_%s_%d_%d_%d", gen.Data(), metric.Data(), p_type.Data(), p_nrg.Data(), s+1, i, r);
        TGraphErrors *g1 = (TGraphErrors*) f->Get(g_name);
        Double_t * tmp_rate = g1->GetY();
        Double_t * tmp_rate_err = g1->GetEY();
        for(Int_t k=0; k<scale.size(); k++){
            rate[k] += tmp_rate[k];
            rate_qerr[k] += tmp_rate_err[k]*tmp_rate_err[k];
        }
    }
    for(Int_t k=0; k<rate_qerr.size(); k++){
        rate_err[k] = sqrt(rate_qerr[k]);
    }
    TGraphErrors g_out(scale.size(), &scale[0], &rate[0], &scale_err[0], &rate_err[0]);
    //f_out.cd();
    TString g_out_name = Form("%s_%s_%s_%s_%d_%d", gen.Data(), metric.Data(), p_type.Data(), p_nrg.Data(), i, r);
    g_out.Write(g_out_name, TObject::kOverwrite);

    return 0;
}



Int_t sep_collect(){
    #include "../constants.h"
    TString file_stem = "/home/garrettl/projects/rrg-jmammei/garrettl";

    TString in_name = Form("%s/analysis/current-scan/collect.root", file_stem.Data());
    TFile *f = new TFile(in_name);
    TFile f_out("sep_collect.root", "RECREATE");
    f_out.cd();

    for(Int_t g=0; g<gen.size(); g++){
        for(Int_t m=0; m<metric.size(); m++){
            for(Int_t p=0; p<p_type.size(); p++){
                for(Int_t q=0; q<p_nrg.size(); q++){
                    for(Int_t r=0; r<n_ring+1; r++){
                        if(r!=0 && r!=5){ 
                            for(Int_t i=0; i<n_not_sector5+1; i++){ 
                                collector(f, n_septant, gen[g], metric[m], p_type[p], p_nrg[q], i, r, scale);
                            }
                        } else{
                            for(Int_t i=0; i<n_sector+1; i++){
                                collector(f, n_septant, gen[g], metric[m], p_type[p], p_nrg[q], i, r, scale);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

