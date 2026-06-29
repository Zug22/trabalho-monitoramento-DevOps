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
atomic<int> total_falhas_login(0);
atomic<int> vulnerabilidades_conhecidas(0);
atomic<int> alteracoes_configuracao(0);
atomic<double> ultima_latencia_ms(0.0);

unsigned long long calcular_checksum_arquivo(const string& caminho) {
    ifstream arquivo(caminho, ios::binary);

    if (!arquivo.is_open()) {
        return 0;
    }

    unsigned long long hash = 1469598103934665603ULL;
    const unsigned long long primo = 1099511628211ULL;
    char caractere;

    while (arquivo.get(caractere)) {
        hash ^= static_cast<unsigned char>(caractere);
        hash *= primo;
    }

    return hash % 1000000000000ULL;
}

int main() {
    iniciar_uso_cpu();
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/")
    ([](){
        const string html = R"HTML(
<!doctype html>
<html lang="pt-BR">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Monitoramento DevOps</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Archivo+Black&family=Space+Grotesk:wght@400;500;700&display=swap');

        :root {
            color-scheme: dark;
            --bg: #080a0f;
            --panel: rgba(18, 22, 31, .82);
            --panel-solid: #11151f;
            --text: #f4f1e8;
            --muted: #9aa3b7;
            --line: rgba(244, 241, 232, .13);
            --ok: #b8ff4d;
            --warn: #ffbd4a;
            --danger: #ff4d5e;
            --accent: #d7ff57;
            --accent-soft: rgba(215, 255, 87, .11);
            --ink: #080a0f;
            --shadow: 0 28px 70px rgba(0, 0, 0, .42);
            --font-display: "Archivo Black", Impact, sans-serif;
            --font-body: "Space Grotesk", Arial, sans-serif;
        }

        * { box-sizing: border-box; }

        body {
            margin: 0;
            font-family: var(--font-body);
            background:
                linear-gradient(120deg, rgba(215, 255, 87, .08), transparent 28%),
                linear-gradient(180deg, #080a0f 0%, #111016 54%, #080a0f 100%);
            color: var(--text);
            min-height: 100vh;
        }

        header {
            background: rgba(8, 10, 15, .82);
            color: #ffffff;
            padding: 18px clamp(16px, 4vw, 44px);
            border-bottom: 1px solid var(--line);
            backdrop-filter: blur(18px);
            position: sticky;
            top: 0;
            z-index: 10;
        }

        .topbar {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 18px;
            max-width: 1320px;
            margin: 0 auto;
        }

        .brand {
            display: grid;
            gap: 4px;
        }

        h1 {
            margin: 0;
            max-width: 820px;
            font-family: var(--font-display);
            font-size: clamp(1.55rem, 3.2vw, 3.4rem);
            line-height: .94;
            letter-spacing: 0;
            text-transform: uppercase;
        }

        .subtitle {
            margin: 0;
            color: var(--accent);
            font-size: .92rem;
            font-weight: 700;
            text-transform: uppercase;
        }

        .nav {
            display: flex;
            flex-wrap: wrap;
            justify-content: flex-end;
            gap: 8px;
            align-items: center;
        }

        .nav a {
            display: inline-flex;
            align-items: center;
            justify-content: center;
            min-height: 36px;
            padding: 7px 11px;
            border: 1px solid var(--line);
            border-radius: 999px;
            color: var(--text);
            background: rgba(244, 241, 232, .04);
            text-decoration: none;
            font-size: .9rem;
            font-weight: 700;
            transition: background .18s ease, color .18s ease, transform .18s ease;
        }

        .nav a:hover {
            background: var(--accent);
            color: var(--ink);
            transform: translateY(-2px);
        }

        .status {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            min-width: max-content;
            padding: 8px 10px;
            border: 1px solid rgba(184, 255, 77, .3);
            border-radius: 999px;
            color: var(--ok);
            font-size: .92rem;
            background: rgba(184, 255, 77, .08);
        }

        .dot {
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background: var(--ok);
            box-shadow: 0 0 18px rgba(184, 255, 77, .8);
        }

        main {
            max-width: 1320px;
            margin: 0 auto;
            padding: 22px clamp(16px, 4vw, 44px) 38px;
        }

        .summary {
            display: grid;
            grid-template-columns: repeat(12, minmax(0, 1fr));
            gap: 16px;
            align-items: stretch;
        }

        .card {
            position: relative;
            overflow: hidden;
            background: var(--panel);
            border: 1px solid var(--line);
            border-radius: 14px;
            padding: 16px;
            min-height: 118px;
            box-shadow: var(--shadow);
            backdrop-filter: blur(14px);
            transition: transform .2s ease, border-color .2s ease, background .2s ease;
        }

        .card:hover {
            transform: translateY(-4px);
            border-color: rgba(215, 255, 87, .42);
            background: rgba(25, 31, 43, .92);
        }

        .summary .card::before {
            content: "";
            position: absolute;
            inset: auto 18px 16px auto;
            width: 38px;
            height: 3px;
            background: currentColor;
        }

        .summary .card:nth-child(1) {
            grid-column: span 3;
            min-height: 138px;
            color: var(--ok);
        }

        .summary .card:nth-child(2),
        .summary .card:nth-child(3),
        .summary .card:nth-child(5),
        .summary .card:nth-child(6) {
            grid-column: span 3;
        }

        .summary .card:nth-child(4) {
            grid-column: span 3;
            color: var(--danger);
        }

        .summary .card:nth-child(7) {
            grid-column: span 3;
            color: var(--warn);
        }

        .summary .card:nth-child(8) {
            grid-column: span 3;
            color: var(--danger);
        }

        .label {
            margin: 0 0 12px;
            color: var(--muted);
            font-size: .76rem;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: .08em;
        }

        .value {
            margin: 0;
            font-family: var(--font-display);
            font-size: clamp(1.7rem, 3.2vw, 3rem);
            line-height: .92;
            font-weight: 800;
            color: var(--text);
        }

        .unit {
            margin-top: 10px;
            color: var(--muted);
            font-size: .92rem;
        }

        .grid {
            display: grid;
            grid-template-columns: minmax(0, 1.15fr) minmax(280px, .55fr);
            gap: 18px;
            margin-top: 18px;
        }

        .section-title {
            margin: 0 0 14px;
            font-family: var(--font-display);
            font-size: clamp(1.1rem, 2vw, 1.6rem);
            line-height: .98;
            text-transform: uppercase;
        }

        .health-row {
            display: grid;
            grid-template-columns: 1fr auto;
            gap: 16px;
            padding: 12px 0;
            border-top: 1px solid var(--line);
        }

        .health-row:first-of-type { border-top: 0; }

        .badge {
            align-self: start;
            border-radius: 999px;
            padding: 7px 11px;
            font-weight: 700;
            font-size: .82rem;
        }

        .ok { background: rgba(184, 255, 77, .12); color: var(--ok); }
        .warn { background: rgba(255, 189, 74, .13); color: var(--warn); }
        .danger { background: rgba(255, 77, 94, .13); color: var(--danger); }

        .links {
            display: grid;
            gap: 9px;
        }

        .links a {
            display: flex;
            align-items: center;
            justify-content: space-between;
            min-height: 46px;
            padding: 9px 11px;
            border: 1px solid var(--line);
            border-radius: 14px;
            color: var(--text);
            text-decoration: none;
            font-weight: 700;
            background: rgba(244, 241, 232, .04);
            transition: transform .18s ease, border-color .18s ease, background .18s ease;
        }

        button {
            width: 100%;
            min-height: 46px;
            border: 1px solid rgba(215, 255, 87, .32);
            border-radius: 14px;
            background: var(--accent);
            color: var(--ink);
            font: inherit;
            font-weight: 700;
            cursor: pointer;
            transition: transform .18s ease, filter .18s ease;
        }

        button:hover,
        .links a:hover {
            transform: translateX(4px);
            border-color: rgba(215, 255, 87, .42);
            background: var(--accent-soft);
        }

        button:hover {
            filter: brightness(1.04);
            background: var(--accent);
        }

        .actions {
            display: grid;
            gap: 10px;
            margin-top: 18px;
        }

        code {
            color: var(--accent);
            font-size: .88rem;
        }

        strong { color: var(--text); }

        @media (max-width: 900px) {
            .summary { grid-template-columns: repeat(2, minmax(0, 1fr)); }
            .summary .card { grid-column: span 1 !important; }
            .grid { grid-template-columns: 1fr; }
            .topbar { align-items: flex-start; flex-direction: column; }
            .nav { justify-content: flex-start; }
        }

        @media (max-width: 560px) {
            .summary { grid-template-columns: 1fr; }
            .summary .card { grid-column: span 1 !important; }
            .health-row { grid-template-columns: 1fr; }
            .status { width: 100%; justify-content: center; }
            h1 { font-size: clamp(1.75rem, 11vw, 3rem); }
        }
    </style>
</head>
<body>
    <header>
        <div class="topbar">
            <div class="brand">
                <h1>Plataforma de Monitoramento DevOps</h1>
                <p class="subtitle">Redes de Computadores 2026</p>
            </div>
            <nav class="nav" aria-label="Navegacao principal">
                <a href="/ataques">Laboratorio</a>
                <a href="http://localhost:3000/d/monitoramento-devops-redes/monitoramento-devops-trabalho-redes?orgId=1&from=now-15m&to=now&timezone=browser&refresh=5s" target="_blank">Grafana <span>:3000</span></a>
                <a href="http://localhost:9090" target="_blank">Prometheus <span>:9090</span></a>
                <div class="status"><span class="dot"></span><span id="coleta">Coletando metricas</span></div>
            </nav>
        </div>
    </header>

    <main>
        <section class="summary" aria-label="Resumo das metricas">
            <article class="card">
                <p class="label">Disponibilidade</p>
                <p class="value" id="servidor_web_up">--</p>
                <div class="unit">web server</div>
            </article>
            <article class="card">
                <p class="label">CPU</p>
                <p class="value" id="percentual_uso_cpu">--</p>
                <div class="unit">percentual</div>
            </article>
            <article class="card">
                <p class="label">Requisicoes</p>
                <p class="value" id="total_requisicoes_http">--</p>
                <div class="unit">total acumulado</div>
            </article>
            <article class="card">
                <p class="label">Erros HTTP</p>
                <p class="value" id="total_erros_http">--</p>
                <div class="unit">4xx/5xx simulados</div>
            </article>
            <article class="card">
                <p class="label">Latencia</p>
                <p class="value" id="ultima_latencia_ms">--</p>
                <div class="unit">milissegundos</div>
            </article>
            <article class="card">
                <p class="label">Conexoes</p>
                <p class="value" id="conexoes_ativas">--</p>
                <div class="unit">ativas agora</div>
            </article>
            <article class="card">
                <p class="label">Login falho</p>
                <p class="value" id="falhas_login_total">--</p>
                <div class="unit">total acumulado</div>
            </article>
            <article class="card">
                <p class="label">Vulnerabilidades</p>
                <p class="value" id="vulnerabilidades_conhecidas">--</p>
                <div class="unit">scanner simulado</div>
            </article>
        </section>

        <section class="grid">
            <article class="card">
                <h2 class="section-title">Saude dos servicos</h2>
                <div class="health-row">
                    <div>
                        <strong>Backend C++</strong>
                        <div><code>/api/trabalho</code> e <code>/metrics</code></div>
                    </div>
                    <span class="badge ok" id="badge-web">OK</span>
                </div>
                <div class="health-row">
                    <div>
                        <strong>Banco PostgreSQL</strong>
                        <div><code>job="postgres"</code> no Prometheus via postgres_exporter</div>
                    </div>
                    <span class="badge warn">Prometheus</span>
                </div>
                <div class="health-row">
                    <div>
                        <strong>Alertas por e-mail</strong>
                        <div>Alertmanager envia alertas reais via Gmail SMTP</div>
                    </div>
                    <span class="badge ok">Configurado</span>
                </div>
                <div class="health-row">
                    <div>
                        <strong>DNS e SMTP</strong>
                        <div>Blackbox Exporter mede disponibilidade e tempo de resposta</div>
                    </div>
                    <span class="badge ok">Probe</span>
                </div>
                <div class="health-row">
                    <div>
                        <strong>Seguranca</strong>
                        <div>DDoS, login falho, alteracao de config e vulnerabilidade conhecida</div>
                    </div>
                    <span class="badge warn">Regras ativas</span>
                </div>
            </article>

            <aside class="card">
                <h2 class="section-title">Atalhos</h2>
                <div class="links">
                    <a href="/metrics" target="_blank">Metricas raw <span>/metrics</span></a>
                    <a href="/ataques" target="_blank">Laboratorio de ataques <span>/ataques</span></a>
                    <a href="http://localhost:9090" target="_blank">Prometheus <span>:9090</span></a>
                    <a href="http://localhost:3000/d/monitoramento-devops-redes/monitoramento-devops-trabalho-redes?orgId=1&from=now-15m&to=now&timezone=browser&refresh=5s&kiosk=tv" target="_blank">Grafana <span>:3000</span></a>
                    <a href="https://mail.google.com" target="_blank">Gmail <span>SMTP :587</span></a>
                </div>
                <div class="actions">
                    <button type="button" data-simulate="/api/simular/login-falho">Simular login falho</button>
                    <button type="button" data-simulate="/api/simular/vulnerabilidade">Simular vulnerabilidade</button>
                    <button type="button" data-simulate="/api/simular/vulnerabilidade/resolver">Resolver vulnerabilidade</button>
                </div>
            </aside>
        </section>
    </main>

    <script>
        const ids = [
            'servidor_web_up',
            'percentual_uso_cpu',
            'total_requisicoes_http',
            'total_erros_http',
            'ultima_latencia_ms',
            'conexoes_ativas',
            'falhas_login_total',
            'vulnerabilidades_conhecidas'
        ];

        function getMetric(metrics, name) {
            if (metrics[name] !== undefined) return metrics[name];
            const found = Object.entries(metrics).find(([key]) => key.startsWith(`${name}{`));
            return found ? found[1] : undefined;
        }

        function parseMetrics(text) {
            return text.split('\n').reduce((acc, line) => {
                if (!line || line.startsWith('#')) return acc;
                const [key, value] = line.trim().split(/\s+/);
                acc[key] = Number(value);
                return acc;
            }, {});
        }

        function formatMetric(id, value) {
            if (!Number.isFinite(value)) return '--';
            if (id === 'servidor_web_up') return value === 1 ? 'UP' : 'DOWN';
            if (id === 'percentual_uso_cpu') return `${value.toFixed(1)}%`;
            if (id === 'ultima_latencia_ms') return value.toFixed(1);
            return value.toLocaleString('pt-BR');
        }

        async function refresh() {
            try {
                const response = await fetch('/metrics', { cache: 'no-store' });
                const metrics = parseMetrics(await response.text());
                ids.forEach((id) => {
                    document.getElementById(id).textContent = formatMetric(id, getMetric(metrics, id));
                });

                const up = metrics.servidor_web_up === 1;
                const badge = document.getElementById('badge-web');
                badge.textContent = up ? 'OK' : 'Critico';
                badge.className = `badge ${up ? 'ok' : 'danger'}`;
                document.getElementById('coleta').textContent = `Atualizado ${new Date().toLocaleTimeString('pt-BR')}`;
            } catch (error) {
                document.getElementById('coleta').textContent = 'Falha ao coletar metricas';
            }
        }

        async function simulate(path) {
            await fetch(path, { cache: 'no-store' });
            await refresh();
        }

        document.querySelectorAll('[data-simulate]').forEach((button) => {
            button.addEventListener('click', () => simulate(button.dataset.simulate));
        });

        refresh();
        setInterval(refresh, 5000);
    </script>
</body>
</html>
)HTML";

        crow::response resposta(html);
        resposta.add_header("Content-Type", "text/html; charset=utf-8");
        return resposta;
    });

    CROW_ROUTE(app, "/ataques")
    ([](){
        const string html = R"HTML(
<!doctype html>
<html lang="pt-BR">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Laboratorio de Ataques</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Archivo+Black&family=Space+Grotesk:wght@400;500;700&display=swap');

        :root {
            color-scheme: dark;
            --bg: #080a0f;
            --surface: rgba(18, 22, 31, .84);
            --text: #f4f1e8;
            --ink: #080a0f;
            --muted: #9aa3b7;
            --line: rgba(244, 241, 232, .13);
            --blue: #66e3ff;
            --blue-soft: rgba(102, 227, 255, .10);
            --red: #ff4d5e;
            --red-soft: rgba(255, 77, 94, .13);
            --amber: #ffbd4a;
            --amber-soft: rgba(255, 189, 74, .13);
            --green: #b8ff4d;
            --green-soft: rgba(184, 255, 77, .12);
            --shadow: 0 28px 70px rgba(0, 0, 0, .42);
            --font-display: "Archivo Black", Impact, sans-serif;
            --font-body: "Space Grotesk", Arial, sans-serif;
        }

        * { box-sizing: border-box; }

        body {
            margin: 0;
            min-height: 100vh;
            background:
                linear-gradient(120deg, rgba(255, 77, 94, .09), transparent 30%),
                linear-gradient(180deg, #080a0f 0%, #120d12 56%, #080a0f 100%);
            color: var(--text);
            font-family: var(--font-body);
        }

        header {
            position: sticky;
            top: 0;
            z-index: 10;
            background: rgba(8, 10, 15, .84);
            color: #fff;
            padding: 18px clamp(16px, 4vw, 44px);
            border-bottom: 1px solid var(--line);
            backdrop-filter: blur(18px);
        }

        .topbar,
        main {
            max-width: 1320px;
            margin: 0 auto;
        }

        .topbar {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 18px;
        }

        .brand {
            display: grid;
            gap: 4px;
        }

        h1 {
            margin: 0;
            max-width: 850px;
            font-family: var(--font-display);
            font-size: clamp(1.55rem, 3.2vw, 3.4rem);
            line-height: .94;
            letter-spacing: 0;
            text-transform: uppercase;
        }

        .subtitle {
            margin: 0;
            color: var(--red);
            font-size: .92rem;
            font-weight: 700;
            text-transform: uppercase;
        }

        .nav {
            display: flex;
            gap: 8px;
            flex-wrap: wrap;
            justify-content: flex-end;
        }

        a,
        button {
            border: 1px solid var(--line);
            border-radius: 999px;
            font: inherit;
            font-weight: 700;
            text-decoration: none;
        }

        .nav a {
            min-height: 40px;
            display: inline-flex;
            align-items: center;
            padding: 7px 11px;
            color: var(--text);
            border-color: var(--line);
            background: rgba(244, 241, 232, .04);
            transition: transform .18s ease, background .18s ease, color .18s ease;
        }

        .nav a:hover {
            background: var(--red);
            color: #fff;
            transform: translateY(-2px);
        }

        main {
            padding: 22px clamp(16px, 4vw, 44px) 38px;
        }

        .layout {
            display: grid;
            grid-template-columns: minmax(0, 1.35fr) minmax(300px, .65fr);
            gap: 16px;
            align-items: start;
        }

        .attacks {
            display: grid;
            grid-template-columns: repeat(12, minmax(0, 1fr));
            gap: 16px;
        }

        .card {
            background: var(--surface);
            border: 1px solid var(--line);
            border-radius: 14px;
            padding: 16px;
            box-shadow: var(--shadow);
            backdrop-filter: blur(14px);
            transition: transform .2s ease, border-color .2s ease, background .2s ease;
            overflow: hidden;
            position: relative;
        }

        .card:hover {
            transform: translateY(-4px);
            border-color: rgba(255, 77, 94, .42);
            background: rgba(25, 31, 43, .94);
        }

        .attacks .card::before {
            content: "";
            position: absolute;
            inset: auto 20px 16px auto;
            height: 3px;
            width: 38px;
            background: currentColor;
        }

        .attacks .card:nth-child(1) {
            grid-column: span 6;
            min-height: 238px;
            color: var(--red);
        }

        .attacks .card:nth-child(2) {
            grid-column: span 6;
            color: var(--amber);
        }

        .attacks .card:nth-child(3) {
            grid-column: span 6;
            color: var(--blue);
        }

        .attacks .card:nth-child(4) {
            grid-column: span 6;
            color: var(--green);
        }

        .card h2,
        .card h3 {
            margin: 0;
            letter-spacing: 0;
        }

        .card h2 {
            max-width: 720px;
            color: var(--text);
            font-family: var(--font-display);
            font-size: clamp(1.05rem, 2vw, 1.65rem);
            line-height: .98;
            text-transform: uppercase;
        }

        .card h3 {
            color: var(--text);
            font-family: var(--font-display);
            font-size: clamp(1.05rem, 1.8vw, 1.45rem);
            line-height: .98;
            text-transform: uppercase;
        }

        .meta {
            margin: 10px 0 14px;
            color: var(--muted);
            font-size: .9rem;
            line-height: 1.45;
        }

        .controls {
            display: grid;
            grid-template-columns: repeat(2, minmax(0, 1fr));
            gap: 10px;
            margin: 12px 0;
        }

        label {
            display: grid;
            gap: 6px;
            color: var(--muted);
            font-size: .75rem;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: .08em;
        }

        input {
            width: 100%;
            min-height: 42px;
            border: 1px solid var(--line);
            border-radius: 14px;
            padding: 9px 12px;
            font: inherit;
            color: var(--text);
            background: rgba(244, 241, 232, .05);
        }

        input:focus {
            outline: 3px solid rgba(102, 227, 255, .16);
            border-color: var(--blue);
        }

        button {
            width: 100%;
            min-height: 42px;
            padding: 9px 12px;
            cursor: pointer;
            color: #fff;
            background: var(--blue);
            border-color: var(--blue);
            border-radius: 14px;
            box-shadow: 0 14px 28px rgba(102, 227, 255, .16);
            transition: transform .14s ease, box-shadow .14s ease, filter .14s ease;
        }

        button:hover {
            transform: translateY(-1px);
            filter: brightness(1.02);
        }

        button.secondary {
            color: var(--blue);
            background: rgba(102, 227, 255, .08);
            box-shadow: none;
        }

        button.danger {
            color: #fff;
            background: var(--red);
            border-color: var(--red);
            box-shadow: 0 14px 28px rgba(255, 77, 94, .18);
        }

        button.warning {
            color: var(--ink);
            background: var(--amber-soft);
            border-color: rgba(255, 189, 74, .36);
            box-shadow: none;
        }

        button:disabled {
            cursor: not-allowed;
            opacity: .62;
        }

        .progress {
            height: 10px;
            border-radius: 999px;
            background: rgba(244, 241, 232, .08);
            overflow: hidden;
            margin-top: 14px;
        }

        .bar {
            width: 0%;
            height: 100%;
            background: currentColor;
            transition: width .18s ease;
        }

        .log {
            min-height: 44px;
            margin-top: 12px;
            padding: 10px;
            border: 1px solid rgba(244, 241, 232, .12);
            border-radius: 14px;
            background: #07090d;
            color: #cbd5e1;
            font-size: .9rem;
            line-height: 1.35;
            font-family: var(--font-body);
        }

        .metrics {
            display: grid;
            gap: 9px;
            margin-top: 16px;
        }

        .metric {
            display: flex;
            justify-content: space-between;
            gap: 12px;
            padding: 12px;
            border: 1px solid var(--line);
            border-radius: 14px;
            background: rgba(244, 241, 232, .04);
            color: var(--muted);
        }

        .metric strong {
            color: var(--text);
            font-size: 1.05rem;
        }

        .pill {
            display: inline-flex;
            align-items: center;
            min-height: 30px;
            width: max-content;
            border-radius: 999px;
            padding: 7px 10px;
            font-size: .82rem;
            font-weight: 800;
            color: var(--green);
            background: var(--green-soft);
        }

        .pill.red {
            color: var(--red);
            background: var(--red-soft);
        }

        @media (max-width: 920px) {
            .layout,
            .attacks {
                grid-template-columns: 1fr;
            }

            .attacks .card {
                grid-column: span 1 !important;
            }

            .topbar {
                align-items: flex-start;
                flex-direction: column;
            }
        }

        @media (max-width: 560px) {
            .controls {
                grid-template-columns: 1fr;
            }

            h1 { font-size: clamp(1.75rem, 11vw, 3rem); }
        }
    </style>
</head>
<body>
    <header>
        <div class="topbar">
            <div class="brand">
                <h1>Laboratorio de Ataques</h1>
                <p class="subtitle">Redes de Computadores 2026</p>
            </div>
            <nav class="nav" aria-label="Atalhos">
                <a href="/">Monitoramento</a>
                <a href="http://localhost:3000/d/monitoramento-devops-redes/monitoramento-devops-trabalho-redes?orgId=1&from=now-15m&to=now&timezone=browser&refresh=5s" target="_blank">Grafana <span>:3000</span></a>
                <a href="http://localhost:9090" target="_blank">Prometheus <span>:9090</span></a>
                <a href="https://mail.google.com" target="_blank">Gmail <span>SMTP :587</span></a>
            </nav>
        </div>
    </header>

    <main>
        <section class="layout">
            <div class="attacks">
                <article class="card">
                    <h2>Pico HTTP / DDoS local</h2>
                    <p class="meta">Dispara requisicoes contra <code>/api/trabalho</code> para elevar RPS, conexoes e erros HTTP.</p>
                    <div class="controls">
                        <label>Requisicoes
                            <input id="http-total" type="number" min="10" max="2000" step="10" value="300">
                        </label>
                        <label>Concorrencia
                            <input id="http-concurrency" type="number" min="1" max="50" step="1" value="20">
                        </label>
                    </div>
                    <button type="button" id="http-run" class="danger">Iniciar pico HTTP</button>
                    <div class="progress"><div class="bar" id="http-bar"></div></div>
                    <div class="log" id="http-log">Aguardando execucao.</div>
                </article>

                <article class="card">
                    <h2>Brute force de login</h2>
                    <p class="meta">Gera falhas de autenticacao para testar o alerta de login falho.</p>
                    <div class="controls">
                        <label>Tentativas
                            <input id="login-total" type="number" min="1" max="200" step="1" value="8">
                        </label>
                        <label>Intervalo ms
                            <input id="login-delay" type="number" min="0" max="2000" step="50" value="100">
                        </label>
                    </div>
                    <button type="button" id="login-run" class="warning">Simular brute force</button>
                    <div class="progress"><div class="bar" id="login-bar"></div></div>
                    <div class="log" id="login-log">Aguardando execucao.</div>
                </article>

                <article class="card">
                    <h2>Alteracao de configuracao</h2>
                    <p class="meta">Altera uma metrica de checksum simulada para acionar o alerta de mudanca de configuracao.</p>
                    <button type="button" id="config-run" class="secondary">Simular alteracao</button>
                    <div class="log" id="config-log">Nenhuma alteracao simulada nesta sessao.</div>
                </article>

                <article class="card">
                    <h2>Vulnerabilidade conhecida</h2>
                    <p class="meta">Marca uma CVE demonstrativa como ativa ou resolvida para testar alerta critico.</p>
                    <div class="controls">
                        <button type="button" id="vuln-run" class="danger">Ativar vulnerabilidade</button>
                        <button type="button" id="vuln-clear" class="secondary">Resolver vulnerabilidade</button>
                    </div>
                    <div class="log" id="vuln-log">Estado atual sera refletido em vulnerabilidades_conhecidas.</div>
                </article>
            </div>

            <aside class="card">
                <div class="pill" id="status-pill">Coletando metricas</div>
                <h3 style="margin-top: 14px;">Metricas ao vivo</h3>
                <div class="metrics">
                    <div class="metric"><span>Requisicoes</span><strong id="m-total">--</strong></div>
                    <div class="metric"><span>Erros HTTP</span><strong id="m-errors">--</strong></div>
                    <div class="metric"><span>Login falho</span><strong id="m-login">--</strong></div>
                    <div class="metric"><span>Conexoes</span><strong id="m-connections">--</strong></div>
                    <div class="metric"><span>Latencia ms</span><strong id="m-latency">--</strong></div>
                    <div class="metric"><span>Vulnerabilidades</span><strong id="m-vuln">--</strong></div>
                </div>
                <div class="log" id="status-log">Os alertas aparecem no Prometheus, Grafana e na caixa de entrada do Gmail apos o tempo de avaliacao.</div>
            </aside>
        </section>
    </main>

    <script>
        const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

        function clampNumber(value, min, max) {
            const number = Number(value);
            if (!Number.isFinite(number)) return min;
            return Math.min(max, Math.max(min, number));
        }

        function parseMetrics(text) {
            return text.split('\n').reduce((acc, line) => {
                if (!line || line.startsWith('#')) return acc;
                const [key, value] = line.trim().split(/\s+/);
                acc[key] = Number(value);
                return acc;
            }, {});
        }

        function getMetric(metrics, name) {
            if (metrics[name] !== undefined) return metrics[name];
            const found = Object.entries(metrics).find(([key]) => key.startsWith(`${name}{`));
            return found ? found[1] : undefined;
        }

        function setProgress(id, done, total) {
            const percent = total > 0 ? Math.round((done / total) * 100) : 0;
            document.getElementById(id).style.width = `${Math.min(100, percent)}%`;
        }

        async function requestQuietly(path) {
            try {
                await fetch(path, { cache: 'no-store' });
            } catch (error) {
                return false;
            }
            return true;
        }

        async function runPool(total, concurrency, path, progressId, logId) {
            let launched = 0;
            let finished = 0;
            const workers = Array.from({ length: concurrency }, async () => {
                while (launched < total) {
                    launched++;
                    await requestQuietly(path);
                    finished++;
                    setProgress(progressId, finished, total);
                    document.getElementById(logId).textContent = `${finished}/${total} requisicoes concluidas.`;
                }
            });
            await Promise.all(workers);
        }

        async function runHttpAttack() {
            const button = document.getElementById('http-run');
            const total = clampNumber(document.getElementById('http-total').value, 10, 100000);
            const concurrency = clampNumber(document.getElementById('http-concurrency').value, 1, 500);
            button.disabled = true;
            document.getElementById('http-log').textContent = 'Executando pico HTTP local...';
            setProgress('http-bar', 0, total);
            await runPool(total, concurrency, '/api/trabalho', 'http-bar', 'http-log');
            document.getElementById('http-log').textContent = `Pico concluido: ${total} requisicoes com concorrencia ${concurrency}.`;
            button.disabled = false;
            await refreshMetrics();
        }

        async function runLoginAttack() {
            const button = document.getElementById('login-run');
            const total = clampNumber(document.getElementById('login-total').value, 1, 100000);
            const delay = clampNumber(document.getElementById('login-delay').value, 0, 2000);
            button.disabled = true;
            setProgress('login-bar', 0, total);
            for (let i = 1; i <= total; i++) {
                await requestQuietly('/api/simular/login-falho');
                setProgress('login-bar', i, total);
                document.getElementById('login-log').textContent = `${i}/${total} tentativas registradas.`;
                if (delay > 0) await sleep(delay);
            }
            document.getElementById('login-log').textContent = `Brute force simulado: ${total} falhas de login.`;
            button.disabled = false;
            await refreshMetrics();
        }

        async function refreshMetrics() {
            try {
                const response = await fetch('/metrics', { cache: 'no-store' });
                const metrics = parseMetrics(await response.text());
                document.getElementById('m-total').textContent = (getMetric(metrics, 'total_requisicoes_http') ?? '--').toLocaleString('pt-BR');
                document.getElementById('m-errors').textContent = (getMetric(metrics, 'total_erros_http') ?? '--').toLocaleString('pt-BR');
                document.getElementById('m-login').textContent = (getMetric(metrics, 'falhas_login_total') ?? '--').toLocaleString('pt-BR');
                document.getElementById('m-connections').textContent = (getMetric(metrics, 'conexoes_ativas') ?? '--').toLocaleString('pt-BR');
                const latency = getMetric(metrics, 'ultima_latencia_ms');
                document.getElementById('m-latency').textContent = Number.isFinite(latency) ? latency.toFixed(1) : '--';
                document.getElementById('m-vuln').textContent = (getMetric(metrics, 'vulnerabilidades_conhecidas') ?? '--').toLocaleString('pt-BR');
                document.getElementById('status-pill').textContent = `Atualizado ${new Date().toLocaleTimeString('pt-BR')}`;
                document.getElementById('status-pill').className = 'pill';
            } catch (error) {
                document.getElementById('status-pill').textContent = 'Falha na coleta';
                document.getElementById('status-pill').className = 'pill red';
            }
        }

        document.getElementById('http-run').addEventListener('click', runHttpAttack);
        document.getElementById('login-run').addEventListener('click', runLoginAttack);
        document.getElementById('config-run').addEventListener('click', async () => {
            await requestQuietly('/api/simular/configuracao');
            document.getElementById('config-log').textContent = 'Alteracao de configuracao simulada.';
            await refreshMetrics();
        });
        document.getElementById('vuln-run').addEventListener('click', async () => {
            await requestQuietly('/api/simular/vulnerabilidade');
            document.getElementById('vuln-log').textContent = 'Vulnerabilidade marcada como ativa.';
            await refreshMetrics();
        });
        document.getElementById('vuln-clear').addEventListener('click', async () => {
            await requestQuietly('/api/simular/vulnerabilidade/resolver');
            document.getElementById('vuln-log').textContent = 'Vulnerabilidade marcada como resolvida.';
            await refreshMetrics();
        });

        refreshMetrics();
        setInterval(refreshMetrics, 3000);
    </script>
</body>
</html>
)HTML";

        crow::response resposta(html);
        resposta.add_header("Content-Type", "text/html; charset=utf-8");
        return resposta;
    });

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

    CROW_ROUTE(app, "/api/simular/login-falho")
    ([](){
        total_falhas_login++;
        return crow::response(401, "Falha de autenticacao simulada");
    });

    CROW_ROUTE(app, "/api/simular/vulnerabilidade")
    ([](){
        vulnerabilidades_conhecidas = 1;
        return crow::response(200, "Vulnerabilidade conhecida simulada");
    });

    CROW_ROUTE(app, "/api/simular/configuracao")
    ([](){
        alteracoes_configuracao++;
        return crow::response(200, "Alteracao de configuracao simulada");
    });

    CROW_ROUTE(app, "/api/simular/vulnerabilidade/resolver")
    ([](){
        vulnerabilidades_conhecidas = 0;
        return crow::response(200, "Vulnerabilidade resolvida");
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
        metrica += "conexoes_ativas " + to_string(conexoes_ativas) + "\n\n";

        metrica += "# HELP falhas_login_total Total acumulado de falhas de autenticacao simuladas\n";
        metrica += "# TYPE falhas_login_total counter\n";
        metrica += "falhas_login_total " + to_string(total_falhas_login) + "\n\n";

        metrica += "# HELP configuracao_checksum Checksum dos arquivos de configuracao monitorados\n";
        metrica += "# TYPE configuracao_checksum gauge\n";
        metrica += "configuracao_checksum{arquivo=\"prometheus.yml\"} " + to_string(calcular_checksum_arquivo("prometheus.yml")) + "\n";
        metrica += "configuracao_checksum{arquivo=\"condicoes_alerta.yml\"} " + to_string(calcular_checksum_arquivo("condicoes_alerta.yml")) + "\n";
        metrica += "configuracao_checksum{arquivo=\"config_alertas.yml\"} " + to_string(calcular_checksum_arquivo("config_alertas.yml")) + "\n";
        metrica += "configuracao_checksum{arquivo=\"simulacao_runtime\"} " + to_string(alteracoes_configuracao) + "\n\n";

        metrica += "# HELP vulnerabilidades_conhecidas Quantidade de vulnerabilidades conhecidas por servico\n";
        metrica += "# TYPE vulnerabilidades_conhecidas gauge\n";
        metrica += "vulnerabilidades_conhecidas{servico=\"backend_cpp\",cve=\"CVE-DEMO-2026-0001\"} " + to_string(vulnerabilidades_conhecidas) + "\n";

        crow::response resposta(metrica);
        resposta.add_header("Content-Type", "text/plain; version=0.0.4");
        return resposta;
    });

    cout << "Backend operando na porta 8080..." << endl;
    app.port(8080).multithreaded().run();

    return 0;
}
