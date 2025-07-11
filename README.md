ACWatch - Protetor de Tensão Inteligente com Monitoramento Web
Bem-vindo ao repositório oficial do ACWatch, um protetor de tensão inteligente, de código aberto, projetado para monitorar e proteger seus equipamentos eletrônicos contra variações na rede elétrica. O projeto oferece uma interface web completa para monitoramento e configuração em tempo real.

(Insira aqui uma foto do seu projeto montado!)

Funcionalidades Principais
Proteção Ativa: Desliga automaticamente os equipamentos quando a tensão sai dos limites seguros definidos por você.

Religamento Inteligente: Aguarda um período de estabilidade (10 segundos) antes de religar os equipamentos.

Monitoramento Web: Acesse um painel de controle completo pelo navegador do seu celular ou computador para ver a tensão, o modo de operação e o estado do relé em tempo real.

Parâmetros Ajustáveis: Configure os limites de tensão mínima e máxima para os modos 110V e 220V diretamente pela interface web.

Log de Falhas: Mantém um histórico de todas as vezes que a proteção foi ativada, acessível pela página web.

Backup de Energia (Opcional): Arquitetura pronta para a implementação de um sistema de backup com baterias.

Como Usar (Primeiros Passos)
Ao ligar o ACWatch pela primeira vez ou após resetar as configurações de Wi-Fi, ele entrará em Modo Ponto de Acesso (AP).

No seu celular ou computador, procure por uma nova rede Wi-Fi.

Conecte-se à rede com os seguintes dados:

Nome da Rede (SSID): ACWatch_AP

Senha: 12345678

Após conectar, abra o navegador e acesse o endereço:

IP do Painel: http://192.168.4.1

Você será direcionado para o painel de controle principal.

Manual da Interface Web
(Esta seção será detalhada com os esquemas elétricos e manuais de uso.)

Painel Principal: Mostra a tensão atual, o modo de operação (110V/220V) e o estado do relé (Ligado/Desligado).

Configurar Wi-Fi: Use este botão para procurar as redes Wi-Fi da sua casa ou sítio e inserir a senha. Após salvar, o ACWatch irá reiniciar e se conectar à sua rede, desativando o Modo AP.

Ver Falhas: Acessa a página com o histórico de todas as vezes que a proteção de tensão foi ativada.

Ajustar Data e Hora / Parâmetros: As seções recolhíveis permitem configurar o relógio (em modo AP) e os limites de tensão para a proteção.

Versões do Projeto
Este repositório contém duas versões do ACWatch, cada uma com um foco diferente.

1. ACWatch Plus (ESP32 Dev Kit)




Status:

✅ Estável e Testado

Placa:

ESP32 Dev Kit (com 38 pinos)

Recursos:

Todas as funcionalidades, incluindo Display OLED e Relógio de Tempo Real (RTC) com bateria.

Esta é a versão completa do projeto, ideal para quem quer uma estação de monitoramento com display físico que mostra as informações o tempo todo. A presença do RTC garante que o horário e os logs de falha nunca se percam, mesmo após longas quedas de energia.

Ver Documentação e Esquema de Ligações

Ver Código-Fonte (Sketch Arduino)

2. ACWatch Eco (ESP32-C3 Mini)




Status:

⚠️ Em Desenvolvimento (Não Testado)

Placa:

ESP32-C3 Mini (ou similar)

Recursos:

Focado em baixo custo e eficiência. Não utiliza display ou RTC. A hora é sincronizada via internet (NTP).

Esta é uma versão mais compacta e econômica, ideal para ser embutida em painéis ou caixas menores. Ela remove os componentes físicos de display e relógio, dependendo 100% da interface web para monitoramento e configuração. Possui um pino de alerta dedicado para um LED ou buzzer externo.

Ver Documentação e Esquema de Ligações

Ver Código-Fonte (Sketch Arduino)

Recomendações para Radioamadores

Para minimizar a geração de ruído (QRM/RFI), especialmente se estiver usando relés eletromecânicos comuns (módulos azuis), considere as seguintes melhorias:

Filtro EMI: Instale um filtro EMI na entrada de energia do projeto. Isso limpará o ruído vindo da rede e impedirá que o ruído gerado pelo seu projeto contamine a fiação da sua estação.

Circuito Snubber: Para relés eletromecânicos, adicione um circuito snubber (resistor + capacitor) em paralelo com a carga de alta tensão para suprimir a faísca gerada na comutação.

Relés de Estado Sólido (SSR): A melhor opção para um ambiente de rádio "silencioso" é usar SSRs com comutação "Zero-Crossing", como os utilizados no desenvolvimento deste projeto.

Próximos Passos (Ideias para o Futuro)
[ ] Integração com sistema de energia solar para gerenciamento de carga.

[ ] Adição de um sensor de temperatura e umidade (DHT22).

[ ] Notificações de falha via Wi-Fi (MQTT, Telegram, etc.).

[ ] Monitoramento de consumo de corrente e potência.

Licença
Este projeto é de código aberto. Sinta-se à vontade para usar, modificar e compartilhar.
