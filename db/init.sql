CREATE TABLE IF NOT EXISTS metricas_servico (
    id BIGSERIAL PRIMARY KEY,
    servico VARCHAR(80) NOT NULL,
    tipo_metrica VARCHAR(80) NOT NULL,
    valor DOUBLE PRECISION NOT NULL,
    unidade VARCHAR(20),
    coletado_em TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS incidentes (
    id BIGSERIAL PRIMARY KEY,
    servico VARCHAR(80) NOT NULL,
    severidade VARCHAR(20) NOT NULL,
    descricao TEXT NOT NULL,
    status VARCHAR(20) NOT NULL DEFAULT 'aberto',
    criado_em TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    resolvido_em TIMESTAMPTZ
);

INSERT INTO metricas_servico (servico, tipo_metrica, valor, unidade)
VALUES
    ('web-server', 'disponibilidade', 1, 'up'),
    ('database', 'disponibilidade', 1, 'up'),
    ('database', 'conexoes_abertas', 1, 'count');
