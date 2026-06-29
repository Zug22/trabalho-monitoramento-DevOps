# Checklist de Entrega

## Pronto

- Front-end responsivo em `http://localhost:8080`.
- Laboratorio de ataques em `http://localhost:8080/ataques`.
- Backend C++ containerizado.
- PostgreSQL com schema inicial.
- Prometheus coletando metricas.
- Grafana provisionado automaticamente.
- Alertmanager configurado para envio real via Gmail SMTP.
- Blackbox Exporter monitorando HTTP, DNS e SMTP.
- Documentacao em `docs/`.

## Antes de Entregar

- Criar `secrets/gmail_app_password.txt` com a senha de app do Gmail.
- Confirmar que todos os containers estao `running`.
- Disparar um alerta e conferir o e-mail real no Gmail.
- Compilar o PDF final fora do repositório com o texto atualizado para Gmail.
- Gravar o video de demonstracao.
- Subir o repositorio no GitHub.
