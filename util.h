#include <map>
#include <vector>
#include <string>

using namespace std;


// Esta estructura guarda el valor en la estructura K-V
typedef struct {
	size_t size;
	string data;
} Value;

// Definicion de la estructura K-V utilizada
// Clave es numerica y valor es dado por estructura Value.
typedef map<unsigned long, Value> KVStore;
