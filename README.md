📝 ACWatch – Protetor de Tensão Inteligente com Monitoramento Web

Bem-vindo ao repositório oficial do ACWatch, um sistema de proteção elétrica com alma de engenharia! Baseado em ESP32, oferece monitoramento web, controle de relé, RTC, interface intuitiva e possibilidade de expansão com sensores.

🔧 Funcionalidades Principais
⚡ Proteção Ativa: desliga automaticamente quando a tensão sai dos limites seguros

🧠 Religamento Inteligente: espera estabilidade antes de reconectar

🌐 Interface Web: controle via celular ou PC com ajuste de hora, Wi-Fi e histórico

📟 Display OLED (versão Plus): exibição direta de tensão e estado

📘 RTC com bateria: preserva horário e logs após quedas de energia

🧪 Modo Eco (ESP32-C3): sem display, ideal pra embutir em caixas compactas

🖼️ Esquemas Elétricos
🔌 Versão ACWatch Plus – ESP32 Dev Kit
<p align="center"> <img src="acwatch-esquema-plus.jpg" alt="Esquema ACWatch Plus" width="650"/> </p> <p align="center"><em>Figura 1 – Esquema completo com RTC, OLED, relé SSR, botão de modo e sensor ZMPT101B.</em></p>

⚙️ Versão ACWatch Eco – ESP32-C3 Mini
<p align="center"> <img src="acwatch-esquema-eco.jpg" alt="Esquema ACWatch Eco" width="650"/> </p> <p align="center"><em>Figura 2 – Versão simplificada sem display ou RTC. Ideal para projetos de baixo custo.</em></p>

🚀 Como Usar – Passos Iniciais
Ligue o ACWatch pela primeira vez (ou após reset Wi-Fi)

Conecte-se à rede Wi-Fi criada:

SSID: ACWatch_AP

Senha: 12345678

Acesse o painel via navegador:

IP: http://192.168.4.1

📂 Arquivos Importantes
ACWatch-ESP32-DevKit.ino — Sketch principal da versão Plus

ACWatch-ESP32-C3.ino — Versão Eco com RTC online (em desenvolvimento)

ACWatch-ESP32-DevKit-Docs.html — Interface Web completa

ACWatch-ESP32-C3-Docs.html — Interface Web modo Eco

📡 Recomendações para Radioamadores
Filtro EMI na entrada

Relés SSR com Zero-Crossing

Circuito Snubber nos relés comuns

🛠️ Próximas Versões
[x] ACWatch V5: ✅ Concluída e funcional

[ ] ACWatch V6: ⚙️ Sensores de temperatura, umidade e corrente (em desenvolvimento)

📜 Licença
Projeto de código aberto. Use, modifique, compartilhe e melhore! Créditos Billy Dorsch
