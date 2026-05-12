```markdown id="a65hkg"
# Roteiro — Vídeo Explicativo Ir Além 2 (Até 4 Minutos)

# Título do Vídeo
CardioIA — Comparação entre Regressão Logística e Modelo Neuromórfico LIF

---

# [00:00 – 00:20] Abertura

Olá, meu nome é Raphael e neste vídeo vou apresentar o desenvolvimento do módulo Ir Além 2 do projeto CardioIA.

O objetivo deste experimento foi comparar dois modelos de Inteligência Artificial aplicados à classificação de riscos em séries temporais de saúde.

Os modelos utilizados foram:

- Regressão Logística;
- Leaky Integrate-and-Fire, conhecido como LIF.

---

# [00:20 – 00:55] Contextualização do Projeto

O CardioIA simula um sistema inteligente de monitoramento de saúde inspirado em dispositivos vestíveis, como smartwatches.

A base de dados utilizada foi sintética e contém informações como:

- temperatura;
- frequência cardíaca;
- nível de movimento;
- detecção de movimento;
- alerta de emergência;
- nível de risco.

Os riscos foram organizados em quatro classes:

- normal;
- medium;
- high;
- critical.

O projeto foi desenvolvido em Python utilizando Google Colab e Google Drive.

---

# [00:55 – 01:35] Tratamento dos Dados

Na primeira etapa, foi realizado o carregamento e tratamento dos dados.

As colunas foram padronizadas e os valores passaram por:

- conversão numérica;
- tratamento de valores ausentes;
- normalização;
- organização temporal.

Depois disso, os registros foram organizados em janelas temporais.

Essas janelas permitem que o modelo analise o comportamento dos sinais ao longo do tempo, em vez de analisar apenas valores isolados.

Foram extraídas características como:

- média;
- desvio padrão;
- máximo;
- mínimo;
- inclinação;
- amplitude.

---

# [01:35 – 02:15] Regressão Logística

O primeiro modelo utilizado foi a Regressão Logística.

Esse modelo é uma abordagem tradicional de Machine Learning muito utilizada em problemas de classificação.

Ele recebeu as características estatísticas extraídas das janelas temporais e foi treinado para identificar os níveis de risco.

Entre as principais vantagens da Regressão Logística estão:

- simplicidade;
- baixo custo computacional;
- facilidade de interpretação.

Após o treinamento, o modelo apresentou excelente desempenho na classificação das amostras sintéticas.

---

# [02:15 – 03:00] Modelo Neuromórfico LIF

O segundo modelo foi o Leaky Integrate-and-Fire, ou LIF.

Esse modelo é inspirado no comportamento de neurônios biológicos.

O sistema transforma sinais fisiológicos em correntes elétricas simuladas.

Essas correntes são acumuladas em um potencial de membrana.

Quando o potencial ultrapassa um limite, ocorre um spike, que representa um disparo neuronal.

A partir disso, foram extraídas características como:

- quantidade de spikes;
- tensão média;
- corrente média;
- inclinação da corrente.

Essas características foram utilizadas para classificar os níveis de risco.

---

# [03:00 – 03:40] Resultados Obtidos

Os resultados mostraram que os dois modelos alcançaram desempenho máximo nas métricas avaliadas.

Foram analisadas métricas como:

- Accuracy;
- Precision;
- Recall;
- F1-score.

Além disso, as matrizes de confusão mostraram que todas as amostras foram classificadas corretamente.

A Regressão Logística apresentou maior simplicidade e facilidade de implementação.

Já o modelo LIF agregou uma abordagem bioinspirada e experimental baseada em computação neuromórfica.

---

# [03:40 – 04:00] Encerramento

Conclui-se que o experimento foi importante para comparar abordagens tradicionais e neuromórficas aplicadas à saúde.

O projeto também demonstrou a construção completa de um pipeline de Inteligência Artificial para séries temporais.

Como próximos passos, o sistema poderá ser expandido com:

- datasets maiores;
- dados mais realistas;
- novos modelos de IA;
- integração com dispositivos reais.

Obrigado pela atenção.
```
