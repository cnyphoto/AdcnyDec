#include <iostream>
#include "Operation.h"

int main(int argc, char* argv[])
{
	DbToCode dtcode;
	dtcode.setConstr("127.0.0.1", 3306, "root", "daitto", "surface");

	SorcePar spar = dtcode.getPar(1);

	std::vector<modpar> mods = dtcode.getMod(spar.id);

	for (auto it = mods.begin(); it != mods.end(); ++it) {
		it->trained = spar.trained;
	}

	spar.mods.push_back(mods[0]);

	Operation Operation(dtcode, spar);

	Operation.star(argc > 1, argc, argv);

	//system("pause");
}
