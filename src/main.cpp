#include "main.h"

using namespace std;

enum class ScreenIdentity : int {
    MainMenuScreen,
    MilitaryScreen,
    /* |---> */MilitaryCampaignScreen,
    /* |---> */SiegeScreen,
    /* |---> */InvasionScreen,
    /* +---> */MultiplayerScreen,
    EconomicScreen,
    /* |---> */EconomicCampaignScreen,
    /* |---> */EconomicMissionScreen,
    /* +---> */SandboxScreen,
    MapEditorScreen,
    GameScreen
};

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);

    EnumRenderDrivers();
    
    SDLWindow window("SH", 1024, 768);
    Renderer renderer(window);

    PrintRendererInfo(renderer.GetRenderer());
   
    if(RunLoadingScreen(renderer)) {
        std::cerr << "Loading has been interrupted" << std::endl;
        return 0;
    }
    
    Screen *rootScreen = new GameScreen;
    
    const bool frameLimit = true;
    const int frameRate = 16;
    const int frameInterval = 1000 / frameRate;
    const int pollRate = 66;
    const int pollInterval = 1000 / pollRate;

    bool quit = false;
    int lastFrame = 0;
    int lastPoll = 0;
    
    boost::asio::io_service io;

    Server server(io, 4500);
    server.StartAccept();
    
    while(!quit) {
        const int pollStart = SDL_GetTicks();
        if(lastPoll + pollInterval < pollStart) {
            io.poll();
            lastPoll = pollStart;
        }
        
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(!rootScreen->HandleEvent(e))
                quit = true;
        }

        const int frameStart = SDL_GetTicks();
        if(lastFrame + pollInterval < frameStart) {
            rootScreen->Draw(renderer);
            lastFrame = frameStart;
        }

        if(frameLimit) {
            const int delayStart = SDL_GetTicks();
            const int nextTick =
                std::min(lastPoll + pollInterval,
                         lastFrame + frameInterval);
            if(delayStart < nextTick) {
                SDL_Delay(nextTick - delayStart);
            }
        }
    }
    
    return 0;
}
