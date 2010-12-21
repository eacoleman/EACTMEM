{
 gSystem->Load("libFWCoreFWLite.so");
 AutoLibraryLoader::enable();
 gSystem->Load("libDataFormatsFWLite.so");
 gROOT->SetStyle ("Plain");
 gSystem->Load("libRooFit") ;
 gSystem->Load("../lib/slc5_ia32_gcc434/libTAMUWWMEPATNtuple.so");
 using namespace RooFit ;
 cout << "loaded" << endl;
}
