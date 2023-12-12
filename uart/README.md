USART To Do List:
- ~~Return usart_handle_t~~
- ~~Create usart_rx and usart_tx tasks.~~
- Use USART as loopback RX -> TX
    - Use queue to send data from usart_rx to usart_tx
    - Blocking RX/TX calls at first.
    - Therefore echo back on terminal display.
- Maybe change color?
    - '\x1B' is escape sequence, therefore one char.
- Update FreeRTOSConfig.h
