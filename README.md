# 🚀 Infraestrutura de Monitoramento e DevOps (C++ Backend)

Este repositório contém a implementação da arquitetura de monitoramento em tempo real exigida para o projeto final da disciplina de Redes. O ambiente foi orquestrado para coletar, armazenar, analisar e alertar sobre o comportamento e a saúde de um servidor web backend escrito em C++.

## 🛠️ Arquitetura e Tecnologias

A stack foi totalmente conteinerizada (exceto o backend) e opera no ecossistema Linux:

* **Backend (C++):** Servidor HTTP simulando processamento multithread. Possui contadores atômicos (`<atomic>`) e injeta intencionalmente uma falha (HTTP 500) a cada 10 requisições (10% de taxa de erro) para testes de resiliência.
* **Prometheus:** Motor principal de raspagem (*scraping*). Realiza a ingestão das métricas do servidor a cada 15 segundos e avaliação contínua das regras de saúde.
* **Grafana:** Front-end da equipe de DevOps. Consome dados do Prometheus para exibir painéis em tempo real (CPU, RPS e Erros), com persistência de dados em volumes Docker.
* **Alertmanager:** Orquestrador de crises. Avalia estados críticos e despacha notificações.
* **MailHog:** Servidor SMTP de testes local. Intercepta os e-mails disparados pelo Alertmanager para validação em ambiente de desenvolvimento sem utilizar caixas de entrada reais.

---

## ⚙️ Pré-requisitos (Ambiente Linux)

Antes de iniciar, certifique-se de possuir os seguintes pacotes instalados no sistema Ubuntu/Debian:

```bash
sudo apt update
sudo apt install g++ docker-compose apache2-utils stress -y
```

---

# 🚀 Como Executar o Projeto

## 1. Subir a Infraestrutura de Contêineres

Inicie os serviços do Prometheus, Grafana, Alertmanager e MailHog em segundo plano:

```bash
sudo docker-compose up -d
```

---

## 2. Compilar e Executar o Backend (C++)

Compile o código vinculando a biblioteca de threads do Linux e execute o servidor na porta 8080:

```bash
g++ main.cpp -lpthread -o exportador
./exportador &
```

---

# 📊 Acesso aos Painéis e Serviços

Com todos os serviços ativos, acesse-os diretamente pelo navegador:

| Serviço           | URL Local             | Descrição                                                    |
| ----------------- | --------------------- | ------------------------------------------------------------ |
| Aplicação Backend | http://localhost:8080 | Responde pelas rotas da API (ex: `/api/trabalho`).           |
| Prometheus        | http://localhost:9090 | Consulta bruta de métricas (PromQL) e status dos alertas.    |
| Alertmanager      | http://localhost:9093 | Gerenciamento de silenciamento e roteamento de alertas.      |
| Grafana           | http://localhost:3000 | Dashboards visuais. Credenciais padrão: admin/admin.         |
| MailHog (Web UI)  | http://localhost:8025 | Caixa de entrada virtual para leitura dos e-mails de alerta. |

---

# 🔥 Testes de Estresse e Simulação de Falhas

Para acionar os gatilhos do Alertmanager e observar os gráficos reagindo no Grafana, utilize os comandos abaixo no terminal Linux.

## Simular Pico de Uso de CPU (Gera Alerta Amarelo/Vermelho)

Sufoca 16 núcleos do processador durante 2 minutos, forçando o envio de alerta por uso excessivo de recursos:

```bash
stress --cpu 16 --timeout 120 &
```

---

## Simular Ataque DDoS e Taxa de Erros HTTP (Gera Alerta Crítico)

Dispara 20.000 requisições com 100 usuários simultâneos, testando a sincronização multithread do C++ e forçando alertas de falha HTTP 500:

```bash
ab -n 20000 -c 100 http://localhost:8080/api/trabalho
```

---

# 📚 Explicando a Estrutura do Código Gerado

* **Markdown e Sintaxe Visual:** O arquivo utiliza a linguagem de marcação Markdown. Os símbolos `#` e `##` definem a hierarquia dos títulos e subtítulos. Os emojis foram utilizados nas seções principais para melhorar a organização visual e facilitar a leitura.

* **Blocos de Código (Bash):** Os comandos de terminal utilizam blocos de código com identificação `bash`, permitindo melhor visualização e cópia dos comandos.

* **Tabelas de Acesso:** A tabela da seção "Acesso aos Painéis" utiliza barras verticais (`|`) para separar as colunas e organizar as informações referentes às portas e serviços.

* **Orientação Baseada em Linux:** Os pré-requisitos, comandos de instalação (`apt install apache2-utils stress`) e execução em segundo plano (`&`) foram definidos considerando um ambiente Unix/Linux.
