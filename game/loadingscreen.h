#ifndef LOADINGSCREEN_H_
#define LOADINGSCREEN_H_

#include <string>

#include <game/screen.h>

namespace Render
{
    class Renderer;
}

namespace Castle
{
    namespace UI
    {
        class LoadingScreen : public Screen
        {
            int mProgressDone;
            int mProgressMax;
            std::string mStage;

        public:
            explicit LoadingScreen();
            virtual ~LoadingScreen();
            
            void SetProgressDone(int done);
            void SetProgressMax(int max);
            void IncreaseDone(int delta = 1);
            void SetProgressLabel(std::string const&);
            double GetCompleteRate() const;
            void Render(Render::Renderer &renderer);
            bool HandleEvent(SDL_Event const&);
        };
    }
}

#endif  // LOADINGSCREEN_H_
