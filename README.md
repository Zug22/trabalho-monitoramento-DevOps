# 🚀 Infraestrutura de Monitoramento e DevOps (C++ Backend)

Implementação de arquitetura de monitoramento em tempo real para o projeto final da disciplina de Redes. Ambiente orquestrado com Docker Compose para coletar, armazenar, analisar e alertar sobre a saúde de um servidor web backend escrito em C++.

## 🛠️ Arquitetura e Tecnologias

| Serviço | Descrição |
|---|---|
| **Backend (C++)** | Servidor HTTP com front-end em `/`, API em `/api/trabalho`, laboratório de ataques em `/ataques` e métricas Prometheus em `/metrics` |
| **PostgreSQL** | Banco de dados com tabelas para histórico de métricas e incidentes |
| **Postgres Exporter** | Exporta métricas do banco para o Prometheus |
| **Prometheus** | Scraping de métricas a cada 15s e avaliação contínua de alertas |
| **Blackbox Exporter** | Probes HTTP, DNS e SMTP para disponibilidade e latência |
| **Grafana** | Dashboards em tempo real com datasource e dashboard provisionados automaticamente |
| **Alertmanager** | Orquestrador de alertas com envio de e-mail via Gmail SMTP |

---

## ⚙️ Pré-requisitos

```bash
docker --version
docker compose version
```

Para compilar fora do Docker (Linux):

```bash
sudo apt update
sudo apt install g++ libboost-all-dev apache2-utils stress -y
make
```

---

## 🚀 Como Executar

```bash
docker compose up -d --build
```

---

## 📊 Serviços e URLs

| Serviço | URL | Descrição |
|---|---|---|
| Front/Backend C++ | http://localhost:8080 | Dashboard, API e métricas |
| Laboratório de Ataques | http://localhost:8080/ataques | Cenários controlados para testar alertas |
| PostgreSQL | localhost:5432 | Banco `monitoramento`, user `monitor`, senha `monitor123` |
| Postgres Exporter | http://localhost:9187 | Métricas Prometheus do banco |
| Blackbox Exporter | http://localhost:9115 | Probes de HTTP, DNS e SMTP |
| Prometheus | http://localhost:9090 | PromQL e status dos alertas |
| Alertmanager | http://localhost:9093 | Gerenciamento de alertas |
| Grafana | http://localhost:3000 | Dashboard provisionado (acesso anônimo habilitado) |

---

## 🔥 Testes de Estresse e Simulação

### Pico de CPU
```bash
stress --cpu 16 --timeout 120 &
```

### DDoS / Pico HTTP
```bash
ab -n 20000 -c 100 http://localhost:8080/api/trabalho
```

### Falhas de Autenticação
```bash
for i in {1..6}; do curl -i http://localhost:8080/api/simular/login-falho; done
```

### Vulnerabilidade Conhecida
```bash
curl http://localhost:8080/api/simular/vulnerabilidade
curl http://localhost:8080/api/simular/vulnerabilidade/resolver
```

### Alteração de Configuração
```bash
curl http://localhost:8080/api/simular/configuracao
```

Ou use o **Laboratório de Ataques** em http://localhost:8080/ataques.

---

## 📈 Métricas do Banco (PromQL)

```promql
pg_up
sum(pg_stat_activity_count)
sum(rate(pg_stat_database_xact_commit[1m]) + rate(pg_stat_database_xact_rollback[1m]))
pg_database_size_bytes{datname="monitoramento"}
rate(total_requisicoes_http[1m])
rate(total_erros_http[1m])
probe_success{job="blackbox_http_backend"}
increase(falhas_login_total[5m])
vulnerabilidades_conhecidas
changes(configuracao_checksum[5m])
```

---

## 📁 Documentação

Os documentos ficam em `docs/`:

| Arquivo | Descrição |
|---|---|
| `arquitetura.md` | Visão técnica da solução |
| `runbook.md` | Guia de instalação e execução |
| `playbooks-incidentes.md` | Resposta a incidentes |
| `testes-e-video.md` | Roteiro do vídeo de demonstração |
| `relatorio-base.md` | Estrutura do relatório |
| `relatorio-overleaf.txt` | Apoio para o Overleaf |
| `RedesSecOps.pdf` | PDF final do trabalho |
| `checklist-entrega.md` | Pendências finais |
