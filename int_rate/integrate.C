#include <fstream>
#include <iostream>

std::vector<Double_t> integral(TH1D* h, Double_t xmin, Double_t xmax){
    std::vector<Double_t> sum;


    TAxis *axis = h->GetXaxis();
    Int_t bmin = axis->FindBin(xmin); 
    Int_t bmax = axis->FindBin(xmax);

    Double_t error= 0;
    Double_t integral = h->IntegralAndError(bmin,bmax, error); // This is a slight over estimate of the error. But to our precision it shouldn't be noticeable. 
    integral -= h->GetBinContent(bmin)*(xmin-axis->GetBinLowEdge(bmin))/axis->GetBinWidth(bmin); //why divide by bin width
    integral -= h->GetBinContent(bmax)*(axis->GetBinUpEdge(bmax)-xmax)/axis->GetBinWidth(bmax);

    sum.push_back(65*7*integral); 
    sum.push_back(65*7*error);  //here is where we multiply by 7 and 65 again so our units are nice and we match the experiment current


    return sum;
}


Double_t addErrorInQuad(Float_t a, Float_t b, Float_t c){

    return sqrt(a*a+b*b+c*c);

}

TGraphErrors plot(TString gen, TString metric, TString p_type, TString p_nrg, Int_t septant, Int_t sector, Int_t ring, std::vector<Double_t> scale){

    Double_t rmin;
    Double_t rmax;

    std::map<TString, TString> histname;
    histname["rate"]=Form("%s_%s_%d_%d_%d", p_type.Data(), p_nrg.Data(), septant, sector, ring);
    histname["asy"]= Form("%s_%s_%d_%d_%d_asy", p_type.Data(), p_nrg.Data(), septant, sector, ring);
    histname["fom"]= Form("%s_%s_%d_%d_%d_fom", p_type.Data(), p_nrg.Data(), septant, sector, ring);

    std::vector<std::vector<Double_t>> rate_integral;
    std::vector<Double_t> off;
    std::vector<Double_t> off_error;
    std::vector<Double_t> rate;
    std::vector<Double_t> rate_error;
    //THStack rate_hs("hs", Form("%s %s, septant: %d, sector: %d, ring: %d", gen.Data(), metric.Data(), septant, sector, ring));
    std::vector<TFile*> source;



    for(Int_t i=0;i < scale.size(); i++){ 
        //NOTE: Be careful, harcoded path! Also the way the file system is set up is important for these macros
        TString source_file = Form("/home/garrettl/projects/rrg-jmammei/garrettl/current-scan_data/analysed_data_asym/%.3f/%s/%s.root", scale[i], gen.Data(), gen.Data()); 
        source.push_back(new TFile(source_file));
        TH1D *h_rate=(TH1D*) source[i]->Get(histname[metric]);
        rmin = h_rate->GetXaxis()->GetXmin(); 
        rmax = h_rate->GetXaxis()->GetXmax();
        //rate_hs.Add(h_rate);
        h_rate->Scale(1/100.0); //divide by the number of files within the folder
        rate_integral.push_back(integral(h_rate, rmin, rmax));  
        off.push_back(i);
        off_error.push_back(0);
        if (metric=="asy"){
            rate.push_back(h_rate->GetMean());
            rate_error.push_back(h_rate->GetMeanError());
        } else if (metric=="fom"){
            rate.push_back(sqrt(rate_integral[i][0]));
            rate_error.push_back(rate_integral[i][1]/(2*sqrt(rate_integral[i][0])));
        } else{	 
            rate.push_back(rate_integral[i][0]);
            rate_error.push_back(rate_integral[i][1]);  
        }
        source[i]->Close();
    }


    //gStyle->SetOptFit(0);

    //TCanvas c("c", "c", 800, 600);

    TGraphErrors graph(scale.size(), &scale[0], &rate[0],  &off_error[0], &rate_error[0]); 

    //graph.Fit("pol1");
    //gPad->Update();
    //TPaveStats *st = (TPaveStats*)graph.FindObject("stats");
    //st->SetX1NDC(0.6); 
    //st->SetX2NDC(0.9);
    //c.Print(Form("graph_%s_%s_%d_%s_%d.png", gen.Data(), metric.Data(), septant, sector.Data(), ring));

    //rate_hs.Draw("HISTnostack");
    //c.Print(Form("out/overlay_%s_%s_%d_%s_%d.png", gen.Data(), metric.Data(), septant, sector.Data(), ring));


    return graph;
}

//Should run using int_macro.py
Int_t integrate(TString generator){
    #include "/home/garrettl/projects/rrg-jmammei/garrettl/analysis/current-scan/constants.h"
    
    TFile f("int_rate.root", "RECREATE");
    std::cout << generator << std::endl;

    for(Int_t l=0 ; l<metric.size() ; l++){	
        for(Int_t p=0; p<p_type.size(); p++){
            std::cout << p_type[p] << std::endl;
            for(Int_t q=0; q<p_nrg.size(); q++){ 
                std::cout << p_nrg[q] << std::endl;
                for(Int_t k=0; k<n_septant; k++){
                    std::cout << k+1 << std::endl;
                    for(Int_t j=0; j<n_sector+1; j++){ 
                        for(Int_t m=0; m<n_ring+1; m++){
                            TGraphErrors g_err=plot(generator, metric[l], p_type[p], p_nrg[q], k+1, j, m, scale);
                            f.cd();
                            g_err.Write(Form("%s_%s_%s_%s_%d_%d_%d", generator.Data(), metric[l].Data(), p_type[p].Data(), p_nrg[q].Data(), k+1, j, m), TObject::kOverwrite); 
                        }
                    }
                }
            }
        }
    } 
    

    f.Close();
    return 0;
}
