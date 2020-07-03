
void list_histos(const char *fname) {
    TKey *key; 
    TFile *f = TFile::Open(fname, "READ"); 
    if (!f || f->IsZombie()) {
        cout << "Unable to open " << fname << " for reading..." <<endl;
        return;
    }
    Int_t total = 0;
    TIter next((TList *)f->GetListOfKeys());
    while (key = (TKey *)next()) {
        TClass *cl = gROOT->GetClass(key->GetClassName());
        if (cl->InheritsFrom("TH1")){ 
        // the following line is not needed if you only want // to count the histograms 
        // TH1 *h = (TH1 *)key->ReadObj();
        //cout << "Histo found: " << h->GetName() << " - " << h->GetTitle() << endl;
        total++;
        }
    }
    cout << "Found " << total << " Histograms" << endl;
}
