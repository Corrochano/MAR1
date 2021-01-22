/*
Álvaro Corrochano López

Esto es una prueba para comprobar que todo funciona correctamente.

*/

#include "BTree.h"

#include<iostream>

using namespace std;


int main() {
	BTree<int> tree = BTree<int>();
	
	if (tree.isEmpty()) cout << "El arbol esta vacio\n";
	else cout << "El arbol no esta vacio\n";

	tree.insert(1);
	tree.insert(3);
	tree.insert(2);
	tree.insert(6);
	tree.insert(4);
	tree.insert(7);
	tree.insert(5);

	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';
	int b = 1;
	if (tree.search(b) != NULL) cout << "El elemento " << b << " esta en el arbol.\n";
	else cout << "El elemento " << b << " no esta en el arbol.\n";

	b = 0;
	if (tree.search(b) != NULL) cout << "El elemento " << b << " esta en el arbol.\n";
	else cout << "El elemento " << b << " no esta en el arbol.\n";

	if (tree.isEmpty()) cout << "El arbol esta vacio\n";
	else cout << "El arbol no esta vacio\n";

	int n = tree.n_keys();
	cout << "El numero maximo de keys es: " << n << '\n';

	int* r = tree.root();
	int e_r = tree.n_root();

	cout << "En el nodo raiz hay " << e_r << " keys, que son: ";

	for (int i = 0; i < e_r; i++) cout << r[i] << ' ';
	cout << '\n';

	tree.remove(6);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(4);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(2);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(5);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(1);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(3);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(7);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.insert(1);
	tree.insert(3);
	tree.insert(2);
	tree.insert(6);
	tree.insert(4);
	tree.insert(7);
	tree.insert(5);

	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(8);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.insert(8);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';

	tree.remove(8);
	cout << "Recorrido del arbol:";
	tree.traverse();
	cout << '\n';
	
	return 0;
}