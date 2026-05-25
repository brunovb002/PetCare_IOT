# 🐾 PetCareAI+ — Monitor Inteligente de Saúde Animal

> Projeto desenvolvido para o \*\*Challenge FIAP 2026\*\* em parceria com a \*\*CLYVO VET\*\*  
> Disciplina: Disruptive Architectures: IoT, IOB \& Generative IA

\---

## 📋 Índice

* [Sobre o Projeto](#sobre-o-projeto)
* [Problema que Resolve](#problema-que-resolve)
* [Solução Proposta](#solução-proposta)
* [Arquitetura](#arquitetura)
* [Sensores e Componentes](#sensores-e-componentes)
* [Tecnologias Utilizadas](#tecnologias-utilizadas)
* [Alertas Inteligentes](#alertas-inteligentes)
* [Como Executar](#como-executar)
* [Simulação Wokwi](#simulação-wokwi)
* [Dashboard](#dashboard)
* [Próximos Passos](#próximos-passos)
* [Integrantes](#integrantes)

\---

## 📌 Sobre o Projeto

O **PetCareAI+** é um sistema IoT de monitoramento contínuo de saúde animal que simula um **wearable veterinário**. O dispositivo coleta sinais vitais do pet e dados do ambiente em tempo real, enviando alertas inteligentes ao tutor quando algum parâmetro sair da faixa saudável.

O projeto faz parte do ecossistema **CLYVO VET**, que busca transformar a jornada de saúde do pet de um modelo reativo e episódico para uma experiência **contínua, preventiva e inteligente**.

\---

## 🔍 Problema que Resolve

A jornada de saúde do pet é fragmentada. O tutor só interage com o ecossistema veterinário em situações de urgência, gerando:

* **Baixa recorrência** — consultas e exames preventivos são esquecidos
* **Agravamento evitável** — problemas detectados tarde
* **Vínculo fraco** — sem histórico longitudinal entre clínica e tutor
* **Menor LTV** — oportunidades de acompanhamento perdidas

\---

## 💡 Solução Proposta

Um sistema IoT composto por:

* **Wearable simulado** (ESP32 + sensores) monitorando sinais vitais do pet
* **Sensor ambiental** monitorando o ambiente onde o pet vive
* **Sensor de vasilha** detectando automaticamente quando o pet come e bebe água
* **Dashboard em tempo real** com alertas inteligentes cruzando múltiplos dados
* **Persistência histórica** em banco de dados MySQL

\---

## 🏗️ Arquitetura

```
ESP32 (Wokwi)
    │
    │ MQTT (TLS)
    ▼
HiveMQ Cloud (Broker)
    │
    │ Subscribe
    ▼
Node-RED
    ├── Dashboard em tempo real (gauges, gráficos)
    ├── Motor de Alertas (cruzamento de dados)
    ├── Detector de Vasilha (sensor de consumo)
    └── MySQL (persistência histórica)
```

\---

## 🔧 Sensores e Componentes

|Sensor|Pino|Mede|Faixa Saudável|
|-|-|-|-|
|DHT22 (corporal)|D15|Temperatura corporal|37.5°C – 39.2°C|
|DHT22 (ambiente)|D4|Temperatura e umidade ambiente|—|
|Potenciômetro|D34|Frequência cardíaca (BPM)|60 – 140 BPM|
|Potenciômetro|D35|Hidratação (%)|> 60%|
|Potenciômetro|D32|Nível de atividade (%)|> 20%|
|Potenciômetro|D36|Nível vasilha de comida (%)|—|
|Potenciômetro|D39|Nível vasilha de água (%)|—|
|Botão verde|D18|Registro manual — pet comeu|—|
|Botão azul|D19|Registro manual — pet bebeu água|—|

> ⚠️ Em produção real, o potenciômetro de hidratação seria substituído por um sensor de bioimpedância cutânea, o de atividade por um acelerômetro MPU6050 e o de frequência cardíaca por um sensor de pulso (MAX30102).

\---

## 🛠️ Tecnologias Utilizadas

|Tecnologia|Uso|
|-|-|
|ESP32|Microcontrolador principal|
|Wokwi|Simulação do circuito|
|PlatformIO + VS Code|Compilação e desenvolvimento|
|MQTT (TLS 8883)|Protocolo de comunicação|
|HiveMQ Cloud|Broker MQTT na nuvem|
|Node-RED|Processamento e dashboard|
|MySQL|Persistência de dados|
|C++ (Arduino)|Firmware do ESP32|

\---

## 🚨 Alertas Inteligentes

O sistema cruza múltiplos dados para gerar alertas contextualizados:

|Condição|Alerta|
|-|-|
|Temperatura corporal > 39.5°C|🌡️ FEBRE|
|Temperatura corporal < 37.5°C|🥶 HIPOTERMIA|
|BPM > 160|❤️ TAQUICARDIA|
|BPM < 60|❤️ BRADICARDIA|
|Hidratação < 40%|💧 DESIDRATAÇÃO|
|Temp alta + atividade baixa + ambiente quente|🔥 ESTRESSE TÉRMICO|
|Febre + prostração|⚠️ ALERTA CRÍTICO|
|Desidratação + prostração|⚠️ ALERTA CRÍTICO|
|Vasilha caiu > 5%|✅ Pet comeu / bebeu água|

\---

## 🔗 Simulação Wokwi

Visualização online do circuito (referência):  
👉 https://wokwi.com/projects/464927875055063041

Para rodar localmente com dados em tempo real, siga as instruções em [Como Executar](#como-executar).

## 🚀 Como Executar

### Pré-requisitos

* [VS Code](https://code.visualstudio.com/)
* [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) (extensão VS Code)
* [Wokwi for VS Code](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode) (extensão VS Code)
* [Node-RED](https://nodered.org/docs/getting-started/local)
* [MySQL](https://dev.mysql.com/downloads/)

### 1\. Clone o repositório

```bash
git clone https://github.com/brunovb002/PetCare\_IOT.git
cd PetCare\_IOT
```

### 2\. Configure o banco de dados

```sql
CREATE DATABASE IF NOT EXISTS petvital;
USE petvital;

CREATE TABLE leituras\_vitais (
  id INT AUTO\_INCREMENT PRIMARY KEY,
  temperatura\_corporal DECIMAL(4,1),
  frequencia\_cardiaca INT,
  hidratacao DECIMAL(4,1),
  atividade DECIMAL(4,1),
  temperatura\_ambiente DECIMAL(4,1),
  umidade\_ambiente DECIMAL(4,1),
  criado\_em TIMESTAMP DEFAULT CURRENT\_TIMESTAMP
);

CREATE TABLE registro\_tutor (
  id INT AUTO\_INCREMENT PRIMARY KEY,
  evento VARCHAR(20),
  criado\_em TIMESTAMP DEFAULT CURRENT\_TIMESTAMP
);
```

### 3\. Configure as credenciais MQTT

No arquivo `src/main.cpp` substitua:

```cpp
const char\* MQTT\_HOST     = "SEU\_CLUSTER.hivemq.cloud";
const char\* MQTT\_USER     = "SEU\_USUARIO";
const char\* MQTT\_PASSWORD = "SUA\_SENHA";
```

### 4\. Compile o projeto

```
Ctrl+Alt+B no VS Code com PlatformIO
```

### 5\. Inicie o Node-RED

```bash
node-red
```

Acesse `http://localhost:1880` e importe o arquivo `flows.json` da pasta `nodered/`.

### 6\. Inicie a simulação

```
Ctrl+Shift+P → Wokwi: Start Simulator
```

### 7\. Acesse o dashboard

```
http://localhost:1880/ui
```

\---

## 🔗 Simulação Wokwi

Acesse a simulação online:  
👉 [https://wokwi.com/projects/464927875055063041](https://wokwi.com/projects/464927875055063041)

\---

## 📊 Dashboard

O dashboard Node-RED exibe em tempo real:

* **Sinais Vitais** — temperatura corporal, frequência cardíaca, hidratação, atividade
* **Ambiente** — temperatura e umidade do ambiente
* **Alertas** — painel inteligente com status do pet
* **Registro do Tutor** — histórico de alimentação e hidratação
* **Histórico** — tabela com últimas leituras salvas no MySQL

\---

## 🔮 Próximos Passos

* \[ ] Substituir potenciômetro de atividade por acelerômetro **MPU6050**
* \[ ] Substituir potenciômetro de hidratação por sensor de **bioimpedância**
* \[ ] Substituir potenciômetro de frequência cardíaca por sensor **MAX30102**
* \[ ] Integração com app mobile (React Native)
* \[ ] Notificações push para o tutor
* \[ ] Gráfico de barras com histórico semanal de alimentação

\---

## 👥 Integrantes

|Nome|RM|
|-|-|
|Bruno Vinicius Barbosa|566366|
|Guilherme de Andrade Martini|566087|
|Raphael Gomes Mancera|562279|

\---

**FIAP 2026 · Challenge CLYVO VET · Disruptive Architectures: IoT, IOB \& Generative IA**

