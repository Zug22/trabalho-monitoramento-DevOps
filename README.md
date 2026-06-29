# 🚀 Infraestrutura de Monitoramento e DevOps (C++ Backend)

Este repositório contém a implementação da arquitetura de monitoramento em tempo real exigida para o projeto final da disciplina de Redes. O ambiente foi orquestrado para coletar, armazenar, analisar e alertar sobre o comportamento e a saúde de um servidor web backend escrito em C++.

## 🛠️ Arquitetura e Tecnologias

A stack foi totalmente conteinerizada e opera com Docker Compose:

* **Backend (C++):** Servidor HTTP com front-end responsivo em `/`, API de teste em `/api/trabalho`, rotas de simulação e métricas Prometheus em `/metrics`.
* **PostgreSQL:** Banco de dados da plataforma, iniciado com tabelas para histórico de métricas e incidentes.
* **Postgres Exporter:** Exporta métricas do banco para o Prometheus, incluindo disponibilidade, conexões, transações, rollbacks e tamanho do banco.
* **Prometheus:** Motor principal de raspagem (*scraping*). Realiza a ingestão das métricas do servidor a cada 15 segundos e avaliação contínua das regras de saúde.
* **Blackbox Exporter:** Executa probes HTTP, DNS e SMTP para medir disponibilidade e latência de serviços de rede.
* **Grafana:** Front-end da equipe de DevOps. Consome dados do Prometheus para exibir painéis em tempo real, com datasource e dashboard provisionados automaticamente.
* **Alertmanager:** Orquestrador de crises. Avalia estados críticos e despacha notificações.
* **Gmail SMTP:** Canal real de e-mail usado pelo Alertmanager para enviar alertas para a equipe.

---

## ⚙️ Pré-requisitos

Para executar tudo via contêiner:

```bash
docker --version
docker compose version
```

Em Linux, caso queira compilar o backend fora do Docker:

```bash
sudo apt update
sudo apt install g++ libboost-all-dev apache2-utils stress -y
make
```

---

# 🚀 Como Executar o Projeto

Suba toda a infraestrutura:

```bash
docker compose up -d --build
```

O backend C++ também é compilado dentro do Docker. Se preferir executar manualmente no Linux, use `make run`.

---

# 📊 Acesso aos Painéis e Serviços

Com todos os serviços ativos, acesse-os diretamente pelo navegador:

| Serviço           | URL Local             | Descrição                                                    |
| ----------------- | --------------------- | ------------------------------------------------------------ |
| Front/Backend C++ | http://localhost:8080 | Dashboard próprio, API e métricas em `/metrics`.             |
| Laboratório       | http://localhost:8080/ataques | Cenários controlados para testar alertas da aplicação. |
| PostgreSQL        | localhost:5432        | Banco `monitoramento`, usuário `monitor`, senha `monitor123`. |
| Postgres Exporter | http://localhost:9187 | Métricas Prometheus do banco de dados.                       |
| Blackbox Exporter | http://localhost:9115 | Probes de HTTP, DNS e SMTP.                                  |
| Prometheus        | http://localhost:9090 | Consulta bruta de métricas (PromQL) e status dos alertas.    |
| Alertmanager      | http://localhost:9093 | Gerenciamento de silenciamento e roteamento de alertas.      |
| Grafana           | http://localhost:3000 | Dashboard provisionado. Credenciais padrão: admin/admin.     |
| Gmail SMTP        | smtp.gmail.com:587    | Envio real dos e-mails de alerta pelo Alertmanager.          |

---

# Documentação da Entrega

Os documentos de apoio ficam em `docs/`:

* `arquitetura.md`: visão técnica da solução.
* `runbook.md`: guia de instalação, execução e checagens.
* `playbooks-incidentes.md`: resposta a incidentes e níveis de alerta.
* `testes-e-video.md`: roteiro de navegação e gravação do vídeo.
* `relatorio-base.md`: estrutura para transformar em PDF no padrão de artigo/ABNT.
* `checklist-entrega.md`: pendências finais antes de enviar.

O PDF final deve ser compilado fora do repositório, a partir do texto atualizado do relatório, para evitar versionar artefatos binários gerados.

---

# Métricas do Banco de Dados

O Prometheus coleta o job `postgres` pelo container `postgres_exporter`. As principais consultas PromQL para o dashboard do banco são:

```promql
pg_up
sum(pg_stat_activity_count)
sum(rate(pg_stat_database_xact_commit[1m]) + rate(pg_stat_database_xact_rollback[1m]))
sum(rate(pg_stat_database_xact_rollback[1m]))
pg_database_size_bytes{datname="monitoramento"}
```

Outras consultas usadas no dashboard:

```promql
rate(total_requisicoes_http[1m])
rate(total_erros_http[1m])
probe_success{job="blackbox_http_backend"}
probe_success{job="blackbox_dns"}
probe_success{job="blackbox_smtp"}
increase(falhas_login_total[5m])
vulnerabilidades_conhecidas
changes(configuracao_checksum[5m])
```

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

## Simular Falhas de Autenticação

```bash
for i in {1..6}; do curl -i http://localhost:8080/api/simular/login-falho; done
```

## Simular Vulnerabilidade Conhecida

```bash
curl http://localhost:8080/api/simular/vulnerabilidade
curl http://localhost:8080/api/simular/vulnerabilidade/resolver
```

## Usar o Laboratório de Ataques

Abra `http://localhost:8080/ataques` para executar os cenários pelo navegador:

* Pico HTTP / DDoS local contra `/api/trabalho`.
* Brute force de login contra `/api/simular/login-falho`.
* Alteração de configuração simulada.
* Vulnerabilidade conhecida simulada.

## Simular Alteração de Configuração

Edite e salve `prometheus.yml`, `condicoes_alerta.yml` ou `config_alertas.yml`. O backend expõe o checksum desses arquivos em `/metrics`, e o Prometheus dispara alerta se houver mudança.

---

# Estado Atual

O projeto já atende aos principais itens do enunciado: front-end responsivo, backend de métricas, banco monitorado, dashboards, alertas por e-mail, runbooks/playbooks e monitoramentos extras de segurança. A validação final depende de subir a stack no Docker e gravar o vídeo de demonstração.
