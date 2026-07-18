#!/bin/bash

# 1. Definir la estética (Copiado de tu dwm config.h)
FONTE="JetBrainsMono Nerd Font:style=Bold:size=13"
GRI_OSC="#222222" # col_gray1 (Fondo normal)
GRI_CLA="#bbbbbb" # col_gray3 (Texto normal)
NARANJA="#ff800f" # col_cyan  (Fondo seleccionado)
BLANCO="#eeeeee"  # col_gray4 (Texto seleccionado)

# 2. Obtener lista de dispositivos
devices=$(wpctl status | sed -n '/Sinks:/,/Sources:/p' | grep -E '[0-9]+\.' | sed 's/^[[:space:]]*//')

# 3. Lanzar dmenu con los parámetros de apariencia
# -l 10 lo hace vertical para que sea más legible
chosen=$(echo "$devices" | dmenu -i -l 10 -p "Audio Output:" \
  -fn "$FONTE" \
  -nb "$GRI_OSC" \
  -nf "$GRI_CLA" \
  -sb "$NARANJA" \
  -sf "$BLANCO")

# 4. Cambiar el dispositivo
id=$(echo "$chosen" | awk '{print $2}' | tr -d '.')

if [ -n "$id" ]; then
  wpctl set-default "$id"

  for stream in $(pactl list sink-inputs short | awk '{print $1}'); do
    pactl move-sink-input "$stream" "$id" 2>/dev/null
  done
fi
