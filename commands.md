# XPAD PLUS - Comandos de Comunicação

## Visão Geral

O XPAD PLUS aceita comandos através de múltiplas interfaces de comunicação:

- **Serial** (UART)
- **Bluetooth Low Energy (BLE)**
- **Ethernet (TCP/IP)**

Os comandos podem ser enviados individualmente ou em lote, utilizando o separador `|` para múltiplos comandos em uma única transmissão.

---

## Comandos Básicos

### Conectividade e Status

| Comando | Descrição | Resposta |
|---------|-----------|----------|
| `#ping` ou `ping` | Testa conectividade | `#PONG` |
| `#get_state` | Obtém estado atual de leitura | `#READING` ou `#IDLE` |
| `#get_info` | Informações do dispositivo | `#NAME:`, `#BT_MAC:`, `#ETH_MAC:`, `#IP:` |
| `#get_serial` | Serial do dispositivo | `#SERIAL:[serial]` |
| `#restart` | Reinicia o dispositivo | - |

### Controle de Leitura

| Comando | Descrição | Resposta |
|---------|-----------|----------|
| `#read:on` ou `readtag on` | Inicia leitura de tags | `#READ:ON` |
| `#read:off` ou `readtag off` | Para leitura de tags | `#READ:OFF` |
| `initreadtag:on` | Inicia leitura automaticamente | - |
| `initreadtag:off` | Desabilita leitura automática | - |

### Gerenciamento de Tags

| Comando | Descrição | Resposta |
|---------|-----------|----------|
| `#get_tags` | Lista epcs detectados | Lista de epcs |
| `#get_tags_all` | Lista todas as informações das tags | Lista completa de tags |
| `#clear` | Limpa buffer de tags | - |

### Escrita de Tags

| Comando | Descrição | Formato |
|---------|-----------|---------|
| `#write:[EPC];[PASSWORD]` | Escreve EPC sem filtro | `#write:000102030405060708090A0B;12345678` |
| `#write:[EPC];[PASSWORD];EPC;[TARGET]` | Escreve EPC com filtro no epc | `#write:000102030405060708090A0B;12345678;epc;AABBCCDDEEFF001122334455` |
| `#write:[EPC];[PASSWORD];TID;[TARGET]` | Escreve EPC com filtro no tid | `#write:000102030405060708090A0B;12345678;tid;E28012341234123412341234` |

**Parâmetros de escrita:**

- **EPC**: 24 caracteres hexadecimais (12 bytes)
- **PASSWORD**: 8 caracteres hexadecimais (4 bytes)
- **TYPE**: `epc` ou `tid`
- **TARGET**: 24 caracteres hexadecimais para filtro

---

## Configuração de Potência e Sessão

| Comando | Descrição | Faixa/Valores |
|---------|-----------|---------------|
| `#read_power:[VALOR]` ou `readpower:[VALOR]` | Define potência de leitura | Conforme especificação |
| `#get_power` | Obtém potência atual | `#POWER:[valor]` |
| `#session:[VALOR]` ou `gen2session:[VALOR]` | Define sessão Gen2 | 0 ao máximo configurado |
| `#get_session` | Obtém sessão atual | `#SESSION:[valor]` |
| `#password[...]` | Altera senha do reader | Conforme protocolo |

---

## Configurações Gerais

### Modo de Operação

| Comando | Descrição | Valores |
|---------|-----------|---------|
| `readmode hid` | Modo teclado (HID) | - |
| `readmode normal` | Modo normal | - |
| `#buzzer:on` | Habilita buzzer | `#BUZZER:ON` |
| `#buzzer:off` | Desabilita buzzer | `#BUZZER:OFF` |
| `#decode_gtin:on` | Habilita decodificação GTIN | - |
| `#decode_gtin:off` | Desabilita decodificação GTIN | - |

### Configuração de Rede

| Comando | Descrição | Valores |
|---------|-----------|---------|
| `#hotspot:on` | Habilita modo hotspot | - |
| `#hotspot:off` | Desabilita modo hotspot | - |

### Configuração de Prefixo

| Comando | Descrição | Exemplo |
|---------|-----------|---------|
| `#prefix:[VALOR]` | Define prefixo para tags | `#prefix:TAG_` |
| `#get_prefix` | Obtém prefixo atual | `#PREFIX:[valor]` |

---

## Comando de Configuração Múltipla

### `#set_cmd:[COMANDOS]`

Permite configurar múltiplas opções simultaneamente usando o separador `|`.

**Comandos disponíveis no set_cmd:**

| Comando | Descrição | Exemplo |
|---------|-----------|---------|
| `set_ant:[N],[ATIVO],[POTÊNCIA],[RSSI]` | Configura antena específica | `set_ant:1,on,30,25` |
| `session:[VALOR]` | Define sessão | `session:1` |
| `read_power:[VALOR]` | Potência de leitura | `read_power:30` |
| `write_power:[VALOR]` | Potência de escrita | `write_power:30` |
| `buzzer:on/off` | Controle do buzzer | `buzzer:on` |
| `gpi_stop_delay:[MS]` | Delay de parada GPI | `gpi_stop_delay:1000` |
| `decode_gtin:on/off` | Decodificação GTIN | `decode_gtin:on` |
| `start_reading:on/off` | Leitura automática | `start_reading:on` |
| `gpi_start:on/off` | Início por GPI | `gpi_start:on` |
| `always_send:on/off` | Envio contínuo | `always_send:on` |
| `simple_send:on/off` | Envio simplificado | `simple_send:on` |
| `keyboard:on/off` | Modo teclado | `keyboard:on` |

**Exemplo de uso:**

```text
#set_cmd:session:1|read_power:30|buzzer:on|start_reading:on
```

**Resposta:** Para cada comando processado:

- `#CMD:[comando]` (log do comando)
- `#SET_CMD:OK` ou `#SET_CMD:NOK`

---

## Comandos em Lote

### Múltiplos Comandos com `|`

É possível enviar vários comandos em uma única transmissão, separados por `|`:

**Exemplo:**

```text
#read:on|#clear|#buzzer:on
```

**Comportamento:**

- Cada comando é processado sequencialmente
- Comandos `#set_cmd:` são tratados de forma especial (não são separados)
- Outros comandos são processados individualmente

---
