#include "rendermode.h"

#include "game/gm1.h"
#include "game/gm1reader.h"

#include <boost/program_options.hpp>
#include <string>

namespace bpo = boost::program_options;

namespace GMTool
{

    std::string RenderMode::ModeName() const
    {
        return "render";
    }
    
    void RenderMode::RegisterOptions(bpo::options_description &desc)
    {
        bpo::options_description renderOptions("Render mode options");
        renderOptions.add_options()
            ("renderer", bpo::value<std::string>()->default_value("tgx"), "tgx, bmp, png, raw")
            ("palette-index", bpo::value<int>()->default_value(0), "Palette index to use when render 8-bit image")
            ;

        desc.add(renderOptions);
    }

    int RenderMode::HandleMode(const bpo::variables_map &vars)
    {
        throw std::runtime_error("Not implemented");
    }
    
}
