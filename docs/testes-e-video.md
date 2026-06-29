# Testes e Roteiro de Video

## Navegacao

1. Abrir `http://localhost:8080`.
2. Conferir cards de metricas.
3. Abrir `http://localhost:8080/ataques`.
4. Abrir Grafana em `http://localhost:3000`.
5. Conferir targets em `http://localhost:9090/targets`.
6. Abrir a caixa de entrada do Gmail usada como destino dos alertas.

## Testes

### Carga HTTP

Use o laboratorio de ataques ou:

```bash
ab -n 20000 -c 100 http://localhost:8080/api/trabalho
```

### Login Falho

```bash
for i in {1..6}; do curl -i http://localhost:8080/api/simular/login-falho; done
```

### Vulnerabilidade

```bash
curl http://localhost:8080/api/simular/vulnerabilidade
curl http://localhost:8080/api/simular/vulnerabilidade/resolver
```

### Alteracao de Configuracao

```bash
curl http://localhost:8080/api/simular/configuracao
```

## Roteiro do Video

1. Apresentar a arquitetura.
2. Mostrar `docker compose ps`.
3. Navegar pelo front principal.
4. Rodar cenarios no laboratorio de ataques.
5. Mostrar metricas no Grafana.
6. Mostrar alertas no Prometheus.
7. Mostrar o e-mail recebido no Gmail.
