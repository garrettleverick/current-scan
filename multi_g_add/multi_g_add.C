
int plot_save(TString metric, Int_t septant, Int_t sector, Int_t ring, std::vector<Int_t> colour, std::vector<Int_t> m_style, TFile *f1, TFile *f2, TCanvas *c, std::vector<Double_t> scale, TString field_map, TString out_dir){
 
    TMultiGraph *mg = new TMultiGraph(); 

    Int_t sum_mode;
    //sum mode 1 is a summation over all listed energies, types, and generators in an entry of graphs
    //sum mode 2 is a more specific summation, only choosing specific plots (generators, types, and energies)
    //placing a 2_ in front of the generator name will allow you to plot from a second file
    
    std::vector<std::vector<std::vector<TString>>> graphs;
    //graphs.push_back({{list of gen}, {list of p_type}, {list of p_nrg}, {title}, {scale}, {sum_mode=1}}); is the format for sum mode 1
    //graphs.push_back({{list of names}, {}, {}, {title}, {scale}, {sum_mode=2}}); is the format for sum mode 2
    graphs.push_back({{"moller"}, {"electron"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"moller electron, >1MeV"}, {"1"}, {"1"}});
    graphs.push_back({{"elastic"}, {"electron"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"elastic electron, >1MeV"}, {"1"}, {"1"}});
    graphs.push_back({{"inelastic"}, {"electron"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"inelastic electron, >1MeV"}, {"1"}, {"1"}});
    //graphs.push_back({{"moller", "elastic", "inelastic"}, {"positron"}, {"1_to_10", "10_to_100", "100_to_1000", "gte_1000"}, {"phys e+, >1MeV"}, {"1"}, {"1"}});

    TString title; //will store title given above
    TGraphErrors *g1;

    Double_t * tmp_y_val; //temporarily store graph's value for summation
    Double_t * tmp_y_err;
    std::vector<Double_t> y_val(scale.size(), 0.0);
    std::vector<Double_t> y_err_sq(scale.size(), 0.0);
    std::vector<Double_t> y_err(scale.size(), 0.0);
    std::vector<Double_t> scale_err(scale.size(), 0.0);
    
    for(Int_t i=0; i<graphs.size(); i++){ //loop over things on same TMultiGraph
        std::fill(y_val.begin(), y_val.end(), 0.0);
        std::fill(y_err.begin(), y_err.end(), 0.0); 
        std::fill(y_err_sq.begin(), y_err_sq.end(), 0.0); 

        Double_t scale_plot = graphs[i][4][0].Atof();
        sum_mode = graphs[i][5][0].Atoi();
        if(sum_mode==1){
            for(Int_t j=0; j<graphs[i][0].size(); j++){ //loop through generators
                for(Int_t k=0; k<graphs[i][1].size(); k++){ //type
                    for(Int_t l=0; l<graphs[i][2].size(); l++){ //energy
                        TString tmp_name = Form("%s_%s_%s_%s_%d_%d_%d", graphs[i][0][j].Data(), metric.Data(), graphs[i][1][k].Data(), graphs[i][2][l].Data(), septant, sector, ring);
                        TString g_name;
                        if(tmp_name.BeginsWith("2_")){
                            g_name = tmp_name(2,tmp_name.Length()-1); //cuts off the 2_
                            g1 = (TGraphErrors*) f2->Get(g_name);
                        }else {
                            g_name = tmp_name;
                            g1 = (TGraphErrors*) f1->Get(g_name);
                        }
//                        std::cout << g_name << std::endl;
                        tmp_y_val = g1->GetY();
                        tmp_y_err = g1->GetEY();                    
                        for(Int_t m=0; m<scale.size(); m++){ //scale for viewing purposes
                            y_val[m] += scale_plot*tmp_y_val[m];
                            y_err_sq[m] += scale_plot*scale_plot*tmp_y_err[m]*tmp_y_err[m];
                        }
                        for(Int_t m=0; m<scale.size(); m++){
                            y_err[m] = sqrt(y_err_sq[m]);
                        }
                    }
                }
            }   
        }else if(sum_mode==2){
            for(Int_t j=0; j<graphs[i][0].size(); j++){
                TString tmp_name = graphs[i][0][j];
                TString g_name;
                if(tmp_name.BeginsWith("2_")){
                    g_name = tmp_name(2,tmp_name.Length()-1); //cuts off the 2_
                    g1 = (TGraphErrors*) f2->Get(g_name);
                }else {
                    g_name = tmp_name;
                    g1 = (TGraphErrors*) f1->Get(g_name);
                }
//                std::cout << g_name << std::endl;
                tmp_y_val = g1->GetY();
                tmp_y_err = g1->GetEY();                    
                for(Int_t m=0; m<scale.size(); m++){ //loop through each data pointand scale for viewing purposes
                    y_val[m] += scale_plot*tmp_y_val[m];
                    y_err_sq[m] += scale_plot*scale_plot*tmp_y_err[m]*tmp_y_err[m];
                }
                for(Int_t m=0; m<scale.size(); m++){
                    y_err[m] = sqrt(y_err_sq[m]);
                }
            }
        }

        
/*      //attemp at plotting using a log scale (requires values/errors >0)
        Double_t diff;
        for(Int_t test=0; test<y_val.size(); test++){
            if(y_val[test]<=0){
                y_val[test] = 1e-5;
            }
            if(y_val[test]-y_err[test]<=1e-5){
                diff = y_val[test]-y_err[test];
                y_err[test] = y_err[test]-abs(diff)-1e-5; 
            }
        }
*/
        //name the graphs for the legend and give them a marker style and colour
        TGraphErrors *g_out = new TGraphErrors(scale.size(), &scale[0], &y_val[0], &scale_err[0], &y_err[0]);
        title = Form("%s X %s", graphs[i][3][0].Data(), graphs[i][4][0].Data());
        g_out->SetTitle(title);
        g_out->SetMarkerColor(colour[i]);
        g_out->SetLineColor(colour[i]);
        g_out->SetMarkerStyle(m_style[i]);
        mg->Add(g_out);
    
    }
    
    TString out_name;
    TString short_name = Form("%s_%d_%d_%d", metric.Data(), septant, sector, ring);
    if(field_map=="asym"){    
        mg->SetTitle(Form("%s vs Scale, Asymmetric Map, Septant: %d, Sector: %d, Ring: %d; Magnetic Field Scaling Factor; Rate [GHz]", metric.Data(), septant, sector, ring)); 
        out_name = Form("asym/%s/%s.png", out_dir.Data(), short_name.Data());
    }else if(field_map=="sym"){
        mg->SetTitle(Form("%s vs Scale, Symmetric Map, Septant: %d, Sector: %d, Ring: %d; Magnetic Field Scaling Factor; Rate [GHz]", metric.Data(), septant, sector, ring)); 
        out_name = Form("sym/%s/%s.png", out_dir.Data(), short_name.Data());
    }
   
    //gPad->SetLogy(); 
    mg->GetXaxis()->CenterTitle(1);
    mg->GetYaxis()->CenterTitle(1);
    mg->GetXaxis()->SetLimits(0.65, 1.35);
    mg->Draw("AP");

    c->Update(); 
    c->BuildLegend(0.7, 0.5, 1, 0.9);
    c->Print(Form("%s", out_name.Data())); //make sure destniiation already exists

//    f_out.cd();
    mg->Write(short_name);
    c->Write(Form("c_%s", short_name.Data()));
    return 0;
}



int multi_g_add(TString field_map, TString out_dir){
    #include "../constants.h"
    TString f1_name, f_out_name;
    TString file_stem = "/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/"; 
    f1_name = Form("%s%s/good_e.root", file_stem.Data(), field_map.Data());
    f_out_name = Form("%s%s/multi_g_add.root", file_stem.Data(), field_map.Data());
    gSystem->Exec(Form("mkdir -p %s%s/%s", file_stem.Data(), field_map.Data(), out_dir.Data()));
    
    TFile *f1 = new TFile(f1_name);
    TFile *f2 = new TFile("/home/garrettl/projects/rrg-jmammei/garrettl/mag_over10M/collect3.root");
    TFile f_out(f_out_name, "RECREATE");   
    
    std::vector<Int_t> colour = {1, 2, 3, 4, 6, 46, 7, 8, 9, 32};
    std::vector<Int_t> m_style = {20, 21, 22, 23, 29, 33, 43, 116, 34, 121};

    TCanvas *c = new TCanvas("c", "c", 800, 600);
    
    f_out.cd();

    //loop over each TMultiGraph you want to make
    for(Int_t l=0; l<metric.size(); l++){
        for(Int_t j=0; j<n_septant; j++){
            for(Int_t m=0; m<n_ring+1; m++){
                if(m!=5 && m!=0){
                    for(Int_t k=0; k<n_not_sector5+1; k++){
                        plot_save(metric[l], j+1, k, m, colour, m_style, f1, f2, c, scale, field_map, out_dir);
                        }
                } else{
                    for(Int_t k=0; k<n_sector+1; k++){
                        plot_save(metric[l], j+1, k, m, colour, m_style, f1, f2, c, scale, field_map, out_dir);
                    }
                } 
            }
        }
    }   


    return 0;

}

