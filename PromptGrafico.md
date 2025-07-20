Forma de leitura:

Análise Climática Comparativa (Temperatura e Umidade)
O resultado é um painel com dois gráficos alinhados. O de cima permite que você compare a temperatura entre as três cidades, e o de baixo permite a comparação da umidade. As cores das cidades são as mesmas nos dois gráficos para manter a consistência. Este modelo torna a comparação entre as cidades muito mais direta e clara.

O Dashboard:
Enquanto os gráficos anteriores mostravam a variação ao longo do tempo, este novo Dashboard de Distribuição resume as condições climáticas de cada cidade, sendo ideal para comparar o perfil geral de cada uma.
Dashboard de Distribuição Climática
Este painel utiliza gráficos de caixa (box plots) para mostrar a faixa de valores de temperatura e umidade em cada cidade.
A caixa representa 50% de todos os dados medidos (do 1º ao 3º quartil).
A linha dentro da caixa é a mediana (o valor do meio).
As "hastes" (whiskers) mostram a maior parte do restante dos dados.
Os pontos fora das hastes podem ser considerados valores atípicos (outliers).
Com este dashboard, você pode responder rapidamente a perguntas como:
Qual cidade teve a maior variação de temperatura?
Qual cidade é, em geral, mais úmida?
A temperatura em Belo Monte é mais estável que em Arapiraca?


Prompt para usar em uma IA:

Prompt para Dashboard Climático Abrangente e Comparativo
Objetivo: Desenvolver um dashboard de Business Intelligence (BI) completo e interativo para a análise climática comparativa entre múltiplas cidades. O painel deve apresentar duas visualizações principais: uma para análise de tendências ao longo do tempo e outra para a análise da distribuição estatística dos dados.

Título do Dashboard: Análise Climática Detalhada: Belo Monte, Arapiraca e União dos Palmares

Componente 1: Análise de Tendências Temporais
Esta seção focará na comparação das métricas ao longo do tempo.

Layout: Dois gráficos de linha empilhados verticalmente, com eixo de tempo (Eixo X) sincronizado.

Gráfico 1 (Superior) - Comparativo de Temperatura:

Tipo: Gráfico de linhas.

Conteúdo: Uma linha para cada cidade, plotando a Temperatura (°C). Cada cidade deve ser representada por uma cor única e consistente em todo o dashboard.

Título: Variação da Temperatura (°C).

Legenda: Identificando a cor de cada cidade.

Gráfico 2 (Inferior) - Comparativo de Umidade:

Tipo: Gráfico de linhas.

Conteúdo: Uma linha para cada cidade, plotando a Umidade Relativa (%). As cores devem ser as mesmas do gráfico de temperatura.

Título: Variação da Umidade Relativa (%).

Legenda: Idêntica à do gráfico superior.

Componente 2: Análise de Distribuição Estatística
Esta seção focará no resumo do perfil climático de cada cidade.

Layout: Dois gráficos de caixa (box plots) empilhados verticalmente.

Gráfico 3 (Superior) - Distribuição de Temperatura:

Tipo: Gráfico de Caixa (Box Plot).

Conteúdo: Um "box" para cada cidade, mostrando a distribuição estatística (mediana, quartis, outliers) dos dados de Temperatura (°C).

Título: Distribuição das Temperaturas por Cidade.

Gráfico 4 (Inferior) - Distribuição de Umidade:

Tipo: Gráfico de Caixa (Box Plot).

Conteúdo: Um "box" para cada cidade, mostrando a distribuição dos dados de Umidade Relativa (%).

Título: Distribuição da Umidade por Cidade.

Requisitos de Interatividade e Estilo:
Filtro de Cidades Global: Um único filtro (checkboxes ou botões) no topo do dashboard que permita ao usuário selecionar/desselecionar cidades, atualizando TODOS os quatro gráficos simultaneamente.

Tooltips Detalhados: Ao passar o mouse sobre os gráficos de linha (Componente 1), exibir data, hora, cidade, temperatura e umidade. Ao passar sobre os box plots (Componente 2), exibir os valores estatísticos (mediana, máximo, mínimo, quartis).

Design Profissional: Paleta de cores coesa, fundo limpo, fontes legíveis e títulos claros em todos os componentes para garantir uma leitura fácil e intuitiva.
