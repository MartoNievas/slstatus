#!/usr/bin/env bash

# 1. Chequeo rápido de Systemd
if ! systemctl is-active --quiet bluetooth; then
  echo "󰂲 Off"
  exit 0
fi

# 2. Chequeo del Adaptador (Solo si el servicio está vivo)
# Usamos bluetoothctl de forma que si falla el comando, no asuma que está ON
STATUS=$(bluetoothctl show | grep "Powered:" | awk '{print $2}')

if [ "$STATUS" = "yes" ]; then
  # Intentamos ver si hay algo conectado
  DEVICES=$(bluetoothctl info | grep "Name" | cut -d ' ' -f 2-)

  if [ -z "$DEVICES" ]; then
    echo "󰂯 On"
  else
    echo "󰂱 ${DEVICES:0:10}"
  fi
else
  # Si no es "yes", está apagado o en transición
  echo "󰂲 Off"
fi
