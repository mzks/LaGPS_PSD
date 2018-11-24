

int gammaFit(){

    auto f = TFile::Open("./spectrum.root");
    auto h = dynamic_cast<TH1D*>(f->Get("area05"));

    h->Draw();

    double fmin = 0.06e-6;
    double fmax = 0.12e-6;
    auto func = new TF1("func","gaus(0) + expo(3)",fmin,fmax);

    func->SetLineColor(kRed);
    func->SetParameter(0, 61);
    func->SetParameter(1, 8.5e-8);
    func->SetParameter(2, 4.3e-9);
    func->SetParameter(3, 8);
    func->SetParameter(4, -8.6e7);

    h->Fit("func","","",fmin,fmax);

    return 0;
}
