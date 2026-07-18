#!/bin/bash

# Colores para que se vea bonito en st
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== DIAGNÓSTICO DE SISTEMA ARCH LINUX ===${NC}"

# 1. Chequeo de Servicios Fallidos
FAILED_UNITS=$(systemctl --failed --no-legend | wc -l)
if [ "$FAILED_UNITS" -eq 0 ]; then
  echo -e "[${GREEN}OK${NC}] Servicios Systemd: Todos operativos."
else
  echo -e "[${RED}FAIL${NC}] Servicios fallidos: $FAILED_UNITS"
  systemctl --failed
fi

# 2. Chequeo de Errores Críticos (Journal)
# Filtramos errores de la sesión actual
ERRORS=$(journalctl -p 3 -xb -q | wc -l)
if [ "$ERRORS" -eq 0 ]; then
  echo -e "[${GREEN}OK${NC}] Journal (Logs): Limpio como una patena."
else
  echo -e "[${RED}FAIL${NC}] Errores críticos en este arranque: $ERRORS"
  # Opcional: mostrar los errores si son pocos
  # journalctl -p 3 -xb -n 5
fi

# 3. Archivos .pacnew pendientes
PACNEWS=$(pacdiff -o | wc -l)
if [ "$PACNEWS" -eq 0 ]; then
  echo -e "[${GREEN}OK${NC}] Configuración: No hay archivos .pacnew pendientes."
else
  echo -e "[${YELLOW}WARN${NC}] Tienes $PACNEWS archivos .pacnew por revisar."
  echo "      Ejecuta 'pacdiff' pronto."
fi

# 4. Zombies (Importante para dwm)
ZOMBIES=$(ps -eo stat | grep -c '^Z')
if [ "$ZOMBIES" -eq 0 ]; then
  echo -e "[${GREEN}OK${NC}] Procesos: No hay zombies devorando recursos."
else
  echo -e "[${YELLOW}WARN${NC}] Detectados $ZOMBIES procesos zombie."
  ps aux | grep 'Z' | grep -v grep
fi

# 5. Estado de la partición Root
DISK_USAGE=$(df / --output=pcent | tail -1 | tr -dc '0-9')
if [ "$DISK_USAGE" -lt 85 ]; then
  echo -e "[${GREEN}OK${NC}] Disco (/): Uso saludable del $DISK_USAGE%."
else
  echo -e "[${RED}ALERT${NC}] Disco (/): Espacio crítico! Uso del $DISK_USAGE%."
fi

echo -e "${BLUE}=== FIN DEL DIAGNÓSTICO ===${NC}"
