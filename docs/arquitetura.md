# Arquitetura da Plataforma

## Visao Geral

A plataforma monitora servicos de rede e infraestrutura usando backend C++, Prometheus, Grafana, Alertmanager, Gmail SMTP, PostgreSQL, Postgres Exporter e Blackbox Exporter.

## Componentes

| Componente | Responsabilidade |
| --- | --- |
| Backend C++ | Serve o front-end, expoe metricas em `/metrics` e possui rotas de simulacao. |
| PostgreSQL | Banco da plataforma, com tabelas de metricas e incidentes. |
| Postgres Exporter | Exporta metricas do PostgreSQL para o Prometheus. |
| Prometheus | Coleta metricas, armazena series temporais e avalia alertas. |
| Grafana | Exibe dashboards por servico e visao consolidada. |
| Alertmanager | Recebe alertas do Prometheus e envia notificacoes. |
| Gmail SMTP | Envia e-mails reais de alerta para a equipe. |
| Blackbox Exporter | Executa probes HTTP, DNS e SMTP. |

## Fluxo

1. O backend C++ expoe metricas em `/metrics`.
2. O Prometheus coleta metricas do backend, banco e probes.
3. O Grafana consulta o Prometheus para renderizar dashboards.
4. O Prometheus envia alertas ao Alertmanager.
5. O Alertmanager envia e-mails reais pelo SMTP do Gmail.
