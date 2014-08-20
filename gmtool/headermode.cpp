#include "headermode.h"

#include <iostream>

#include <gm1/gm1writer.h>
#include <gm1/gm1reader.h>
#include <gm1/gm1entryreader.h>

namespace gmtool
{
    int HeaderMode::Exec(const ModeConfig &cfg)
    {
        cfg.verbose << "Reading file " << mInputFile << std::endl;
        gm1::GM1Reader reader(mInputFile);

        cfg.verbose << "Collection has " << reader.NumEntries() << " entries" << std::endl;

        const gm1::Header &header = reader.Header();

        if(mReaderTypeRequested) {
            cfg.stdout << reader.GetEntryReader().GetName() << std::endl;
            return EXIT_SUCCESS;
        }

        if(mCountRequested) {
            cfg.stdout << reader.NumEntries() << std::endl;
            return EXIT_SUCCESS;
        }

        if(!mBinary) {
            gm1::PrintHeader(cfg.stdout, header);
        } else {
            gm1::WriteHeader(cfg.stdout, reader.Header());
        }
        return EXIT_SUCCESS;
    }
}
