#include "rendermode.h"

#include "game/gm1.h"
#include "game/gm1reader.h"

#include "tgxrendermode.h"
#include "strutils.h"

#include <memory>
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
        RegisterFormats();
        for(const auto &ptr : mFormats) {
            ptr->RegisterOptions(desc);
        }
        
        bpo::options_description renderOptions("Render mode options");
        renderOptions.add_options()
            ("format", bpo::value<std::string>()->default_value(DefaultFormat().c_str()), SupportedFormats().c_str())
            ("palette", bpo::value<int>()->default_value(0), "Palette index to use when render 8-bit image")
            ;

        desc.add(renderOptions);
    }

    std::string RenderMode::SupportedFormats() const
    {
        std::vector<std::string> formats;
        for(const auto &ptr : mFormats) {
            formats.push_back(ptr->ModeName());
        }
        return StringUtils::JoinStrings(formats.begin(), formats.end(), ", ", "<no formats>");
    }

    std::string RenderMode::DefaultFormat() const
    {
        if(mFormats.empty()) {
            // TODO maybe throw an exception?
            return std::string("<no format>");
        }
        return mFormats.front()->ModeName();
    }
    
    void RenderMode::RegisterFormats()
    {
        mFormats.emplace_back(new TGXRenderMode);
    }
    
    int RenderMode::HandleMode(const bpo::variables_map &vars)
    {
        std::string format = vars["format"].as<std::string>();
        for(std::unique_ptr<ModeHandler> &ptr : mFormats) {
            if(ptr->ModeName() == format) {
                return ptr->HandleMode(vars);
            }
        }
        
        throw std::runtime_error("Unknown format");
    }
    
}
