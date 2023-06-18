namespace dragon {
  /// Run the unpacker
  int Mid2Root(const TString& args);

  /// Chain together all trees in multiple DRAGON files.
  void MakeChains(const char* prefix, const Int_t* runnumbers, Int_t nruns, const char* format = "$DH/rootfiles/run%d.root", Bool_t sonik = kFALSE);

  /// Chain together all trees in multiple DRAGON files.
  void MakeChains(const Int_t* runnumbers, Int_t nruns, const char* format = "$DH/rootfiles/run%d.root", Bool_t sonik = kFALSE);

  /// Chain together trees using a vector instead of array
  void MakeChains(const std::vector<Int_t>& runnumbers, const char* format = "$DH/rootfiles/run%d.root", Bool_t sonik = kFALSE);

  /// Chain together trees using a vector instead of array
  void MakeChains(const char* prefix, const std::vector<Int_t>& runnumbers, const char* format = "$DH/rootfiles/run%d.root", Bool_t sonik = kFALSE);
}
