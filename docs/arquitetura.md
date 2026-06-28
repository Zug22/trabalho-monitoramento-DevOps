# Arquitetura da Plataforma

## Visão Geral

A plataforma monitora serviços de rede e infraestrutura usando backend C++, Prometheus, Grafana, Alertmanager, MailHog, PostgreSQL, Postgres Exporter e Blackbox Exporter.

## Componentes

| Componente | Responsabilidade |
| --- | --- |
| Backend C++ | Serve o front-end, expõe métricas em `/metrics` e possui rotas de simulação. |
| PostgreSQL | Banco da plataforma, com tabelas de métricas e incidentes. |
| Postgres Exporter | Exporta métricas do PostgreSQL para o Prometheus. |
| Prometheus | Coleta métricas, armazena séries temporais e avalia alertas. |
| Grafana | Exibe dashboards por serviço e visão consolidada. |
| Alertmanager | Recebe alertas do Prometheus e envia notificações. |
| MailHog | Simula uma caixa de e-mail local para validar alertas. |
| Blackbox Exporter | Executa probes HTTP, DNS e SMTP. |

## Fluxo

1. O backend C++ expõe métricas em `/metrics`.
2. O Prometheus coleta métricas do backend, banco e probes.
3. O Grafana consulta o Prometheus para renderizar dashboards.
4. O Prometheus envia alertas ao Alertmanager.
5. O Alertmanager envia e-mails ao MailHog.

