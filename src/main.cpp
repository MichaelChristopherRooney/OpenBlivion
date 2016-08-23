#include <src\engine.h>

int main(int argc, char** argv) {

	engine game;

	if (!game.run()) {
		printf("Press any key to exit...\n");
		getchar();
	}


}

