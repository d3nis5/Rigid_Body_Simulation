/**
 * Bakalarska praca - Simualace pevnych teles
 * VUT FIT, 2018/2019
 *
 * Autor:	Denis Leitner, xleitn02
 * Subor:	Main.cpp
 *
 */

#include "Main.h"


int main(void)
{
	Simulation simulation;

	if (simulation.initialize())
	{
		simulation.run();
	}
	else
	{
		return 1;
	}

	return 0;
}