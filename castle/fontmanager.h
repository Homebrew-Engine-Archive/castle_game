#ifndef FONT_MANAGER_H_
#define FONT_MANAGER_H_

namespace Render
{

    class Renderer;
    
    class FontManager
    {
    public:
        virtual void AddFont(const std::string &filename, font_size_t size) = 0;
        virtual void ParseCollection(const CollectionData &data, const FontCollectionInfo &info) = 0;
    };

}

#endif
