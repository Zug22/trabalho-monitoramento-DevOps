# Runbook

## Subir o Ambiente

```bash
docker compose up -d --build
```

## Verificar Serviços

```bash
docker compose ps
```

## URLs

| Serviço | URL |
| --- | --- |
| Front principal | http://localhost:8080 |
| Laboratório de ataques | http://localhost:8080/ataques |
| Prometheus | http://localhost:9090 |
| Grafana | http://localhost:3000 |
| Alertmanager | http://localhost:9093 |
| MailHog | http://localhost:8025 |

## Logs

```bash
docker compose logs backend
docker compose logs prometheus
docker compose logs grafana
```

## Parar

```bash
docker compose down
```

