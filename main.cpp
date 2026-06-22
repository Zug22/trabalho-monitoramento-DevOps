#include "crow_all.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <atomic>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

// --- MÉTRICAS DE CPU (Mantidas) ---
unsigned long long ultTotalUsr, ultTotalUsrBaixo, ultTotalSis, ultTotalOcioso;

void iniciar_uso_cpu() {
    ifstream arquivo("/proc/stat");
    string linha, rotuloCpu;
    getline(arquivo, linha);
    istringstream fluxo(linha);
    fluxo >> rotuloCpu >> ultTotalUsr >> ultTotalUsrBaixo >> ultTotalSis >> ultTotalOcioso;
}

double obter_uso_cpu() {
    ifstream arquivo("/proc/stat");
    string linha, rotuloCpu;
    unsigned long long totalUsr, totalUsrBaixo, totalSis, totalOcioso;
    getline(arquivo, linha);
    istringstream fluxo(linha);
    fluxo >> rotuloCpu >> totalUsr >> totalUsrBaixo >> totalSis >> totalOcioso;

    unsigned long long difUsr = totalUsr - ultTotalUsr;
    unsigned long long difUsrBaixo = totalUsrBaixo - ultTotalUsrBaixo;
    unsigned long long difSis = totalSis - ultTotalSis;
    unsigned long long difOcioso = totalOcioso - ultTotalOcioso;

    unsigned long long difTotal = difUsr + difUsrBaixo + difSis + difOcioso;
    double percentual = 0.0;
    
    if (difTotal != 0) {
        percentual = double(difTotal - difOcioso) / double(difTotal) * 100.0;
    }

    ultTotalUsr = totalUsr;
    ultTotalUsrBaixo = totalUsrBaixo;
    ultTotalSis = totalSis;
    ultTotalOcioso = totalOcioso;
    return percentual;
}

// --- NOVAS MÉTRICAS DO WEB SERVER ---
// Variáveis atômicas garantem segurança em servidores multithread no Linux
atomic<int> total_requisicoes(0);
atomic<int> total_erros(0);
atomic<int> conexoes_ativas(0);
atomic<double> ultima_latencia_ms(0.0);

int main() {
    iniciar_uso_cpu();
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    // Rota que simula o serviço real da sua aplicação recebendo acessos
    CROW_ROUTE(app, "/api/trabalho")
    ([](){
        conexoes_ativas++; // Incrementa as conexões ativas assim que o cliente bate aqui
        total_requisicoes++;
        
        // Inicia o cronômetro de latência
        auto inicio = high_resolution_clock::now();

        // Simula o tempo de processamento do servidor (delay de 20ms)
        this_thread::sleep_for(milliseconds(20));

        // Simula uma taxa de erro (1 erro 500 a cada 10 requisições)
        bool erro_simulado = (total_requisicoes % 10 == 0);

        // Para o cronômetro e calcula a latência em milissegundos
        auto fim = high_resolution_clock::now();
        ultima_latencia_ms = duration<double, milli>(fim - inicio).count();

        conexoes_ativas--; // A requisição terminou, libera a conexão

        if (erro_simulado) {
            total_erros++;
            return crow::response(500, "Erro interno simulado");
        }

        return crow::response(200, "Processamento OK");
    });

    // Rota que o Prometheus consome
    CROW_ROUTE(app, "/metrics")
    ([](){
        double cpu = obter_uso_cpu();
        string metrica = "";

        metrica += "# HELP percentual_uso_cpu Uso de CPU\n";
        metrica += "# TYPE percentual_uso_cpu gauge\n";
        metrica += "percentual_uso_cpu " + to_string(cpu) + "\n\n";

        // 1. Disponibilidade
        metrica += "# HELP servidor_web_up Status do Web Server (1=UP, 0=DOWN)\n";
        metrica += "# TYPE servidor_web_up gauge\n";
        metrica += "servidor_web_up 1\n\n";

        // 2. RPS (Prometheus calcula a taxa baseado no total acumulado)
        metrica += "# HELP total_requisicoes_http Total acumulado de acessos\n";
        metrica += "# TYPE total_requisicoes_http counter\n";
        metrica += "total_requisicoes_http " + to_string(total_requisicoes) + "\n\n";

        // 3. Taxa de Erros
        metrica += "# HELP total_erros_http Total acumulado de erros 4xx/5xx\n";
        metrica += "# TYPE total_erros_http counter\n";
        metrica += "total_erros_http " + to_string(total_erros) + "\n\n";

        // 4. Latência
        metrica += "# HELP ultima_latencia_ms Tempo da última resposta em milissegundos\n";
        metrica += "# TYPE ultima_latencia_ms gauge\n";
        metrica += "ultima_latencia_ms " + to_string(ultima_latencia_ms) + "\n\n";

        // 5. Conexões Ativas
        metrica += "# HELP conexoes_ativas Quantidade de clientes processando agora\n";
        metrica += "# TYPE conexoes_ativas gauge\n";
        metrica += "conexoes_ativas " + to_string(conexoes_ativas) + "\n";

        crow::response resposta(metrica);
        resposta.add_header("Content-Type", "text/plain; version=0.0.4");
        return resposta;
    });

    cout << "Backend operando na porta 8080..." << endl;
    app.port(8080).multithreaded().run();

    return 0;
}
