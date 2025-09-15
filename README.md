# Algoritmo Genético - Distribuição de Carga

**Vencimento:** domingo, 5 out. 2025, 23:59  

---

## Algoritmos Genéticos - Problema da Distribuição de Carga num Avião

Usando algoritmos genéticos, resolva o problema da distribuição de carga num avião.  

Em anexo, foram adicionados slides para explicar melhor o problema descrito abaixo.

---

## Descrição

O problema da **distribuição de carga num avião** é um problema de otimização que consiste em distribuir a carga de um avião de forma a **maximizar a sua eficiência**.  

A carga é composta por diversos itens, cada um com um **peso** e um **volume** específico.  
O avião tem um limite de peso e volume que pode transportar, e o objetivo é distribuir a carga de forma a **maximizar a eficiência** do avião.  

O avião de carga possui **três compartimentos**:

| Compartimento | Volume (m³) | Peso (tonelada) |
|---------------|-------------|-----------------|
| Dianteiro (D) | 6800        | 10              |
| Central (C)   | 8700        | 16              |
| Traseiro (T)  | 5300        | 8               |

> Para manter o equilíbrio do avião, a carga em cada compartimento deve ser distribuída de forma a manter o **peso da carga proporcional ao volume disponível**.

---

## Formulação do Problema

Existem **4 cargas** a serem distribuídas nos compartimentos do avião.  
Cada carga possui **peso**, **volume** e proporciona um **lucro específico** à empresa que transporta a carga.

| Carga | Volume (m³/ton) | Peso (tonelada) | Lucro (R$/tonelada) |
|-------|-----------------|-----------------|---------------------|
| C1    | 480             | 18              | 310                 |
| C2    | 650             | 15              | 380                 |
| C3    | 580             | 23              | 350                 |
| C4    | 390             | 12              | 285                 |

### Objetivo

Determinar quanto de cada carga (**C1, C2, C3, C4**) deve ser alocado em cada compartimento (**D, C, T**) de forma a **maximizar o lucro total da empresa**.

---

## Restrições

- Cada carga pode ser dividida em quantas partes forem necessárias, desde que em frações ≥ **1 kg**.  
- Cada carga pode ser distribuída em dois ou mais compartimentos.  
- Quaisquer cargas podem ser alocadas em qualquer compartimento.  

---

## Implementação

Implemente um **algoritmo genético** para resolver o problema da distribuição de carga no avião.  

O algoritmo deve ser capaz de determinar a **quantidade de cada carga em cada compartimento** de forma a **maximizar o lucro total** da empresa.
