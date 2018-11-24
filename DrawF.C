
int DrawF(){

    gROOT->SetStyle("ATLAS");

    double b2, b3, b4, b5;
    ifstream ifs("./EvsF_ga.dat");
    ifstream ifs2("./EvsF_si.dat");
    ifstream ifs3("./EvsF_ga200.dat");

    auto hist = new TH1D("hist","hist",189,0.1,2.0);
    auto hist2 = new TH1D("si","si",189,0.1,2.0);
    auto hist3 = new TH1D("ga200","ga200",189,0.1,2.0);

    for(int i=0;i<189;++i){
        ifs >> b2 >> b3 >> b4 >> b5;
        hist->SetBinContent(i, b4);
    }
    for(int i=0;i<189;++i){
        ifs2 >> b2 >> b3 >> b4 >> b5;
        hist2->SetBinContent(i, b4);
    }
    for(int i=0;i<189;++i){
        ifs3 >> b2 >> b3 >> b4 >> b5;
        hist3->SetBinContent(i, b4);
    }

    hist->SetTitle(";Energy (MeV);F-measure");

    hist2->SetLineColor(kRed);
    hist3->SetLineStyle(2);
    hist->Draw();
    hist2->Draw("same");
    hist3->Draw("same");

    auto leg = new TLegend();
    leg->AddEntry(hist2, "Shape indicator","l");
    leg->AddEntry(hist, "Diff. Gate (Optimized)","l");
    leg->AddEntry(hist3, "Diff. Gate (Non opt.)","l");
    leg->Draw("same");

    return 0;
    
}

