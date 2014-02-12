#include "main.h"

using namespace std;

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);
    SDLWindow window("SH", 640, 480);
    SDLRenderer renderer(window);

    vector<string> files;
    LoadStringList("gm1list.txt", back_inserter(files));

    map<string, GM1CollectionScheme> collections;

    map<pair<string, size_t>, GM1Entry> entries;
    for(const auto &file : files) {
        SDL_Log("Reading %s", file.c_str());
        SDL_RWops *src = SDL_RWFromFile(file.c_str(), "rb");

        try {
            GM1CollectionScheme scheme(src);
            collections.insert(
                make_pair(
                    file, scheme));

            vector<GM1Entry> xs;
            LoadEntries(src, scheme, xs);
            for(size_t i = 0; i < xs.size(); ++i)
                entries.insert(make_pair(make_pair(file, i), xs[i]));
            
        } catch(const EOFError &eof) {
            SDL_Log("An error occured while reading %s", file.c_str());
            SDL_Log("\t%s", eof.what());
        } catch(const FormatError &fmt) {
            SDL_Log("An error occured while decoding %s", file.c_str());
            SDL_Log("\t%s", fmt.what());
        }
        
        SDL_RWclose(src);
    }
    
    bool quit = false;
    while(!quit) {
        SDL_Event e;
        SDL_PumpEvents();
        if(SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                }
                break;
            }
        }
        
        SDL_FilterEvents(InputEventFilter, NULL);

        renderer.Present();
        SDL_Delay(10);
    }
    
    return 0;
}

int InputEventFilter(void*, SDL_Event *event)
{
    switch(event->type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        return 0;
    }
    return 1;
}

template<class OutputIterator>
void LoadStringList(const char * filename, OutputIterator out)
{
    ifstream fin(filename);
    string line;
    while(getline(fin, line))
        *out++ = line;
}

bool IsFileExists(SDL_RWops *src)
{
    return (SDL_RWsize(src) != 0);
}
