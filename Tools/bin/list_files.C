vector<TString> list_files(const char *dirname="./", const char *ext=".root")
{
   vector<TString> ret;
   TSystemDirectory dir(dirname, dirname);
   TList *files = dir.GetListOfFiles();
   if (files) {
      TSystemFile *file;
      TString fname;
      TIter next(files);
      while ((file=(TSystemFile*)next())) {
         fname = file->GetName();
         if (!file->IsDirectory() && fname.EndsWith(ext)) {
            cout << fname.Data() << endl;
            ret.push_back(fname);
         }
      }
   }
   return ret;
}
