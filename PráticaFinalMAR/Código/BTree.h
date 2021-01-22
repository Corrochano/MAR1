/*
- Representación de un Árbol-B
- Álvaro Corrochano López
*/

#ifndef __BTREE_H
#define __BTREE_H

#include <cstddef>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <string>
using namespace std;

/** Mínimo de claves que puede almacenar un nodo en un Árbol-B */
const int MIN_SIZE = 2;

/** Máximo de claves que puede almacenar un nodo en un Árbol-B */
const int MAX_SIZE = 1000;

/** Tamaño por defecto de un árbol si este no se especifica */
const int DEFAULT_SIZE = 3;

/** Excepción, B-Árbol vacío  */
class E_BTree_Empty{};

/** Excepción, B-Árbol con menos de 2 claves por nodo */
class E_BTree_Lower{};

/** Excepción, B-Árbol con más de 1000 claves por nodo */
class E_BTree_Bigger{};

/**
  Clase para representar a un nodo del árbol, guarda la siguiente información:
  - Número máximo de keys que puede almacenar el nodo.
  - Número de keys (valores) actualmente almacenados en el nodo en orden creciente.
  - Booleano que indica si el nodo es una hoja.
  - Punteros a sus hijos.
  */
template <class T>
class Node {

public:

    /** Constructor que construye un nodo con el tamaño máximo determinado de keys como máximo

    @param max_elems número máximo de keys que puede almacenar el nodo
    */
    Node(int max_elems) : _elems(), _child(), _n_elems(0), _max_elems(max_elems), _is_leaf(true) {
        _elems = new T[max_elems];
        _child = new Node *[max_elems + 1];
    }

    /** Constructor que construye un nodo con el tamaño máximo determinado de keys como máximo

    @param max_elems número máximo de keys que puede almacenar el nodo
    @param is_leaf indica si el nodo es o no hoja
    */
    Node(int max_elems, bool is_leaf) : _elems(), _child(), _n_elems(0), _max_elems(max_elems), _is_leaf(is_leaf) {
        _elems = new T[max_elems];
        _child = new Node *[max_elems + 1];
    }

    /**
    Función que elimina la key k del subárbol con este nodo como raíz

    @param k key a eliminar
    */
    void remove(T k) {
        int i = 0; 
        while (i < _n_elems && _elems[i] < k) i++; // Busco la posición de la primera key mayor o igual que k

        int t = (_max_elems + 1) / 2; // Mitad del máximo de hijos

        if (i < _n_elems && _elems[i] == k) { // Si la clave a borrar está en este nodo
            if (_is_leaf) removeFromLeaf(i); // Si soy nodo hoja, llamo a eliminar en hoja
            else removeFromNonLeaf(i); // Si no soy hoja, llamo a eliminar en no hoja
        }
        else { // Si no está en este nodo

            if (_is_leaf) { // Si el nodo es hoja, la key no está en el árbol
                cout << "The key  " << k << " is not in the tree so we can't remove it.\n";
                return;
            }

            bool is_in_last = ((i == _n_elems) ? true : false); // bool para indicar si k está en el subárbol con el último hijo del nodo como raíz

            if (_child[i]->_n_elems < t) { // Si el árbol donde se supone que está el último hijo tiene menos de t keys
                fill(i);
            }

            if (is_in_last && i > _n_elems) _child[i - 1]->remove(k); // Si el último hijo ha hecho merge lo ha hecho con el anterior, así que debemos eliminar ahí
            else _child[i]->remove(k);
        }
    }

    /**
    Función que elimina la key en la posición i del nodo (que es hoja)

    @param i posición donde está la key a eliminar
    */
    void removeFromLeaf(int i) {

        for (int j = i + 1; j < _n_elems; j++) { // movemos las keys posteriores a la posición i
            _elems[j - 1] = _elems[j];
        }

        _n_elems--; // Reducimos el contador de keys
    }

    /**
    Función que elimina la key en la posición i del nodo (que no es hoja).

    @param i posición donde está la key a eliminar
    */
    void removeFromNonLeaf(int i) {
        T k = _elems[i];
        int t = (_max_elems + 1) / 2; // // Mitad del máximo de hijos

        if (_child[i]->_n_elems >= t) { // si el hijo que precede a k tiene por lo menos t elementos
            T pred = getPred(i); // buscamos el predecesor de k en ese árbol
            _elems[i] = pred; // intercambiamos k con el predecesor
            _child[i]->remove(pred); // eliminamos k en el hijo
        }

        else if (_child[i + 1]->_n_elems >= t) { // si el predecesor no los tiene, comprobamos que el sucesor tenga al menos t keys
            T suc = getSucc(i); // buscamos al sucesor de k en ese árbol
            _elems[i] = suc; // intercambiamos k con el sucesor
            _child[i + 1]->remove(suc); // eliminamos k en el hijo
        }

        else { // si ninguno de los dos tiene al menos t keys
            merge(i); // hacemos una unión del hijo que precede a k y del que lo sucede 
            _child[i]->remove(k); // eliminamos k del hijo i (que contiene la unión del hijo predecesor  y del sucesor de k, además de k)
        }
    }

    /**
    Función para encontrar al elemento sucesor del elemento en la posición i

    @param i posición donde está el hijo sucesor

    @return el sucesor de la key en la posición i
    */
    int getSucc(int i) {
        Node* c = _child[i + 1];
        while (!c->_is_leaf) c = c->_child[0]; // Mientras no sea hoja, cogemos el que está más a la izquierda
        return c->_elems[0]; // Devuelvo la primera key de la hoja más a la izquierda
    }

    /**
    Función para encontrar al elemento predecesor del elemento en la posición i

    @param i posición donde está el hijo predecesor

    @return el predecesor de la key en la posición i
*/
    T getPred(int i) {
        Node* c = _child[i];
        while (!c->_is_leaf) c = c->_child[c->_n_elems]; // Mientras que c no sea hoja, cogemos el que esté más a la derecha
        return c->_elems[c->_n_elems - 1]; // Devuelvo la útima key de la hoja más a la derecha
    }

    /**
    Función para quitar una key del hijo en la posición i + 1 y añadir una key al hijo en la posición i

    @param i índice donde está el hijo que obtiene la clave.
    */
    void borrowFromNext(int i) {

        Node* child = _child[i];
        Node* sibling = _child[i + 1]; // Hijo siguiente al hijo al que vamos a dar la keyS

        child->_elems[(child->_n_elems)] = _elems[i]; // Insertamos la key en i del padre como última key del hijo

        if (!(child->_is_leaf)) child->_child[(child->_n_elems) + 1] = sibling->_child[0]; // Si el nodo no es hoja, el primer hijo de su hermano es su último hijo

        _elems[i] = sibling->_elems[0]; // La primer key del hermano es la key en la posición i del padre

        for (int j = 1; j < sibling->_n_elems; j++) sibling->_elems[j - 1] = sibling->_elems[j];  // Movemos todas las keys del hermano

        if (!sibling->_is_leaf) { // Si el hermano no es hoja
            for (int j = 1; j <= sibling->_n_elems; j++) sibling->_child[j - 1] = sibling->_child[j]; // Movemos sus hijos
        }

        child->_n_elems += 1; // Aumentamos el número de keys del hijo
        sibling->_n_elems -= 1; // Disminuimos el número de keys del hermano
    }

    /**
    Función para quitar una key del hijo en la posición i - 1 y añadir una key al hijo en la posición i

    @param i índice donde está el hijo que obtiene la clave.
    */
    void borrowFromPrev(int i) {

        Node* child = _child[i];
        Node* sibling = _child[i - 1]; // hermano anterior al hijo que queremos que consiga una key

        for (int j = child->_n_elems - 1; j >= 0; j--) { // movemos todas las keys  del hijo en la posición i
            child->_elems[j + 1] = child->_elems[j];
        }

        if (!child->_is_leaf) { // Si este hijo no es hoja, movemos todos sus hijos
            for (int j = child->_n_elems; j >= 0; j--) child->_child[j + 1] = child->_child[j];
        }

        child->_elems[0] = _elems[i - 1]; // La primer key del hijo pasa a ser la key en la posición i - 1 del padre

        if (!child->_is_leaf) child->_child[0] = sibling->_child[sibling->_n_elems]; // Si el hijo no es hoja, su primer hijo pasa a ser el último de su hermano

        _elems[i - 1] = sibling->_elems[sibling->_n_elems - 1]; // Movemos la key del hermano al padre

        child->_n_elems += 1; // aumentamos el número de keys del hijo
        sibling->_n_elems -= 1; // disminuimos el número de keys del hermano
    }
 
    /**
    Función para rellenar el hijo en la posición i dado que tiene menos de t keys.

    @param i posición del hijo que queremos rellenar
    */
    void fill(int i) {      

        int t = (_max_elems + 1) / 2; // Mitad del máximo de hijo

        if (i != 0 && _child[i - 1]->_n_elems >= t) borrowFromPrev(i); // si tiene hermano predecesor, si este tiene al menos t keys, coge una key de ese hijo

        else if (i != _n_elems && _child[i + 1]->_n_elems  >= t) borrowFromNext(i); // si tiene hermano sucesor, si este tiene al menos t keys, coge una key de ese hijo

        else { // si ninguno tiene al menos t keys
            if (i != _n_elems) merge(i); // si no es el último hijo, hace merge con su sucesor
            else merge(i - 1); // si es el último hace merge con su predecesor
        }
    }

    /**
    Función para unir dos hijos (el i y el i+1) en el hijo i.
    El hijo i + 1 es liberado.
    
    @param i posición del hijo que va a acoger la unión (y el primero que se va a unir, con el i+1)
    */
    void merge(int i) {
        int t = (_max_elems + 1) / 2; // Mitad del máximo de hijo
        Node* child = _child[i];
        Node* sibling = _child[i + 1];

        child->_elems[t - 1] = _elems[i]; // Ponemos la key i del padre en la posición t-1 del hijo
 
        for (int j = 0; j < sibling->_n_elems; j++) { // Copiamos las keys del hermano en el hijo
            child->_elems[j + t] = sibling->_elems[j];
        }

        if (!child->_is_leaf) { // Copiamos los hijos del hermano en el hijo
            for (int j = 0; j <= sibling->_n_elems; j++) {
                child->_child[j + t] = sibling->_child[j];
            }
        }

        for (int j = i + 1; j < _n_elems; j++) { // Movemos las keys del padre posteriores a i
            _elems[j - 1] = _elems[j];
        }

        for (int j = i + 2; j <= _n_elems; j++) { // Movemos los hijos posteriores a la posición i+1 en el padre
            _child[j - 1] = _child[j];
        }

        child->_n_elems += sibling->_n_elems + 1; // Actualizamos el número de keys en el hijo
        _n_elems--; // Actualizamos el número de keys en el padre

        delete(sibling); // Liberamos al hermano
    }

    /**  Atributos  */
    T* _elems;       // valores del nodo
    Node** _child;  // punteros a sus hijos
    int _n_elems;   // número de keys que tiene el nodo actualmente
    int _max_elems; // número máximo de keys que puede tener el nodo
    bool _is_leaf;  // booleano que indica si el nodo es hoja o no
};

/**

Clase que representa a un Árbol-B

@author Álvaro Corrochano López

*/
template <class T>
class BTree {

public:

    /** Constructor vacío, tamaño de las keys con tamaño por defecto (= 3) 
        Complejidad: O(1)
    */
    BTree() {
        _root = new Node<T>(DEFAULT_SIZE);
        _size = DEFAULT_SIZE;
    };

    /**
    Constructor vacío especificando el tamaño

    Error: Si el tamaño especificado es menor que 2, lanza una excepción E_BTree_Lower
    Error: Si el tamaño especificado es mayor que 1000, lanza una excepción E_BTree_Bigger
 
    @param size número máximo de keys que puede almacenar el nodo
    */
    BTree(int size) {
        if (size < MIN_SIZE) throw E_BTree_Lower();
        if (size > MAX_SIZE) throw E_BTree_Bigger();

        _root = new Node<T>(size);
        _size = size;
    };

    /**
    Constructor que crea un árbol dado un puntero a un nodo y el número máximo de keys que tendrá cada nodo.

    @param n nodo que será la raíz del árbol
    @param s número máximo de keys que tendrá cada nodo
    */
    BTree(Node<T>* n, int s) {
        _root = n;
        _size = s;
    }

    /** Devuelve el número de keys alojadas en el nodo raíz del árbol

    @return Número de keys alojadas en el nodo raíz del árbol
    */
    int n_root() const {
        return _root->_n_elems;
    }

    /** Devuelve las keys alojadas  en el nodo raíz del árbol

    Error: Si el árbol está vacío, lanza una excepción E_BTree_Empty

    @return El valor en la raíz
    */
    T* root() const {
        if (isEmpty()) {
            throw E_BTree_Empty();
        }
        return _root->_elems;
    }


    /** Devuelve el número máximo de keys que puede almacenar un nodo del árbol

    @return número máximo de keys que puede almacenar un nodo del árbol
    */
    int n_keys() {
        return _size;
    }

    /** Indica si el árbol está o no vacío

    @return true si el árbol está vacío y false si no lo está
    */
    bool isEmpty() const {
        return _root->_n_elems == 0;
    }

   /**
   Función para recorrer un árbol, va sacando las keys guardadas en orden (siempre creciente).
   */
    void traverse()
    {
        int i;
        for (i = 0; i < _root->_n_elems; i++) {
            if (!_root->_is_leaf) { // Si no es hoja, recorro los hijos
                BTree(_root->_child[i], _size).traverse();
            }
            cout << " " << _root->_elems[i];
        }

        if (!_root->_is_leaf) { // Si no es hoja, recorro su último hijo (no se hace en el bucle).
            BTree(_root->_child[i], _size).traverse();
        }

    }


    /** Busca el elemento pasado por parámetro en el árbol.

      Error: Si el árbol está vacío, lanza una excepción E_BTree_Empty
    
      @param k elemento a buscar en el árbol.
    
      @return retorna el nodo donde se encuentra la clave a NULL en caso de no encontrarla.
    */
    Node<T>* search(int k) {
        int i = 0;

        if(isEmpty()) throw E_BTree_Empty(); // Si el árbol está vacío, error
       
        while (i < _root->_n_elems && k > _root->_elems[i]) { // Iteramos para encontrar el primer índice i cuya clava cumpla key <= k 
             i++;
        }

        if (i < _root->_n_elems && k == _root->_elems[i]) { // Si he encontrado el elemento
            return _root;    
        }

        else if (_root->_is_leaf) { // Si el nodo es hoja     
            return NULL;
        }

        else { //  En otro caso, miro el hijo del index conseguido anteriormente
            return BTree(_root->_child[i], _size).search(k);
        }

    }

    /**
        Función para insertar un elemento en un árbol.
        Hace uso de splitChild en caso de estar lleno el nodo (y de insert_nonfull después de crear el nuevo nodo) y de insert_nofull en 
        caso de no estarlo.
        
        @param k elemento a insertar en el árbol.
    */
    void insert(T k) {

        if (_root->_n_elems == _root->_max_elems) { // Si el nodo está lleno
            Node<T>* r = _root; // Me guardo la raíz actual

            Node<T>* s = new Node<T>(_size, false); // Creo un nuevo nodo
            s->_n_elems = 0; // Le asigno que tiene 0 keys
            s->_child[0] = r; // La antigua raíz es su hijo

            splitChild(0, s); // Parto la raíz y añado 1 de sus elementos a la nueva raíz
            BTree(s, _size).insert_nonfull(k); // Añado el elemento en s
            _root = s; // s es el nuevo nodo raíz
        }
        else { // Si no lo está, inserción no completo
            insert_nonfull(k);
        }

    }


    /**
    Función para eliminar el elemento k del árbol

    Error: Si el árbol está vacío, lanza una excepción E_BTree_Empty

    @param k elemento a eliminar
    */
    void remove (int k) {
        if (isEmpty()) throw E_BTree_Empty(); // Si el árbol está vacío, error

        _root->remove(k); // Llamamos a la función remove de la raíz

        if (_root->_n_elems == 0) { // si la raíz se ha quedado sin keys
            Node<T>* old_root = _root;
            if (!_root->_is_leaf) _root = _root->_child[0]; // si no es hoja, su primer hijo es la nueva raíz
            else _root = new Node<T>(_size);

            delete old_root; // liberamos la raíz antigua
        }
    }

private:

    /**
    Función usada cuando se quiere añadir k a un nodo que está lleno, este se parte en dos y tanto la partición como el nodo quedan colgando 
    del padre (que es el parámetro x), manteniendo el orden dentro de los árboles.

    @param i posición que ocupa el nodo que queremos partir dentro de los hijos del padre
    @param x nodo padre del que queremos partir
    */
    void splitChild(int i, Node<T>* x) {
        Node<T>* y = x->_child[i]; // y es el hijo i de x
        Node<T>* z = new Node<T>(y->_max_elems, y->_is_leaf); // Creación de un nuevo nodo donde van a guardar la mitad de las keys de y
        int t = (x->_max_elems + 1) / 2; // Mitad del total de hijos que tiene y (se supone que está lleno)
        z->_n_elems = t - 1; // z tiene la mitad de keys de y

        for (int j = 0; j < t - 1; j++) { // Metemos la mitad de los elementos de y en z (los más grandes)
            z->_elems[j] = y->_elems[j + t];
        }

        if (!y->_is_leaf) { // Si y no es hoja
            for (int j = 0; j < t; j++) { // Pasamos la mitad de sus hijos a z
                z->_child[j] = y->_child[j + t];
            }
        }
        y->_n_elems = t - 1; // y pasa a tener la mitad de elemntos

        for (int j = x->_n_elems + 1; j > i; j--) { // Movemos los hijos de x
            x->_child[j + 1] = x->_child[j];
        }
        x->_child[i + 1] = z; // z es hijo de x

        for (int j = x->_n_elems - 1; j >= i; j--) { // movemos las keys de x
            x->_elems[j + 1] = x->_elems[j];
        }

        x->_elems[i] = y->_elems[t - 1]; // Añadimos la key de en medio de y en x para que separe y de z
        x->_n_elems += 1; // Aumentamos el número de elementos de x

    }

    /**
    Función usada cuando se quiere añadir k a un nodo que no está completamente lleno. 
    Se busca su posición y se inserta, insertándose diréctamente en caso de ser hoja y en su hijo en caso de serlo.
    
    @param x nodo donde se desea realizar la inserción
    @param k elemento a insertar en el nodo
    */
    void insert_nonfull(T k) {
        int i = _root->_n_elems - 1; // Elementos que tiene el nodo actualmente
        if (_root->_is_leaf) { // Si el nodo es hoja
            while (i >= 0 && k < _root->_elems[i]) { // buscamos la nueva localización de key a ser insertada y desplazamos las mayores
                _root->_elems[i + 1] = _root->_elems[i];
                i--;
            }
            _root->_elems[i + 1] = k; // insertamos la key
            _root->_n_elems += 1; // Aumentamos el número de keys que tiene el nodo
        }
        else { // Si el nodo no es una hoja
            while (i >= 0 && k < _root->_elems[i]) { // Buscamos al hijo que tendrá a k
                i--;
            }
            i += 1;

            if (_root->_child[i]->_n_elems == _size) { // Comprobamos si está lleno
                splitChild(i, _root); // como está lleno, le hacemos split
                
                if (_root->_elems[i] < k) { // Al hacer el split, la key del medio del hijo sube y este se parte en dos,
                    i++;                //por lo que comprobamos en cual de las dos partes irá k
                }
            }
            BTree(_root->_child[i], _size).insert_nonfull(k);
        }
    }

    /** Atributos */
    Node<T> *_root; // Puntero que apunta al nodo raíz
    int _size; // Máximo de keys en cada nodo
};

#endif