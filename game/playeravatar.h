#ifndef PLAYERAVATAR_H_
#define PLAYERAVATAR_H_

namespace Castle {
    namespace World {
        class PlayerAvatar
        {
        public:
            explicit PlayerAvatar() = default;
        
            PlayerAvatar(PlayerAvatar const&) = default;
            PlayerAvatar& operator=(PlayerAvatar const&) = default;

            virtual ~PlayerAvatar() = default;
        };

        inline bool operator==(const PlayerAvatar &lhs, const PlayerAvatar &rhs)
        {
            return &lhs == &rhs;
        }

        inline bool operator<(const PlayerAvatar &lhs, const PlayerAvatar &rhs)
        {
            return &lhs < &rhs;
        }
    }
}

#endif // PLAYERAVATAR_H_
