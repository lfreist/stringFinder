add_library(SFReaders
        ESFPOSIXReader.h ESFPOSIXReader.cpp
        ESFIFSTREAMReader.h ESFIFSTREAMReader.cpp
        ESFCompressedPOSIXReader.h ESFCompressedPOSIXReader.cpp
        ESFCompressedIFSTREAMReader.h ESFCompressedIFSTREAMReader.cpp
        ISFReader.h ISFReader.cpp
        ISFCompressedReader.h ISFCompressedReader.cpp
        )
target_link_libraries(SFReaders PUBLIC SFMetaFile)