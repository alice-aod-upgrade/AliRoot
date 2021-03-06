void sim(Int_t nev=1) {

  // Libraries required by the simulation
  gSystem->Load("liblhapdf"); // Parton density functions
  gSystem->Load("libEGPythia6"); // TGenerator interface
  gSystem->Load("libpythia6"); // Pythia
  gSystem->Load("libAliPythia6"); // ALICE specific implementations
  gSystem->Load("libgeant321");

  gSystem->Load("libTTherminator"); // Therminator library


  AliSimulation simulator;
  simulator.SetMakeSDigits("TRD TOF PHOS HMPID EMCAL MUON FMD ZDC PMD T0 VZERO");
  simulator.SetMakeDigitsFromHits("ITS TPC");
  simulator.SetWriteRawData("ALL","raw.root",kTRUE);

  simulator.SetDefaultStorage("local://$ALICE_ROOT/OCDB");
  simulator.SetSpecificStorage("GRP/GRP/Data",
			       Form("local://%s",gSystem->pwd()));
 
  simulator.SetRunHLT("default"); // In case we do not have ancored production

  TStopwatch timer;
  timer.Start();
  simulator.Run(nev);
  timer.Stop();
  timer.Print();
}
