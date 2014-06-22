#ifndef PALETTE_H_
#define PALETTE_H_

#include <iosfwd>
#include <memory>

#include <SDL.h>

namespace core
{
    class Color;
}

namespace castle
{
    class Palette
    {
    protected:
        std::shared_ptr<SDL_Palette> mStorage;
        
    public:
        typedef SDL_Color         value_type;
        typedef SDL_Color const*  const_iterator;
        typedef SDL_Color*        iterator;
        typedef size_t            size_type;

        Palette();
        explicit Palette(size_t colors);
        Palette(Palette const&);
        Palette& operator=(Palette const&);
        Palette(Palette&&);
        Palette& operator=(Palette&&);
        virtual ~Palette();
        
        value_type const& operator[](size_type index) const;
        value_type& operator[](size_type index);
        size_type Size() const;
        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();

        bool Null() const;
        bool operator!() const;

        SDL_Palette& GetSDLPalette();
    };
}

#endif  // PALETTE_H_
