#ifndef COLLECTIONS_H_
#define COLLECTIONS_H_

namespace UI
{
    class LoadingScreen;
}

namespace Render
{
    class Collections
    {
    public:
        Collections();
        size_t AddGroup(std::string archive, std::string group, size_t from, size_t size, size_t stride = 1);
        
        bool LoadAll(UI::LoadingScreen *screen);

        Surface GetGroup(const std::string &archive, const std::string &group, size_t index);
    };
    
}

#endif
