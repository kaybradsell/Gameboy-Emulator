#include <iostream>
#include <GameBoy.h>
#include <Debugger.h>

int main()
{
    srand(time(0));

    GameBoy gb = GameBoy();
    Debugger d = Debugger(&gb);

    while (d.Running())
    {
        d.Display();
    }
}
