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

    constexpr bool operator==(const PlayerAvatar &lhs, const PlayerAvatar &rhs)
    {
        return true;
    }
}

#endif // PLAYERAVATAR_H_
