
int DrawGate2(){

    gROOT->SetStyle("ATLAS");

    int b1;
    double b2, b3, b4, b5;
    ifstream ifs("./gate2.dat");

    auto hist = new TH2D("hist","hist",69,5,695, 19,0.1,2.0);

    for(int i=0;i<19;++i){
        for(int j=0;j<69;++j){
            ifs >> b1 >> b2 >> b3 >> b4 >> b5;
                hist->SetBinContent(j, i, b4);
        }
    }

    hist->SetTitle(";Gate (ns);Energy region (MeV)");
    hist->Draw("cont1z");

    return 0;
    
}

