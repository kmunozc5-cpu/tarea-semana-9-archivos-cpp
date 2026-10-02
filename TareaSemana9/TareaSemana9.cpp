// TareaSemana9.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//
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

// Función para limpiar espacios, retornos de carro (\r de Windows) y saltos de línea
std::string limpiarEspacios(const std::string& str) {
    size_t inicio = str.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fin = str.find_last_not_of(" \t\r\n");
    return str.substr(inicio, fin - inicio + 1);
}

int main() {
    std::string rutaEntrada = "datos/productos.csv";
    std::string rutaSalida = "reportes/resumen.txt";

    // 1. Abrir archivo de datos con ifstream
    std::ifstream archivo(rutaEntrada);

    // Búsqueda en rutas relativas por si Visual Studio ejecuta desde una subcarpeta
    if (!archivo.is_open()) {
        rutaEntrada = "../datos/productos.csv";
        rutaSalida = "../reportes/resumen.txt";
        archivo.open(rutaEntrada);
    }
    if (!archivo.is_open()) {
        rutaEntrada = "../../datos/productos.csv";
        rutaSalida = "../../reportes/resumen.txt";
        archivo.open(rutaEntrada);
    }

    // Comprobar apertura correcta
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo fuente datos/productos.csv" << std::endl;
        return 1;
    }

    std::vector<Producto> inventario;
    int registrosValidos = 0;
    int registrosInvalidos = 0;
    double totalInventario = 0.0;
    std::string linea;

    // 2. Omitir el encabezado del CSV
    if (!std::getline(archivo, linea)) {
        std::cerr << "Error: El archivo CSV se encuentra vacio." << std::endl;
        archivo.close();
        return 1;
    }

    // 3. Procesar fila por fila con stringstream
    while (std::getline(archivo, linea)) {
        if (limpiarEspacios(linea).empty()) continue;

        std::stringstream ss(linea);
        std::string codigo, nombre, precioTxt, existTxt;

        std::getline(ss, codigo, ',');
        std::getline(ss, nombre, ',');
        std::getline(ss, precioTxt, ',');
        std::getline(ss, existTxt, ',');

        codigo = limpiarEspacios(codigo);
        nombre = limpiarEspacios(nombre);
        precioTxt = limpiarEspacios(precioTxt);
        existTxt = limpiarEspacios(existTxt);

        // 4. Validar campos obligatorios no vacíos
        if (codigo.empty() || nombre.empty() || precioTxt.empty() || existTxt.empty()) {
            registrosInvalidos++;
            continue;
        }

        // Conversión protegida de tipos numéricos
        try {
            size_t idxP, idxE;
            double precio = std::stod(precioTxt, &idxP);
            int existencia = std::stoi(existTxt, &idxE);

            // Validar que toda la cadena haya sido numérica
            if (idxP != precioTxt.length() || idxE != existTxt.length()) {
                registrosInvalidos++;
                continue;
            }

            // Reglas de negocio: precio > 0 y existencia >= 0
            if (precio > 0.0 && existencia >= 0) {
                registrosValidos++;
                totalInventario += (precio * existencia);
                inventario.push_back({ codigo, nombre, precio, existencia });
            }
            else {
                registrosInvalidos++;
            }
        }
        catch (...) {
            // Captura errores de conversión (stod / stoi) sin detener la ejecución
            registrosInvalidos++;
        }
    }
    archivo.close();

    // 5 y 6. Mostrar resultados generales en consola
    std::cout << "========================================" << std::endl;
    std::cout << "        CONTROL DE INVENTARIO           " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Registros validos   : " << registrosValidos << std::endl;
    std::cout << "Registros invalidos : " << registrosInvalidos << std::endl;
    std::cout << "Valor total invent. : Q" << std::fixed << std::setprecision(2) << totalInventario << std::endl;
    std::cout << "----------------------------------------\n" << std::endl;

    // 7. Búsqueda continua utilizando un ciclo while
    std::string buscado;
    std::vector<std::string> historialBusquedas;

    while (true) {
        std::cout << "Ingrese codigo a buscar (o 'salir' para terminar): ";
        std::cin >> buscado;

        if (buscado == "salir" || buscado == "SALIR" || buscado == "Salir") {
            break;
        }

        bool encontrado = false;
        std::string resultado;

        for (const auto& prod : inventario) {
            if (prod.codigo == buscado) {
                encontrado = true;
                resultado = "Codigo [" + prod.codigo + "] -> " + prod.nombre +
                    " | Precio: Q" + std::to_string(prod.precio) +
                    " | Existencia: " + std::to_string(prod.existencia);
                break;
            }
        }

        if (!encontrado) {
            resultado = "Codigo [" + buscado + "] -> No existe en el inventario valido.";
        }

        std::cout << resultado << "\n" << std::endl;
        historialBusquedas.push_back(resultado);
    }

    // 8. Generar el reporte en reportes/resumen.txt
    std::ofstream reporte(rutaSalida);
    if (!reporte.is_open()) {
        std::cerr << "Error: No se pudo generar el archivo de reporte en " << rutaSalida << std::endl;
        return 1;
    }

    reporte << "========================================" << "\n";
    reporte << "        REPORTE DE INVENTARIO           " << "\n";
    reporte << "========================================" << "\n";
    reporte << "Registros validos procesados   : " << registrosValidos << "\n";
    reporte << "Registros invalidos descartados: " << registrosInvalidos << "\n";
    reporte << "Valor total del inventario     : Q" << std::fixed << std::setprecision(2) << totalInventario << "\n";
    reporte << "----------------------------------------" << "\n";
    reporte << "Resultado(s) de busqueda:" << "\n";

    if (historialBusquedas.empty()) {
        reporte << "No se consultaron codigos durante la sesion.\n";
    }
    else {
        for (const auto& item : historialBusquedas) {
            reporte << item << "\n";
        }
    }
    reporte << "========================================" << "\n";

    reporte.close();
    std::cout << "\nReporte guardado con exito en " << rutaSalida << std::endl;

    // Pausa final
    std::cout << "\nPresione Enter para salir...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}
