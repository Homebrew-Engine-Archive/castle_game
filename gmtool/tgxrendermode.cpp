#include "tgxrendermode.h"

#include <boost/program_options.hpp>
#include <string>
#include <vector>

namespace bpo = boost::program_options;

namespace GMTool
{
    void TGXRenderMode::RegisterOptions(bpo::options_description &desc)
    {

    }
    
    std::string TGXRenderMode::ModeName() const
    {
        return std::string("tgx");
    }
    
    int TGXRenderMode::HandleMode(const bpo::variables_map &vars)
    {
        return 0;
    }
    
}
