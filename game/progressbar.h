#ifndef PROGRESSBAR_H_
#define PROGRESSBAR_H_

namespace UI
{
    class Widget
    {
    public:
        virtual void SetEnabled(bool on) = 0;
        virtual bool IsEnabled() const = 0;
    };
    
    class ProgressBar : public Widget
    {
    protected:
        virtual void OnComplete() { }
    public:
        virtual void SetProgressMax(int max) = 0;
        virtual void SetProgressDone(int done) = 0;
        virtual double GetCompleteRate() const = 0;
    };

    class Button : public Widget
    {
    protected:
        virtual void OnPressed() { }
        virtual void OnReleased() { }
        virtual void OnMouseIn() { }
        virtual void OnMouseOver() { }
        virtual bool IsMouseIn(int x, int y) const = 0;
    public:
        virtual void Click() = 0;
    };

}

#endif
