#include <iostream>
#include "Operation.h"

int main(int argc, char* argv[])
{
	DbToCode dtcode;

	SorcePar spar = dtcode.getPar();

	std::vector<modpar> mods = dtcode.getMod(spar.id);

	for (auto it = mods.begin(); it != mods.end(); ++it) {
		it->trained = spar.trained;
	}

	spar.mods.push_back(mods[0]);

	Operation Operation(dtcode, spar);

	Operation.star(argc > 1, argc, argv);

	//system("pause");
}
