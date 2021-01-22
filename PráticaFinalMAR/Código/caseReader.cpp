/*
Álvaro Corrochano López

Main del programa.

Lee un caso de prueba de un .txt y lo ejecuta.

*/

#include "BTree.h"

#include<iostream>
#include<fstream>

using namespace std;


int main() {

	BTree<int> tree = BTree<int>(3);

	ifstream fe("prueba.txt");
	char action;
	int n;

	while (!fe.eof()) {
		fe >> action;
		fe >> n;
		switch (action) {

		case 'i': // Insert case
			// cout << "Insertamos " << n << '\n';
			tree.insert(n);
			break;

		case 'd': // delete case
			// cout << "Eliminamos " << n << '\n';
			tree.remove(n);
			break;

		case 's': // search case
			// cout << "Buscamos " << n << '\n';
			tree.search(n);
			break;
		}

	}

	tree.traverse();

	fe.close();
	return 0;
}