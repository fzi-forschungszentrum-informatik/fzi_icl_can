^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package fzi_icl_can
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.0.8 (2016-05-12)
------------------
* added license information for pcan
* add wget as build dependency
* make the package a plain cmake package again
* Contributors: Felix Mauch

1.0.7 (2016-05-11)
------------------
* add popt as a dependency as it is needed for pcan
* Contributors: Felix Mauch

1.0.6 (2016-05-11)
------------------
* add dummy libpcan files so that findPKG will find the lib.
* Contributors: Felix Mauch

1.0.5 (2016-05-11)
------------------
* build pcan library inside this package and provide a script to build the kernel module
* Contributors: Felix Mauch

1.0.4 (2016-05-04)
------------------
* get libpcan from ros package
* add third-party specific package.xml tags and install rule
* Contributors: Felix Mauch

1.0.3 (2016-05-02)
------------------
* find icl_core components correctly
* Contributors: Felix Mauch

1.0.2 (2016-05-02)
------------------
* declared package as plain cmake package
* Contributors: Felix Mauch

1.0.1 (2016-04-08)
------------------
* Initial public commit of fzi_icl_can library
* Contributors: Georg Heppner, Jan Oberlaender, Hugo Ritzkowski, Felix Mauch, Thomas Schamm, Klaus Uhl
