#include "main.h"

using namespace std;

int main()
{
    SDLContext(SDL_INIT_EVERYTHING);
    SDLWindow window("SH", 1024, 768);
    Renderer renderer(window);
    
    if(RunLoadingScreen(renderer)) {
        SDL_Log("Loading has been interrupted.");
        return 0;
    }
    
    Screen *rootScreen = new GameScreen;
    
    bool quit = false;
    const bool frameLimit = false;
    const int frameRate = 16;
    const int frameInterval = 1000 / frameRate;
    const int pollRate = 66;
    const int pollInterval = 1000 / pollRate;
    
    int lastFrame = 0;
    int lastPoll = 0;
    
    boost::asio::io_service io;
    boost::asio::ip::tcp::acceptor acceptor(io);
    boost::asio::ip::tcp::socket socket(io);

    boost::asio::deadline_timer timer(io, boost::posix_time::seconds(1));
    
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
            const int tickDelta = delayStart - pollStart;
            const int nextTick =
                std::min(lastPoll + pollInterval,
                         lastFrame + frameInterval);
            if(tickDelta < nextTick) {
                SDL_Delay(nextTick - tickDelta);
            }
        }
    }
    
    return 0;
}

