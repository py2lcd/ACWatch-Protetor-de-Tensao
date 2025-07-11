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
---

### 🔌 Esquema ACWatch com Relé SSR

<p align="center">
  <img src="ACWatch_Plus_SSR.jpg" alt="Esquema com Relé SSR" width="650"/>
</p>
<p align="center"><em>Figura 1 – Versão com relé SSR e RTC, ideal para acionamentos silenciosos e alta durabilidade.</em></p>

---

### ⚙️ Esquema ACWatch com Módulo de 2 Relés Mecânicos

<p align="center">
  <img src="ACWatch_Plus_2CH_Relay.jpg" alt="Esquema com Relé 2CH" width="650"/>
</p>
<p align="center"><em>Figura 2 – Versão utilizando módulo de 2 relés mecânicos com controle separado por GPIOs.</em></p>

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
