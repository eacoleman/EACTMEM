#!/bin/bash
#------------------------------------------------------------
# Modified from Bernd Stelzer 2006
#------------------------------------------------------------

if [[ $# -lt 1 ]] ;
  then echo "Usage: $0 <datafile>"; exit
fi

echo "------------------------ Running makeTarball.sh"

mkdir tarball
echo "Copying" $1 "..."
cp $1 tarball
cd tarball

#------------------------------------------------------------
# Copy the root shared libraries
#------------------------------------------------------------
mkdir root
mkdir root/lib
mkdir root/cint
mkdir root/cint/include

echo "Copying root libraries..."
cp $ROOTSYS/cint/include/bool.h ./root/cint/include
cp $ROOTSYS/cint/MAKEINFO ./root/cint
cp $ROOTSYS/lib/libCint.so ./root/lib
cp $ROOTSYS/lib/libCore.so ./root/lib
cp $ROOTSYS/lib/libGpad.so ./root/lib
cp $ROOTSYS/lib/libGraf3d.so ./root/lib
cp $ROOTSYS/lib/libGraf.so ./root/lib
cp $ROOTSYS/lib/libGui.so ./root/lib
cp $ROOTSYS/lib/libHist.so ./root/lib
cp $ROOTSYS/lib/libMatrix.so ./root/lib
cp $ROOTSYS/lib/libMinuit.so ./root/lib
cp $ROOTSYS/lib/libPhysics.so ./root/lib
cp $ROOTSYS/lib/libPostscript.so ./root/lib
cp $ROOTSYS/lib/libRint.so ./root/lib
cp $ROOTSYS/lib/libTree.so ./root/lib

echo "Copying shared object libraries..."
cp ../shlib/lib* .
cp $DISKCACHE_I_DIR/lib/libdiskcache_samroot.* .
cp $DISKCACHE_I_DIR/lib/libdiskcache_samapi.* .
cp $SAM_CPP_API_DIR/lib/so/lib* .

echo "Copying executable..."
cp ../../run_MatrixElement/run_MatrixElement.exe .

echo "Copying sundry files..."
cp ../../run_MatrixElement/*.df .
cp ../../run_MatrixElement/*.tbl .
cp ../../macros/getEntriesAndSkim.exe .
#cp /usr/krb5/bin/rcp .

echo "Copying worker script..."
cp ../../run_MatrixElement/run_me_caf.sh .
cp ../../run_MatrixElement/run_me_caf_multi.sh .
cp ../../run_MatrixElement/run_MatrixElement.cc .

#------------------------------------------------------------
# Tar up the list of files specified in caf_tar.lis
#------------------------------------------------------------
echo "Making tarball..."
tar cf caf_me.tar *
echo "Zipping tarball..."
gzip caf_me.tar

#------------------------------------------------------------
# Clean up
#------------------------------------------------------------
echo "Cleaning up..."
mv caf_me.tar.gz ../

rm -rf *
cd ..
rmdir tarball
echo "caf_me.tar.gz created!"
echo "------------------------ End of makeTarball.sh"
