#ifndef PLAYERAVATAR_H_
#define PLAYERAVATAR_H_

namespace Castle
{
    class PlayerAvatar
    {
    public:
        PlayerAvatar() = default;
        
        PlayerAvatar(PlayerAvatar const&) = default;
        PlayerAvatar& operator=(PlayerAvatar const&) = default;

        ~PlayerAvatar() = default;
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

#endif // PLAYERAVATAR_H_
