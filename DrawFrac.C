
TGraph* getGraph(int runnum = 5){

    vector<double> si, frac;
    double b1;
    int b2, b3;
    ifstream ifs(Form("fra%01d.dat",runnum));

    while(ifs >> b1 >> b2 >> b3){
        si.push_back(b1);
        frac.push_back((double)b3/(double)b2);
    }

    auto tg = new TGraph(si.size(),si.data(),frac.data());
    return tg;
    
}

int DrawFrac(){

    gROOT->SetStyle("ATLAS");

    auto tg5 = getGraph(5);
    auto tg6 = getGraph(6);

    tg5->SetTitle(";Shape Indicator;Remaining Fraction");
    tg6->SetLineColor(kRed);

    tg5->Draw("AL");
    tg6->Draw("L");

    auto leg = new TLegend();
    leg->AddEntry(tg5,"Gamma","l");
    leg->AddEntry(tg6,"Alpha","l");
    leg->Draw("same");

    return 0;
}
