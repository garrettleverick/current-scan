Double_t computeSquare(Double_t x){return x*x;};

//add vector component wise
std::vector<Double_t> vector_sum(std::vector<Double_t> v1, std::vector<Double_t> v2){
    std::vector<Double_t> v_out;
    for(Int_t i=0; i<v1.size(); i++){
        v_out.push_back(v1[i]+v2[i]);
    }

    return v_out;
}

//subtract vectors component wise
std::vector<Double_t> vector_sub(std::vector<Double_t> v1, std::vector<Double_t> v2){
    std::vector<Double_t> v_out;
    for(Int_t i=0; i<v1.size(); i++){
        v_out.push_back(v1[i]-v2[i]);
    }

    return v_out;
}


std::vector<Double_t> vector_square(std::vector<Double_t> v1){
    std::vector<Double_t> v_out;
    for(Int_t i=0; i<v1.size(); i++){
        v_out.push_back(v1[i]*v1[i]);
    }

    return v_out;
}


std::vector<Double_t> vector_sqrt(std::vector<Double_t> v1){
    std::vector<Double_t> v_out;
    for(Int_t i=0; i<v1.size(); i++){
        v_out.push_back(sqrt(v1[i]));
    }

    return v_out;
}

void calculator(TString metric, TString p_nrg, Int_t j, Int_t k ,Int_t m, std::vector<Double_t> scale, std::vector<TString> gen, TFile* f_in, TFile* f_out){
    Double_t * tmp_y_val;
    Double_t * tmp_y_err;
    std::vector<TGraphErrors*> graphs;
    std::vector<std::vector<Double_t>> values;
    std::vector<std::vector<Double_t>> errors;
    std::vector<Double_t> empty;
    std::vector<Double_t> scale_err(scale.size(), 0.0);
    TGraphErrors *g1;
    
    //f_in->Cd();

    TString in_stem = Form("%s_primary_%s_%d_%d_%d", metric.Data(), p_nrg.Data(), j+1, k, m);
    TString g_name = Form("beam_%s_electron_%s_%d_%d_%d", metric.Data(), p_nrg.Data(), j+1, k, m);
    g1 = (TGraphErrors*) f_in->Get(g_name);
    graphs.push_back(g1); 
    for(Int_t g=0; g<gen.size(); g++){
        g_name = Form("%s_%s", gen[g].Data(), in_stem.Data()); 
        g1 = (TGraphErrors*) f_in->Get(g_name);
        graphs.push_back(g1);
    }
    
    for(Int_t g=0; g<graphs.size(); g++){
        tmp_y_val = graphs[g]->GetY();
        tmp_y_err = graphs[g]->GetEY();
        values.push_back(empty);
        errors.push_back(empty); 
        for(Int_t s=0; s<scale.size(); s++){
            values[g].push_back(tmp_y_val[s]);
            errors[g].push_back(tmp_y_err[s]);
        }
    }
   
    std::vector<Double_t> rate = vector_sum(values[0], values[4]);
    rate = vector_sub(rate, values[1]);
    rate = vector_sub(rate, values[2]);
    rate = vector_sub(rate, values[3]);

    for(Int_t g=0; g<errors.size(); g++){errors[g] = vector_square(errors[g]);} //square all errors
    std::vector<Double_t> rate_err = vector_sum(errors[0], errors[1]);
    rate_err = vector_sum(rate_err, errors[2]);
    rate_err = vector_sum(rate_err, errors[3]);
    rate_err = vector_sum(rate_err, errors[4]); 
    rate_err = vector_sqrt(rate_err);
    //f_out->Cd();

    TGraphErrors *g_out = new TGraphErrors(scale.size(), &scale[0], &rate[0], &scale_err[0], &rate_err[0]);
    TString g_out_name = Form("beam_%s_electron_%s_%d_%d_%d", metric.Data(), p_nrg.Data(), j+1, k, m);
    g_out->Write(g_out_name, TObject::kOverwrite);

}   


Int_t beam_secondary_e(TString field_map){
    #include "../constants.h"    
   
    TString file_stem = "/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/";
    TString in_name = Form("%s%s/collect.root", file_stem.Data(), field_map.Data());
    TString out_name = Form("%s%s/good_e.root", file_stem.Data(), field_map.Data());
    
    TFile *f_in = new TFile(in_name, "READ");
    f_in->Cp(out_name);
    TFile *f_out = new TFile(out_name, "UPDATE");

    for(Int_t i=0; i<metric.size(); i++){
        for(Int_t q=0; q<p_nrg.size(); q++){
            for(Int_t j=0; j<n_septant; j++){
                for(Int_t m=0; m<n_ring+1; m++){
                    if(m!=5 && m!=0){
                        for(Int_t k=0; k<n_not_sector5+1; k++){
                            calculator(metric[i].Data(), p_nrg[q].Data(), j, k, m, scale, gen, f_in, f_out);                                                         
                            cout << Form("%s_%d_%d_%d", p_nrg[q].Data(), j+1, k, m) << endl;
                        }
                    } else{
                        for(Int_t k=0; k<n_sector+1; k++){
                            calculator(metric[i].Data(), p_nrg[q].Data(), j, k, m, scale, gen, f_in, f_out);                                                         
                            cout << Form("%s_%d_%d_%d", p_nrg[q].Data(), j+1, k, m) << endl;
                        }
                    }
                }
            }
        }
    }
    

    return 0;

}


