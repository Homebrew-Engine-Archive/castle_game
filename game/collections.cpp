#include "collections.h"

namespace Render
{
    
    class CollectionHandler
    {
    public:
        virtual void Init(Collections &collections);
    };
    
    class Archer : public Render::CollectionHandler
    {
    public:
        void Init(Collections &collections);
    };

    void Archer::Init(Collections &cs)
    {
        cs.AddCollection("body_archer");
        
    }
    
    Collections::Collections()
    {
        
    }
    
} // namespace Render
