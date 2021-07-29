#ifndef command_h
#define command_h

#if 0
struct Command
{
    virtual ~Command();
    virtual void executeGameAction(Game * game) = 0;
    virtual void executePlayerAction(Player * player) = 0;
};

struct RotateLeftCommand : public Command
{
    virtual void executePlayerAction(Player * player)
    {
        //player->rotateLeft();
    }
};

struct RotateRightCommand : public Command
{
    virtual void executePlayerAction(Player * player)
    {
        //player->rotateRight();
    }
};

struct StartGameCommand : public Command
{
    virtual void executeGameAction(Game * game)
    {
        //game.startGame();
    }
}
#endif

#endif /* command_h */
