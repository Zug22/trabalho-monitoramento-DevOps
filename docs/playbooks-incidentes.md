# Playbooks de Incidentes

## CPU Alta

1. Conferir o painel de CPU no Grafana.
2. Verificar se há teste de carga em execução.
3. Consultar logs do backend.
4. Reduzir carga ou reiniciar serviço se necessário.

## Erros HTTP ou DDoS

1. Conferir RPS e taxa de erro.
2. Validar a rota `/api/trabalho`.
3. Verificar se o laboratório de ataques foi acionado.
4. Em ambiente real, aplicar rate limit ou bloqueio de origem.

## Banco Indisponível

1. Conferir `docker compose ps postgres`.
2. Verificar logs do PostgreSQL e Postgres Exporter.
3. Conferir conexões abertas, transações e rollbacks.

## Falhas de Autenticação

1. Conferir a métrica `falhas_login_total`.
2. Verificar se foi executada a simulação de brute force.
3. Em ambiente real, aplicar bloqueio temporário e revisão de autenticação.

## Vulnerabilidade Conhecida

1. Identificar o serviço afetado.
2. Conferir se a simulação está ativa.
3. Corrigir versão vulnerável em ambiente real.
4. Validar resolução no dashboard.

