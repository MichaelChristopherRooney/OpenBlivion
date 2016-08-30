#include <src\engine.h>
#include <src\resource\nif.h>

int main(int argc, char** argv) {

	nif n;
	n.load("C:\\Users\\Michael\\Desktop\\meshes\\armor\\fur\\shield.nif");
	//engine game;

	//if (!game.run()) {
	//	printf("Press any key to exit...\n");
	//	getchar();
	//}

	printf("Press enter to exit\n");
	getchar();

}

