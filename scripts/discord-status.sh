#!/usr/bin/env bash

# 1. Chequeamos si existe algún proceso que contenga 'vesktop' o 'discord'
# Usamos -f para buscar en la línea de comandos completa, no solo en el nombre del binario
if ! pgrep -f "vesktop" >/dev/null; then
  echo "󰙯 Off"
  exit 0
fi

# 2. Obtener notificaciones desde el título de la ventana
# Buscamos por la clase 'vesktop' o 'Vesktop' (fijate con xprop si cambia)
NOTIFS=$(xdotool search --classname "vesktop" getwindowname 2>/dev/null | grep -oP '\(\d+\)' | head -n 1 | tr -d '()')

if [ -z "$NOTIFS" ] || [ "$NOTIFS" -eq 0 ]; then
  echo "󰙯 On"
else
  # Si tienes mensajes, se verá como: 󰙯 3
  echo "󰙯 $NOTIFS"
fi
