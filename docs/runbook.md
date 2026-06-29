# Runbook

## Subir o Ambiente

```bash
docker compose up -d --build
```

## Verificar Servicos

```bash
docker compose ps
```

## URLs

| Servico | URL |
| --- | --- |
| Front principal | http://localhost:8080 |
| Laboratorio de ataques | http://localhost:8080/ataques |
| Prometheus | http://localhost:9090 |
| Grafana | http://localhost:3000 |
| Alertmanager | http://localhost:9093 |
| Gmail | https://mail.google.com |

## Logs

```bash
docker compose logs backend
docker compose logs prometheus
docker compose logs grafana
docker compose logs alertmanager
```

## Parar

```bash
docker compose down
```
