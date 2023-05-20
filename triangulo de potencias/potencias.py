import math,cmath


amostras_corrente = [220, 230, 225, 218, 222]  
amostras_tensao = [10, 3, 2, 5, 9]  

####potencia aparente é o modulo de s |s|, presisamos do Vrms e Irms somente , e para fazermos isso so precisamos das amostras de tensão# 


def calcular_Vrms(amostras_tensao):
    soma_quadrados = sum(v**2 for v in amostras_tensao)
    media_quadrados = soma_quadrados / len(amostras_tensao)
    Vrms = math.sqrt(media_quadrados)
    return Vrms

Vrms = calcular_Vrms(amostras_tensao)
print("O valor eficaz da tensão é:", Vrms, "Volts")

def calcular_Irms(amostras_corrente):
    
    soma_quadrados = sum(v**2 for v in amostras_corrente)
    media_quadrados = soma_quadrados / len(amostras_corrente)
    Irms = math.sqrt(media_quadrados)
    return Irms

Irms = calcular_Irms(amostras_corrente)
print("O valor eficaz da corrente é:", Irms, "amperes")

def potencia_aparente(Vrms, Irms):
    
    potencia_aparente = Vrms * Irms
    return potencia_aparente

aparente = potencia_aparente(Vrms, Irms)

print("A potência aparente é:", aparente, "VA (Volt-Ampere)")


######potencia ativa#######pata fazer ela precisamos do angulo de fase e fator de potencia precisamos da 

#####angulo de fase#####

def calcular_angulo_fase(tensoes, correntes):
    soma_produto = 0
    soma_tensao_quadrado = 0
    soma_corrente_quadrado = 0

    for t in range(len(tensoes)):
        soma_produto += tensoes[t] * correntes[t]
        soma_tensao_quadrado += tensoes[t] ** 2
        soma_corrente_quadrado += correntes[t] ** 2

    angulo_rad = math.acos(soma_produto / math.sqrt(soma_tensao_quadrado * soma_corrente_quadrado))

    angulo_graus = math.degrees(angulo_rad)

    return angulo_graus

def calcular_fator_potencia(tensoes, correntes):
    soma_produto = 0
    soma_tensao_quadrado = 0
    soma_corrente_quadrado = 0

    for t in range(len(tensoes)):
        soma_produto += tensoes[t] * correntes[t]
        soma_tensao_quadrado += tensoes[t] ** 2
        soma_corrente_quadrado += correntes[t] ** 2

    angulo_rad = math.acos(soma_produto / math.sqrt(soma_tensao_quadrado * soma_corrente_quadrado))

    fator_potencia = abs(math.cos(angulo_rad))

    return fator_potencia


fator_potencia = calcular_fator_potencia(amostras_tensao, amostras_corrente)
print("Fator de Potência:", fator_potencia)


angulo_fase = calcular_angulo_fase(amostras_tensao, amostras_corrente)
print("Ângulo de Fase:", angulo_fase, "graus")

def calcular_potencia_ativa(amostras_tensao, amostras_corrente,fator_potencia):
    for t in range(len(amostras_tensao)):
        x = amostras_tensao[t] * amostras_corrente[t]*fator_potencia
    potencia_ativa = x * fator_potencia
    return potencia_ativa

potencia_ativa = calcular_potencia_ativa(amostras_tensao, amostras_corrente,fator_potencia)
print("Potência Ativa:", potencia_ativa, "Watts")



####reativa####
def calcular_potencia_reativa(ativa, aparente):
    if aparente >= ativa:
        potencia_reativa = math.sqrt((aparente**2) - (ativa**2))
        return potencia_reativa
    else:
        potencia_reativa = cmath.sqrt((aparente**2) - (ativa**2))
        return potencia_reativa

potencia_reativa = calcular_potencia_reativa(potencia_ativa,aparente)
print("Potência Reativa:", potencia_reativa, "VAR")


