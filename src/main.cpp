#include <iostream>
#include <GameBoy.h>
#include <Debugger.h>

int main()
{
    srand(time(0));

    GameBoy gb = GameBoy();
    Debugger d = Debugger(&gb);

    gb.Pause(true);

    while (d.Running())
    {
        //for (int i = 0; i < 100; i++)
            gb.Cycle();
        d.Display();
    }
}
