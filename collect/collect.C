
Int_t collect(){
    #include "../constants.h"
    TString file_stem = "/home/garrettl/projects/rrg-jmammei/garrettl";

    TFile f_out("collect.root", "RECREATE");
    
    std::vector<std::vector<Int_t>> ring_non5; 
    ring_non5.push_back(std::vector<Int_t> {0});
    ring_non5.push_back(std::vector<Int_t> {1, 2});
    ring_non5.push_back(std::vector<Int_t> {3, 4, 5});
    ring_non5.push_back(std::vector<Int_t> {6, 7, 8});
    ring_non5.push_back(std::vector<Int_t> {9, 10, 11});
    ring_non5.push_back(std::vector<Int_t> {12, 13});

    std::vector<Double_t> scale_err(scale.size(), 0.0);

    TF2 *inelastic_mult = new TF2("inelastic_mult", "y", 0.0, 1.0, 0.0, 2000.0);

    for(Int_t g=0; g<gen.size(); g++){
        TString in_name = Form("%s/mag_over10M/int_data/%s.root", file_stem.Data(), gen[g].Data());
        TFile *f = new TFile(in_name);
        for(Int_t m=0; m<metric.size(); m++){
            for(Int_t p=0; p<p_type.size(); p++){
                for(Int_t q=0; q<p_nrg.size(); q++){
                    for(Int_t s=0; s<n_septant; s++){
                        for(Int_t r=0; r<n_ring+1; r++){
                            if (r!=0 && r!=5){
                                for(Int_t i=0; i<ring_non5.size(); i++){//for each of the 5 sectors
                                    std::vector<Double_t> rate(scale.size(), 0.0);
                                    std::vector<Double_t> rate_qerr(scale.size(), 0.0);
                                    std::vector<Double_t> rate_err(scale.size(), 0.0);
                                    for(Int_t j=0; j<ring_non5[i].size(); j++){//go through the subsectors and add the rates and errors
                                        TString g_name = Form("%s_%s_%s_%s_%d_%d_%d", gen[g].Data(), metric[m].Data(), p_type[p].Data(), p_nrg[q].Data(), s+1, ring_non5[i][j], r);
                                        TGraphErrors *g1 = (TGraphErrors*) f->Get(g_name);
//                                        std::cout << g_name << std::endl;
                                        Double_t * tmp_rate = g1->GetY();
                                        Double_t * tmp_rate_err = g1->GetEY();
                                        if (gen[g]=="inelastic"){//relic of old code
                                            for(Int_t k=0; k<scale.size(); k++){
                                                rate[k] += tmp_rate[k];//in order to show up well on the mg, the inelastics must be X10
                                                rate_qerr[k] += tmp_rate_err[k]*tmp_rate_err[k];
                                            }
                                        } else{
                                            for(Int_t k=0; k<scale.size(); k++){
                                                rate[k] += tmp_rate[k];
                                                rate_qerr[k] += tmp_rate_err[k]*tmp_rate_err[k];
                                            }
                                        }
                                        for(Int_t k=0; k<rate_qerr.size(); k++){
                                            rate_err[k] = sqrt(rate_qerr[k]);
                                        }
                                    }
                                    TGraphErrors g_out(scale.size(), &scale[0], &rate[0], &scale_err[0], &rate_err[0]);
                                    f_out.cd();
                                    TString g_out_name = Form("%s_%s_%s_%s_%d_%d_%d", gen[g].Data(), metric[m].Data(), p_type[p].Data(), p_nrg[q].Data(), s+1, i, r);
                                    g_out.Write(g_out_name, TObject::kOverwrite);
                                }
                            } else{
                                for(Int_t i=0; i<n_sector+1; i++){
                                    TString g_name = Form("%s_%s_%s_%s_%d_%d_%d", gen[g].Data(), metric[m].Data(), p_type[p].Data(), p_nrg[q].Data(), s+1, i, r);
//                                    std::cout << g_name << std::endl;
                                    TGraphErrors *g1 = (TGraphErrors*) f->Get(g_name);
//                                    if(gen[g]=="inelastic"){
//                                        g1->Apply(inelastic_mult);
//                                    }
                                    f_out.cd();
                                    TString g_out_name = Form("%s_%s_%s_%s_%d_%d_%d", gen[g].Data(), metric[m].Data(), p_type[p].Data(), p_nrg[q].Data(), s+1, i, r);
                                    g1->Write(g_out_name, TObject::kOverwrite);
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    return 0;

}
