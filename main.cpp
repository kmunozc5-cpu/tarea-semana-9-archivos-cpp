#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

struct Producto {
    std::string codigo;
    std::string nombre;
    double precio;
    int existencia;
};

std::string limpiarEspacios(const std::string& texto) {
    size_t inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fin = texto.find_last_not_of(" \t\r\n");
    return texto.substr(inicio, fin - inicio + 1);
}

int main() {

    const std::string rutaEntrada = "datos/productos.csv";
    const std::string rutaSalida = "reportes/resumen.txt";

    std::ifstream archivo(rutaEntrada);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo fuente en " << rutaEntrada << std::endl;
        return 1;
    }

    std::vector<Producto> inventario;
    int registrosValidos = 0;
    int registrosInvalidos = 0;
    double totalInventario = 0.0;
    std::string linea;

    if (!std::getline(archivo, linea)) {
        std::cerr << "Error: El archivo se encuentra vacio." << std::endl;
        archivo.close();
        return 1;
    }

    // 3. Procesar registro por registro
    while (std::getline(archivo, linea)) {
        if (limpiarEspacios(linea).empty()) {
            continue; // Saltar líneas en blanco accidentales
        }

        std::stringstream ss(linea);
        std::string codigo, nombre, precioTexto, existenciaTexto;

        std::getline(ss, codigo, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, precioTexto, ',');
        std::getline(ss, existenciaTexto, ',');

        codigo = limpiarEspacios(codigo);
        nombre = limpiarEspacios(nombre);
        precioTexto = limpiarEspacios(precioTexto);
        existenciaTexto = limpiarEspacios(existenciaTexto);

        if (codigo.empty() || nombre.empty() || precioTexto.empty() || existenciaTexto.empty()) {
            registrosInvalidos++;
            continue;
        }

        try {
            size_t idxPrecio, idxExistencia;
            double precio = std::stod(precioTexto, &idxPrecio);
            int existencia = std::stoi(existenciaTexto, &idxExistencia);

            if (idxPrecio != precioTexto.length() || idxExistencia != existenciaTexto.length()) {
                registrosInvalidos++;
                continue;
            }

            // Reglas de negocio: precio > 0 y existencia >= 0
            if (precio > 0.0 && existencia >= 0) {
                registrosValidos++;
                totalInventario += (precio * existencia);
                inventario.push_back({codigo, nombre, precio, existencia});
            } else {
                registrosInvalidos++;
            }
        } catch (...) {
            // Captura errores de conversión (stod / stoi) sin detener el programa
            registrosInvalidos++;
        }
    }
    archivo.close(); 

   
    std::cout << "========================================" << std::endl;
    std::cout << "        CONTROL DE INVENTARIO           " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Registros validos   : " << registrosValidos << std::endl;
    std::cout << "Registros invalidos : " << registrosInvalidos << std::endl;
    std::cout << "Valor total invent. : Q" << std::fixed << std::setprecision(2) << totalInventario << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    std::string codigoBuscado;
    std::cout << "Ingrese el codigo de producto a consultar: ";
    std::cin >> codigoBuscado;

    bool encontrado = false;
    std::string resultadoBusqueda;

    for (const auto& prod : inventario) {
        if (prod.codigo == codigoBuscado) {
            encontrado = true;
            resultadoBusqueda = "Producto encontrado: " + prod.nombre + 
                                " | Precio: Q" + std::to_string(prod.precio) + 
                                " | Existencia: " + std::to_string(prod.existencia);
            break;
        }
    }

    if (!encontrado) {
        resultadoBusqueda = "El producto con codigo '" + codigoBuscado + "' no existe en el inventario valido.";
    }

    std::cout << resultadoBusqueda << std::endl;

    // 8. Generar reporte consolidado en reportes/resumen.txt
    std::ofstream reporte(rutaSalida);
    if (!reporte.is_open()) {
        std::cerr << "Error: No se pudo crear el archivo de reporte en " << rutaSalida << std::endl;
        return 1;
    }

    reporte << "========================================" << "\n";
    reporte << "        REPORTE DE INVENTARIO           " << "\n";
    reporte << "========================================" << "\n";
    reporte << "Registros validos procesados   : " << registrosValidos << "\n";
    reporte << "Registros invalidos descartados: " << registrosInvalidos << "\n";
    reporte << "Valor total del inventario     : Q" << std::fixed << std::setprecision(2) << totalInventario << "\n";
    reporte << "----------------------------------------" << "\n";
    reporte << "Resultado de busqueda:" << "\n";
    reporte << resultadoBusqueda << "\n";
    reporte << "========================================" << "\n";

    reporte.close(); // Cerrar archivo de salida
    std::cout << "\nReporte guardado con exito en " << rutaSalida << std::endl;

    return 0;
}
